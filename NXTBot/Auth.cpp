#include "pch.h"
#include "sodium.h"
#include "XSocket.h"
#include "Auth.h"

class Hwid
{

public:

	Hwid() = default;

	bool Query();
	inline const std::string& getCPU() const;
	inline const std::string& getComputerName() const;
	inline const std::string& getPhysicalHddSerial() const;
	inline const std::string& getUUID() const;

	inline const std::string& GetHwid() const;

private:

	static bool QueryWmic(const std::string& input, std::string& out);

protected:

	std::string m_CPU;
	std::string m_ComputerName;
	std::string m_Physical;
	std::string m_UUid;
};

inline const std::string& Hwid::getCPU() const
{
	return m_CPU;
}

inline const std::string& Hwid::getComputerName() const
{
	return m_ComputerName;
}

inline const std::string& Hwid::getPhysicalHddSerial() const
{
	return m_Physical;
}

inline const std::string& Hwid::getUUID() const
{
	return m_UUid;
}


#define BlockSize 1024
#define TIMEOUT 15000 // 15 seconds time out

// Socket
CXSocket ClientSocket;

// Bob
unsigned char Server_PublicKey[crypto_box_PUBLICKEYBYTES];

// Alice
unsigned char Client_PublicKey[crypto_box_PUBLICKEYBYTES];
unsigned char Client_SecretKey[crypto_box_SECRETKEYBYTES];



void print_bytes(const char* title, const unsigned char* data, size_t dataLen, bool format = true) {
	std::cout << title << std::endl;
	std::cout << std::setfill('0');
	for (size_t i = 0; i < dataLen; ++i) {
		std::cout << std::hex << std::setw(2) << (int)data[i];
		if (format) {
			std::cout << (((i + 1) % 16 == 0) ? "\n" : " ");
		}
	}
	std::cout << std::endl;
}

unsigned char nonceV[crypto_box_NONCEBYTES] = { 0 };

unsigned char* GenerateNonce()
{
	randombytes_buf(nonceV, sizeof nonceV);
	return nonceV;
}


unsigned char* PerformNonceExchange()
{
	// Reset nLen
	int nLen = 0;

	auto nonce = GenerateNonce();

	// Send server our Nonce
	if (ClientSocket.Send((char*)nonce, crypto_box_NONCEBYTES, nLen, TIMEOUT) != E_XSOCKET_SUCCESS)
		return false;

	print_bytes("\n====== [Client Nonce] ======", nonce, crypto_box_NONCEBYTES);

	return nonce;
}


bool SendAuthentication(std::string Username, std::string Password, std::string HWID)
{
	// Space as delimiter meaning NO SPACE in username or password or HWID

	std::string auth_token = Username + ' ' + Password + ' ' + HWID;

	//auth_token = Username + ' ' + Password;

	// Generate and send our nonce as well as receive nonce from server
	auto nonce = PerformNonceExchange();

	unsigned char Cipher[BlockSize];

	sodium_memzero(Cipher, BlockSize);

	if (crypto_box_easy(Cipher, (unsigned char*)auth_token.data(), auth_token.length(), nonce,
		Server_PublicKey, Client_SecretKey) != 0) {
		printf("[-] Encryption Error.\n");
		return false;
	}

	WORD MessageLength = auth_token.length() + crypto_box_MACBYTES + 2; // Byte for size WORD

	memcpy((void*)(Cipher + 2), Cipher, MessageLength);

	Cipher[0] = 0;
	Cipher[1] = 0;

	memcpy(Cipher, &MessageLength, 2);

	//Cipher[0] = MessageLength;

	//auth_token[0] = MessageLength;


	int nLen = 0;

	// Send server our Cipher message
	if (ClientSocket.Send((char*)Cipher, BlockSize, nLen, TIMEOUT) != E_XSOCKET_SUCCESS)
		return false;

	//printf("-------------------------------------------------");

	// Divide by 4 because we don't need to see that many zeros lmao
	print_bytes("\n====== [Cipher Message] ======", Cipher, BlockSize / 16);

	return true;
}

bool PerformKeyExchange()
{
	// Generate Keypair
	crypto_box_keypair(Client_PublicKey, Client_SecretKey);

	print_bytes("====== |Public Key| ======", Client_PublicKey, crypto_box_PUBLICKEYBYTES);

	print_bytes("====== |Private Key| ======", Client_SecretKey, crypto_box_SECRETKEYBYTES);

	printf("-------------------------------------------------\n\n");

	int nLen = 0;

	// Send server our public Key
	if (ClientSocket.Send((char*)Client_PublicKey, crypto_box_PUBLICKEYBYTES, nLen, TIMEOUT) != E_XSOCKET_SUCCESS)
		return false;

	// Make sure we securely zero 
	sodium_memzero(Server_PublicKey, crypto_box_PUBLICKEYBYTES);

	// Reset nLen
	nLen = 0;

	do
	{
		// Receive Server's public key
		if (ClientSocket.Recv((char*)Server_PublicKey, crypto_box_PUBLICKEYBYTES, nLen, TIMEOUT)
			!= E_XSOCKET_SUCCESS)
		{
			printf("Public key receiving error (%d)\n", GetLastError());
			return 0;
		}

	} while (sodium_is_zero(Server_PublicKey, crypto_box_PUBLICKEYBYTES));

	if (nLen == 0)
	{
		printf("[-] Failed to receive server's public key.\n");
		return false;
	}

	print_bytes("====== [Server Public Key] ======", Server_PublicKey, crypto_box_PUBLICKEYBYTES);


	return true;
}


BYTE ReceiveAuthentication()
{
	unsigned char Response[BlockSize];

	// Securely zero this out
	sodium_memzero(Response, BlockSize);

	int nLen = 0;

	do
	{
		if (ClientSocket.Recv((char*)Response, BlockSize, nLen, TIMEOUT)
			!= E_XSOCKET_SUCCESS)
		{
			//printf("Server response error (%d)\n", GetLastError());
			ClientSocket.Close();
			return 0;
		}

	} while (sodium_is_zero(Response, BlockSize));

	ClientSocket.Close();

	//print_bytes("====== [Server Auth Response] ======", Response, BlockSize / 16);

	unsigned char decrypted[BlockSize];

	sodium_memzero(decrypted, BlockSize);

	//print_bytes("====== [Server Nonce] ======", (const unsigned char*)Response, crypto_box_NONCEBYTES);

	if (crypto_box_open_easy(decrypted, Response + crypto_box_NONCEBYTES, 5 + crypto_box_MACBYTES, Response, Server_PublicKey, Client_SecretKey))
	{
		//printf("[-] Response from server was forged!\n\n");
		return 0;
	}

	BYTE Status = decrypted[0];

	//print_bytes("\n====== [Decrypted Response] ======", decrypted, BlockSize / 32);

	return Status;
}

CXSocket CoreServerSocket;

char* DownloadCore(DWORD& Size)
{
	// Initalize winsocks
	if (!CoreServerSocket.Init())
	{
		printf("[-] Failed to initialize winsock.\n");
		return 0;
	}

	// Connect to server
	if (!CoreServerSocket.Connect("ec2-3-19-60-200.us-east-2.compute.amazonaws.com", 42070))
	{
		int nError = CoreServerSocket.GetLastError();
		printf("[-] Failed to connect to server with error %d (%p).\n", nError, nError);
		return false;
	}

	unsigned char* Corebuffer = new unsigned char[2 * 1024 * 1024]();
	unsigned char* Chunk = new unsigned char[1 * 1024 * 1024]();

	int nLen = 0;
	unsigned int receivedSize = 0;
	unsigned int expectedFileSize = 0;
	do
	{
		if (CoreServerSocket.Recv((char*)Chunk, 1 * 1024 * 1024, nLen, TIMEOUT)
			!= E_XSOCKET_SUCCESS)
		{
			printf("[-] Server response error (%d)\n", GetLastError());
			CoreServerSocket.Close();
			return 0;
		}

		if (nLen > 0)
			memcpy(Corebuffer + receivedSize, Chunk, nLen);

		receivedSize += nLen;

		if (receivedSize >= 4 && expectedFileSize == 0)
		{
			expectedFileSize = *(unsigned int*)Chunk;
			//printf("[+] Expecting a core of size: %d bytes.\n", expectedFileSize);
		}
		else if (receivedSize >= 4 && expectedFileSize != 0 && nLen > 0)
		{
			//printf("[+] Received a total of %d/%d bytes.\n", receivedSize, expectedFileSize);
		}
		else if (nLen < 0)
		{
			//printf("[-] Unexpected end to the stream.\n");
			break;
		}
		else {
			//printf("[-] Should not be here bro received size(%d)  expected size(%d).\n", receivedSize, expectedFileSize);
		}

		if (expectedFileSize == receivedSize)
		{
			//printf("[+] Core fully streamed.\n");
			break;
		}

	} while ((receivedSize < 4 && expectedFileSize == 0) || receivedSize < expectedFileSize);

	CoreServerSocket.Close();
	delete[] Chunk;


	if (expectedFileSize != receivedSize)
		return false;

	Size = receivedSize - 4;

	return (char*)(Corebuffer + 4);
}


int AIOAuth(std::string Username, std::string Password, std::string HWID)
{
	// Initialize Sodium crypto
	if (sodium_init() < 0) {
		printf("[-] Failed to initialize crypto.\n");
		return 0;
	}

	// Initalize winsocks
	if (!ClientSocket.Init())
	{
		printf("[-] Failed to initialize winsock.\n");
		return 0;
	}

	// Connect to server
	if (!ClientSocket.Connect("3.12.235.171", 42069))
	{
		int nError = ClientSocket.GetLastError();
		printf("[-] Failed to connect to server with error %d (%p).\n", nError, nError);
		return 0;
	}

	if (!PerformKeyExchange())
	{
		printf("[-] Failed to complete key exchange.\n");
		ClientSocket.Close();
		return 0;
	}

	if (!SendAuthentication(Username, Password, HWID))
	{
		printf("[-] Failed to send authentication token to server.\n");
		ClientSocket.Close();
		return 0;
	}

	return ReceiveAuthentication();
}
