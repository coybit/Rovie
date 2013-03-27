#include <stdio.h>
#include <iostream>
#include <windows.h>
#include "sigar-bin/include/sigar.h"
extern "C" {
#include "sigar-bin/include/sigar_format.h"
}
#include "Log.h"
using namespace std;

#define SAMPLING_PERIOD 1000

SQLite::Database* historyDB;
HHOOK hook;

enum EventType { MOUSE, CPU, MEMORY, CTRLSIGNAL, KEYBOARD };

#ifdef _WIN32
    #include <windows.h>

    void sleep(unsigned milliseconds)
    {
        Sleep(milliseconds);
    }

	BOOL getCursorPos( __out LPPOINT lpPoint)
	{
		return GetCursorPos(lpPoint);
	}

	LRESULT CALLBACK keyboardIsUsed( int code, WPARAM wParam, LPARAM lParam )
	{
		cout << "A Key pressed!" << endl;
		return CallNextHookEx(hook,code,wParam,lParam);
	}

	LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(int nCode,WPARAM wParam, LPARAM lParam)
	{
		char ch;            
		if (((DWORD)lParam & 0x40000000) &&(HC_ACTION==nCode))
		{        
			if ((wParam==VK_SPACE)||(wParam==VK_RETURN)||(wParam>=0x2f ) &&(wParam<=0x100)) 
			{
				if (wParam==VK_RETURN)
				{
					ch='\n';
				}
				else
				{
					   BYTE ks[256];
					GetKeyboardState(ks);

					WORD w;
					UINT scan=0;
					ToAscii(wParam,scan,ks,&w,0);
					ch = char(w); 
				}
			}
		}

		LRESULT RetVal = CallNextHookEx( hook, nCode, wParam, lParam );
		return  RetVal;
	}

	BOOL CtrlHandler( DWORD fdwCtrlType ) 
	{ 
		Log( historyDB, EventType::CTRLSIGNAL, 1, fdwCtrlType );

		switch( fdwCtrlType ) 
		{ 
			// Handle the CTRL-C signal. 
			case CTRL_C_EVENT: 
				printf( "Ctrl-C event\n\n" );
				Beep( 750, 300 ); 
				return( FALSE );
 
			// CTRL-CLOSE: confirm that the user wants to exit. 
			case CTRL_CLOSE_EVENT: 
				Beep( 600, 200 ); 
				printf( "Ctrl-Close event\n\n" );
				return( FALSE ); 
 
			// Pass other signals to the next handler. 
			case CTRL_BREAK_EVENT: 
				Beep( 900, 200 ); 
				printf( "Ctrl-Break event\n\n" );
				return FALSE; 
 
		case CTRL_LOGOFF_EVENT: 
			Beep( 1000, 200 ); 
			printf( "Ctrl-Logoff event\n\n" );
			return FALSE; 
 
		case CTRL_SHUTDOWN_EVENT: 
			Beep( 750, 500 ); 
			printf( "Ctrl-Shutdown event\n\n" );
			return FALSE; 
 
		default: 
			return FALSE; 
		} 
	}

	void Hook()
	{
		//HMODULE hInstance = GetModuleHandle(NULL);
		//hook = SetWindowsHookEx( WH_KEYBOARD_LL, (HOOKPROC)keyboardIsUsed, NULL, NULL );
		//DWORD err = GetLastError();

		if( SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE ) == false ) 
			printf( "\nERROR: Could not set control handler"); 
	}
	
	void UnHook()
	{
		//UnhookWindowsHookEx(hook);
		SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, FALSE );
	}

#else
    #include <unistd.h>

    void sleep(unsigned milliseconds)
    {
        usleep(milliseconds * 1000); // takes microseconds
    }

	BOOL getCursorPos( __out LPPOINT lpPoint)
	{
		throw "Implement It!";
	}

	void Hook()
	{
		throw "Implement It!";
	}

	void UnHook()
	{
		throw "Implement It!";
	}
#endif 

int PressedKeyCounter()
{
	int counter = 0;
	FILE* fp=fopen("c:\\report.txt","rb");
	if( feof(fp) )
		fread( &counter, sizeof(int), 1, fp );
	fclose(fp);

	return counter;
}

int main(int argc, char **argv) 
{
    int status, i;
    sigar_t *sigar;
    sigar_cpu_list_t cpulist1, cpulist2;
	sigar_cpu_perc_t perc;
	bool isFirstTime = true;
	FILE* idleDB;
	unsigned long long sampleCount = 0;
	POINT mousePos1, mousePos2;
	bool mouseIsMoved = false;


	// Initialization
    sigar_open(&sigar);
	historyDB = initLog();
	Hook();

	// Write number of CPUs in the file
	status = sigar_cpu_list_get(sigar, &cpulist1);

	if (status != SIGAR_OK) {
		printf("cpu_list error: %d (%s)\n",
				status, sigar_strerror(sigar, status));
		return 1;
	}

	FILE* sysinfo = fopen( "C:\\sysinfo.db", "wb" );
	fwrite( &cpulist1.number, sizeof(int), 1, sysinfo );
	fclose(sysinfo);

	getCursorPos(&mousePos1);

	while( true ) 
	{
        sleep(SAMPLING_PERIOD);
		cout << "\rSampling ... " << (++sampleCount);

		// Mouse
		getCursorPos(&mousePos2);
		mouseIsMoved = ( mousePos1.x != mousePos2.x || mousePos1.y != mousePos2.y );
		if(mouseIsMoved)
			Log( historyDB, EventType::MOUSE, 1, 0 );

		// CPU usage
		sigar_cpu_list_get(sigar, &cpulist2);
		for (i=0; i<cpulist2.number; i++) {
			sigar_cpu_perc_calculate(&cpulist1.data[i], &cpulist2.data[i], &perc);

			BYTE idlePerc = 100*perc.idle;
			Log( historyDB, EventType::CPU, i, idlePerc );
			
		}

		// RAM usage
		sigar_mem_t mem;
		sigar_mem_get( sigar, &mem );
		BYTE freeMem = mem.free_percent;
		Log( historyDB, EventType::MEMORY, freeMem, 0 );

		sigar_cpu_list_destroy(sigar, &cpulist1);
		cpulist1 = cpulist2;
		mousePos1 = mousePos2;
	}

	// Release resources
	sigar_cpu_list_destroy(sigar, &cpulist2);
    sigar_close(sigar);
	releaseLog(historyDB);
	//UnHook();

    return 0;
}
