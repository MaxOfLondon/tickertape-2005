#ifndef MONITOR_H
#define MONITOR_H

#include <windows.h>
#include <stdio.h>

class Monitor {
  public:
  Monitor(){};
  ~Monitor();
  
  VOID setCallbackFunction( VOID (*_pCallback)(VOID));
  VOID start(char*); // starts monitoring for file writes in directory
  VOID stop();
  
  private:
  HANDLE m_hThread; 
  DWORD m_iID; //thread id
  char* m_szDir; //pathname to be monitored ( default ".\\" )
  VOID __stdcall (*m_pCallback)(VOID); // custom callback function pointer  __attribute__((stdcall))
  
  VOID signal(); // calls custom callback
  
  BOOL m_bMonitoring; // is monitoring thread running? used to kill the thread
  
  /* wrapper to enable using
  		member method as callback */
  static DWORD WINAPI Monitor::threadAdapter(PVOID _this) {
  	return ((Monitor*) _this)->threadProc();
 	}

  DWORD WINAPI threadProc();  /* thread procedure */
  
  		
  		
  		
};

#endif
