#pragma once
#include <Windows.h>

class CXEvent
{
   public :
      CXEvent();
      CXEvent(LPSECURITY_ATTRIBUTES lpSecurityAttributes,
         bool bManualReset,
         bool bInitialState);
   
      CXEvent(LPSECURITY_ATTRIBUTES lpSecurityAttributes,
         bool bManualReset,
         bool bInitialState,
         const TCHAR* pName);
      
      virtual ~CXEvent();

      HANDLE GetEvent() const;
      bool Wait() const;
      bool Wait(DWORD dwTimeoutMillis) const;
      void Reset();
      void Set();
      void Pulse();


   private :

      HANDLE m_hEvent;

};
