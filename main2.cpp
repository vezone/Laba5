// Single-Writer/Multiple-Reader Guard
#include <iostream>
#include <windows.h>
#include <time.h>
#include <omp.h>
#include <tchar.h>
#include <process.h> 
#include "src\SWMRG.h"

#define log(x) std::cout << x << std::endl

CSWMRG g_swmrg;

long Thread(PVOID pvParam) {
	srand(time(0));
	TCHAR sz[50];
	wsprintf(sz, TEXT("SWMRG Test: Thread %d"), PtrToShort(pvParam)); 
	int n = MessageBox(NULL, TEXT("YES: Attempt to read\nNO: Attempt to write"), sz, MB_YESNO);
	 
	if (n == IDYES) {
		g_swmrg.WaitToRead();
		MessageBox(NULL, TEXT("Reader"), sz, MB_OK);
	}
	else {
		g_swmrg.WaitToWrite();
		MessageBox(NULL, TEXT("Writer"), sz, MB_OK);
	}

	MessageBox(NULL, (n == IDYES) ? TEXT("OK stops READING") : TEXT("OK stops WRITING"),sz, MB_OK);
	g_swmrg.Done();
	
	return(0);
}


int main(void) {
	
	int nThreads = 0;
	HANDLE hThreads[MAXIMUM_WAIT_OBJECTS];
	
	omp_set_dynamic(0); 	//Запрещяем системе динамически менять количество нитей
	
	#pragma omp parallel sections num_threads(2) 
	{
		#pragma omp section
		{
			for (nThreads; nThreads < 5; nThreads++) {
				DWORD dwThreadId;
				hThreads[nThreads] =
					chBEGINTHREADEX(NULL, 0, Thread, (PVOID) (DWORD_PTR) nThreads, 0, &dwThreadId);
			}
			
			WaitForMultipleObjects(nThreads, hThreads, TRUE, INFINITE);
	
			while (nThreads--)
				CloseHandle(hThreads[nThreads]);
		}
		
		#pragma omp section
		{
			int k = 5;
			for (k; k < 7; k++) {
				DWORD dwThreadId;
				hThreads[k] =
					chBEGINTHREADEX(NULL, 0, Thread, (PVOID) (DWORD_PTR) k, 0, &dwThreadId);
			}
			
			WaitForMultipleObjects(k, hThreads, TRUE, INFINITE);
	
			while (k >= 5){
				CloseHandle(hThreads[k]);
				k--;
			}
		}
 	}
	
	
	return(0);
}
