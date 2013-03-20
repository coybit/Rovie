#include <stdio.h>
#include <iostream>
#include <windows.h>

using namespace std;

#include "sigar-bin/include/sigar.h"

extern "C" {
#include "sigar-bin/include/sigar_format.h"

}

#define SAMPLING_PERIOD 2000

int main(int argc, char **argv) {
    int status, i;
    sigar_t *sigar;
    sigar_cpu_list_t cpulist1, cpulist2;
	sigar_cpu_perc_t perc;
	bool isFirstTime = true;
	FILE* idleDB;
	unsigned long long sampleCount = 0;

	// Initialization
    sigar_open(&sigar);
	idleDB = fopen( "C:\\idle.db", "wb" );

	// Write number of CPUs in the file
	status = sigar_cpu_list_get(sigar, &cpulist1);

	if (status != SIGAR_OK) {
		printf("cpu_list error: %d (%s)\n",
				status, sigar_strerror(sigar, status));
		return 1;
	}

	fwrite( &cpulist1.number, sizeof(int), 1, idleDB );
	fclose(idleDB);

	while( true ) 
	{
		cout << "\rSampling ... " << (++sampleCount);

		sigar_cpu_list_get(sigar, &cpulist2);
		
		for (i=0; i<cpulist2.number; i++) {
			sigar_cpu_perc_calculate(&cpulist1.data[i], &cpulist2.data[i], &perc);

			BYTE idlePerc = 100*perc.idle;

			idleDB = fopen( "C:\\idle.db", "ab+" );
			fwrite( &idlePerc, sizeof(BYTE), 1, idleDB );
			fclose(idleDB );
		}

		Sleep(SAMPLING_PERIOD);
		
		sigar_cpu_list_destroy(sigar, &cpulist1);
		cpulist1 = cpulist2;
	}

	// Release resources
	sigar_cpu_list_destroy(sigar, &cpulist2);
    sigar_close(sigar);

    return 0;
}