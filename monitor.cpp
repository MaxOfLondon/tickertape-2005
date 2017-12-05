#include "monitor.h"

Monitor::~Monitor() {
  stop();
}

VOID Monitor::start(char* dir=".\\") {
  m_hThread = CreateThread(NULL, 0, threadAdapter, this ,0,&m_iID);
  m_szDir = dir;
  if (m_hThread == NULL) {
      printf( "CreateThread failed (%d)\n", GetLastError() );
      return;
  }
}

VOID Monitor::stop() {
	m_bMonitoring = FALSE;
	CloseHandle(m_hThread);
	m_hThread = 0;
}

DWORD WINAPI Monitor::threadProc() {
	HANDLE cnh;
  m_bMonitoring = TRUE;

  cnh = FindFirstChangeNotification(m_szDir, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
  while (m_bMonitoring) {
  DWORD wr = WaitForSingleObject(cnh, INFINITE);

  	// get rid of all similar events that occur shortly after this
    DWORD dwMilSecs = 200; // arbitrary; enlarge for floppies
    do {
          FindNextChangeNotification(cnh);
          wr = WaitForSingleObject(cnh, dwMilSecs);
    } while(wr == WAIT_OBJECT_0);
    signal();
  }

    FindCloseChangeNotification(cnh); // release notification handle
}

VOID Monitor::signal() {
	if (NULL != m_pCallback) {
  	(*m_pCallback)(); // call custom callback function
  } else {
    printf("No callback function set\n");
  }
}

VOID Monitor::setCallbackFunction( VOID (*_pCallback)(VOID)) {
        m_pCallback = _pCallback;
}
