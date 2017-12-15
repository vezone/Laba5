#pragma once

#include <windows.h>

typedef unsigned (__stdcall *PTHREAD_START) (void *);
#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadId) \
	((HANDLE)_beginthreadex( \
		(void *) (psa), \
		(unsigned) (cbStack), \
		(PTHREAD_START) (pfnStartAddr), \
		(void *) (pvParam), \
		(unsigned) (fdwCreate), \
		(unsigned *) (pdwThreadId)))

class CSWMRG {
	
public:
	CSWMRG(); 
	~CSWMRG(); 
	
	VOID WaitToRead(); 		
	VOID WaitToWrite();		
	VOID Done(); 			
	
private:
	CRITICAL_SECTION m_cs;  
	HANDLE m_hsemReaders; 	
	HANDLE m_hsemWriters; 
	int m_nWaitingReaders; 
	int m_nWaitingWriters; 
	int m_nActive; 			
};
