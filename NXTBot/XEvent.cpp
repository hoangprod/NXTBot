#include "pch.h"
#include ".\xevent.h"

#include <assert.h>

CXEvent::CXEvent()
: m_hEvent(CreateEvent(NULL, TRUE, FALSE, 0))
{
}

CXEvent::CXEvent(
   LPSECURITY_ATTRIBUTES lpEventAttributes, 
   bool bManualReset, 
   bool bInitialState)
   :  m_hEvent(CreateEvent(lpEventAttributes, bManualReset, bInitialState, 0))
{
}

CXEvent::CXEvent(
   LPSECURITY_ATTRIBUTES lpEventAttributes, 
   bool bManualReset, 
   bool bInitialState, 
   const TCHAR* pName)
   :  m_hEvent(CreateEvent(lpEventAttributes, bManualReset, bInitialState, pName))
{

}

CXEvent::~CXEvent()
{
   ::CloseHandle(m_hEvent);
}

HANDLE CXEvent::GetEvent() const
{
   return m_hEvent;
}

bool CXEvent::Wait() const
{
   if (!Wait(INFINITE))
   {
	   //CXLog::Write("CXEvent::Wait(), Unexpected timeout on infinite wait", GetLastError());
     return false;
   }

 return true;
}

bool CXEvent::Wait(DWORD timeoutMillis) const
{
   bool bRet = true;

   DWORD dwResult = ::WaitForSingleObject(m_hEvent, timeoutMillis);

   if (dwResult == WAIT_TIMEOUT)
      bRet = false;
   else if (dwResult == WAIT_OBJECT_0)
      bRet = true;
   //else
   //   CXLog::Write("CXEvent::Wait() - WaitForSingleObject failed", ::GetLastError());

    
   return bRet;
}

void CXEvent::Reset()
{
   if (!::ResetEvent(m_hEvent))
   {
	   //***
      //CXLog::Write("CXEvent::Reset() failed", ::GetLastError());
   }
}

void CXEvent::Set()
{
   if (!::SetEvent(m_hEvent))
   {
	   assert(NULL);
      //CXLog::Write("CXEvent::Set() failed", ::GetLastError());
   }
}

void CXEvent::Pulse()
{
   if (!::PulseEvent(m_hEvent))
   {
      //CXLog::Write("CXEvent::Pulse() failed", ::GetLastError());
   }
}