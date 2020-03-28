////////////////////////////////////////////////////////////////////////////////
// H. Seldon 30.11.10
// http://veridium.net
// hseldon@veridium.net
//
// This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <Ws2tcpip.h>
#include "XSocket.h"


CXSocket::CXSocket(void)
{
	m_hSocket = INVALID_SOCKET;
}

CXSocket::~CXSocket(void)
{
	Close();
}

int CXSocket::GetLastError()
{
	return m_nLastError;
}

bool CXSocket::Init()
{
	WORD    wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 2, 2 );


	int nErr = 0;
	m_nLastError = 0;

	if ((nErr = WSAStartup(wVersionRequested, &wsaData)) != 0)
	{
		m_nLastError = WSAGetLastError();
		return false;
	}


	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) 
	{
		WSACleanup();
		return false;
	}


	return true;
}


void CXSocket::Uninit()
{
	WSACleanup();
}


void CXSocket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}


void CXSocket::Abort()
{
	m_eventStop.Set();
}


bool CXSocket::Connect(const char *pAddr, short uPort)
{
	//Create

	m_nLastError = 0;

	if ((m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		m_nLastError = WSAGetLastError();
		return false;
	}
	

	/////////////////////////////////////////////////////////////////////////////
	
	sockaddr_in sa;

	addrinfo aiHints;
	addrinfo *aiList = NULL;

	memset(&aiHints, 0, sizeof(aiHints));

	aiHints.ai_family	= AF_INET;
	aiHints.ai_socktype	= SOCK_STREAM;
	aiHints.ai_protocol	= IPPROTO_TCP;

	if (getaddrinfo(pAddr, NULL, &aiHints, &aiList) != 0)
	{
		m_nLastError = WSAGetLastError();

		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		return false;
	}

	sa = *((sockaddr_in*)aiList->ai_addr);
	sa.sin_port = htons(uPort);


	/////////////////////////////////////////////////////////////////////////////

	if (connect(m_hSocket, (SOCKADDR*) &sa, sizeof(sa)) == SOCKET_ERROR)
	{
		m_nLastError = WSAGetLastError();

		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		return false;
	}


	/////////////////////////////////////////////////////////////////////////////

	if (WSAEventSelect(m_hSocket, m_eventNet.GetEvent(), FD_READ | FD_WRITE | FD_CLOSE) != 0)
	{
		m_nLastError = WSAGetLastError();
		return false;
	}


	return true;
}


int CXSocket::Send(const char* pBuff, int nLen, int& nLenSent, DWORD dwTimeOut)
{	

_BEGIN:

	int nRet = 0;
	m_nLastError = 0;

	if ((nRet = send(m_hSocket, pBuff, nLen, 0)) > 0 || (m_nLastError = WSAGetLastError()) != WSAEWOULDBLOCK)
		return E_XSOCKET_SUCCESS;


	///////////////////////////////////////////////////////////////////////////////////

	HANDLE arrHandles[2];

	arrHandles[0] = m_eventNet.GetEvent();
	arrHandles[1] = m_eventStop.GetEvent();

	DWORD dwWaitRes = WaitForMultipleObjects(2, arrHandles, FALSE, dwTimeOut); 
	
	if (dwWaitRes == WAIT_OBJECT_0 + 1)
		return E_XSOCKET_ABORTED;
	else if (dwWaitRes != WAIT_OBJECT_0)
		return E_XSOCKET_TIMEDOUT;


	//////////////////////////////////////////////////////////////////////////////////

	WSANETWORKEVENTS myNetEvents;

	if (WSAEnumNetworkEvents(m_hSocket, m_eventNet.GetEvent(), &myNetEvents) != 0)
	{
		m_nLastError = WSAGetLastError();
		return E_XSOCKET_SOCKERR;
	}

	if ((myNetEvents.lNetworkEvents & FD_WRITE) != FD_WRITE)
	{
		goto _BEGIN;
	}

	if (myNetEvents.iErrorCode[FD_WRITE_BIT] != 0)
		return E_XSOCKET_SOCKERR;


	/////////////////////////////////////////////////////////////////////////////////////

	nLenSent = send(m_hSocket, pBuff, nLen, 0);


	return E_XSOCKET_SUCCESS;
}


int CXSocket::Recv(char* pBuff, int nLen, int& nLenReceived, DWORD dwTimeOut)
{

_BEGIN:

	///////////////////////////////////////////////////////////////////////////////////

	HANDLE arrHandles[2];

	arrHandles[0] = m_eventNet.GetEvent();
	arrHandles[1] = m_eventStop.GetEvent();

	DWORD dwWaitRes = WaitForMultipleObjects(2, arrHandles, FALSE, dwTimeOut); 
	
	if (dwWaitRes == WAIT_OBJECT_0 + 1)
		return E_XSOCKET_ABORTED;
	else if (dwWaitRes != WAIT_OBJECT_0)
		return E_XSOCKET_TIMEDOUT;


	////////////////////////////////////////////////////////////////////////////////////

	WSANETWORKEVENTS myNetEvents;

	if (WSAEnumNetworkEvents(m_hSocket, m_eventNet.GetEvent(), &myNetEvents) != 0)
	{
		m_nLastError = WSAGetLastError();
		return E_XSOCKET_SOCKERR;
	}

	if ((myNetEvents.lNetworkEvents & FD_READ) != FD_READ)
		goto _BEGIN;

	if (myNetEvents.iErrorCode[FD_READ_BIT] != 0)
		return E_XSOCKET_SOCKERR;


	/////////////////////////////////////////////////////////////////////////////////////

	if ((nLenReceived = recv(m_hSocket, pBuff, nLen, 0)) == WSAEWOULDBLOCK)
	{
		nLenReceived = 0;
		return E_XSOCKET_NOMOREDATA;
	}


	return E_XSOCKET_SUCCESS;
}

long CXSocket::GetLenDataAvail()
{
	u_long lLen = 0;
	if (ioctlsocket(m_hSocket, FIONREAD, &lLen) != 0)
	{
		m_nLastError = WSAGetLastError();
		return E_XSOCKET_SOCKERR;
	}


	return lLen;
}