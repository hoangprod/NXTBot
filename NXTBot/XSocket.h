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

#pragma once

#include "XEvent.h"
#include <winsock2.h>


enum
{
	E_XSOCKET_SUCCESS,
	E_XSOCKET_TIMEDOUT,
	E_XSOCKET_ABORTED,
	E_XSOCKET_NOMOREDATA,
	E_XSOCKET_SOCKERR
};


class CXSocket
{
protected:

	SOCKET	m_hSocket;

	CXEvent	m_eventNet;
	CXEvent m_eventStop;

	int		m_nLastError;


public:

	CXSocket(void);
	~CXSocket(void);

	bool Init();
	void Uninit();


	////////////////////////////////////////////////////

	bool Connect(const char* pAddr, short uPort);

	int Recv(char* pBuff, int nLen, int& nLenReceived, DWORD dwTimeOut = 0);
	int Send(const char* pBuff, int nLen, int& nLenSent, DWORD dwTimeOut = 0);
	
	long GetLenDataAvail();
	int GetLastError();
	
	void Close();
	void Abort();
	bool IsStopped();
};
