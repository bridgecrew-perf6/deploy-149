#include <stdio.h>
#include <string.h>
#include <x86intrin.h>

#include "../include/mnblas.h"
#include "../include/complexe.h"

#include "flop.h"
#include "papi.h"


#define VECSIZE 50000

#define NB_FOIS 41943
#define MAX_EVENTS 128
#define MAX_RAPL_EVENTS 64


typedef float vfloat[VECSIZE];
typedef complexe_float_t vComplexefloat[VECSIZE];

vfloat vec1, vec2;
vComplexefloat vecComplexef1, vecComplexef2;

char events[MAX_EVENTS][BUFSIZ];
char filenames[MAX_EVENTS][BUFSIZ];
FILE *fff[MAX_EVENTS];
static int num_events=0;

void vector_init(vfloat V, float x)
{
	register unsigned int i;

	for (i = 0; i < VECSIZE; i++){
		V[i] = x;
	}
	return;
}

void vector_init_complexe(vComplexefloat V, float reel, float imaginaire)
{
	register unsigned int i;

	for (i = 0; i < VECSIZE; i++){
	    V[i].real = reel;
	    V[i].imaginary = imaginaire;
  	}

  return;
}

void vector_print(vfloat V)
{
	register unsigned int i;

	for (i = 0; i < VECSIZE; i++){
		printf("%f ", V[i]);
	}
	printf("\n");

	return;
}

int main(int argc, char **argv)
{
	unsigned long long start, end;
  	float res;
  	int i;

  	double somme_double = 0.f;
  	double somme_double_cp = 0.f;

  	//init PAPI
	int retval,cid,rapl_cid=-1,numcmp;
    int EventSet = PAPI_NULL;
    long long values[MAX_EVENTS];
    int code,enum_retval;
    const PAPI_component_info_t *cmpinfo = NULL;
	PAPI_event_info_t info;
    long long start_time,before_time,after_time;
    double elapsed_time,total_time;
    char event_names[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
	char units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
  	// Initialize the PAPI library
  	retval = PAPI_library_init(PAPI_VER_CURRENT);
  	if (retval != PAPI_VER_CURRENT && retval > 0) {
		fprintf(stderr,"PAPI library version mismatch!\n");
		exit(1);
	}
	if (retval < 0) {
		fprintf(stderr, "Initialization error!\n");
		exit(1);
	}

	//return the number of components currently installed
	numcmp = PAPI_num_components();

	//for each compononent, check if it's rapl and get the CID
	for(cid=0; cid<numcmp; cid++) {

		if ( (cmpinfo = PAPI_get_component_info(cid)) == NULL) {
			fprintf(stderr,"PAPI_get_component_info failed\n");
			exit(1);
		}

		if (strstr(cmpinfo->name,"rapl")) {
			rapl_cid=cid;
			printf("Found rapl component at cid %d\n", rapl_cid);

			if (cmpinfo->disabled) {
				fprintf(stderr,"No rapl events found: %s\n", cmpinfo->disabled_reason);
				//don't interrupt the program to let computing 
    			//exit(1);
			}
			break;
		}
    }
     
	//if the component was not found beyond all
    if (cid==numcmp) {
		fprintf(stderr,"No rapl component found\n");
		//don't interrupt the program to let computing 
    	//exit(1);
    }
     
	//Event Sets are user-defined groups of hardware events (preset or native)	
	//create the eventSet
	retval = PAPI_create_eventset( &EventSet );
    if (retval != PAPI_OK) {
    	fprintf(stderr,"Error creating eventset!\n");
    }

	//Given an event code, PAPI_enum_event replaces the event code with the next available event.
	//The modifier argument affects which events are returned. 
	//For all platforms and event types, a value of PAPI_ENUM_ALL (zero) 
	//directs the function to return all possible events.
	//search in the component cid events available
    code = PAPI_NATIVE_MASK;

    enum_retval = PAPI_enum_cmp_event( &code, PAPI_ENUM_FIRST, cid );
    
	while ( enum_retval == PAPI_OK ) {
		//get the name in event_name with the code
    	retval = PAPI_event_code_to_name( code, event_names[num_events] );
    	//in case of error
		if ( retval != PAPI_OK ) {
	  		printf("Error translating %#x\n",code);
	  		exit(1);
		}
		retval = PAPI_get_event_info(code,&info);
		if (retval != PAPI_OK) {
	  		printf("Error while trying to get info about the code\n");
		
		}
	
		strncpy(units[num_events],info.units,sizeof(units[0]));
		//buffer must be null terminated to safely use strstr operation on it below
		units[num_events][sizeof(units[0])-1] = '\0';

		//add event
		retval = PAPI_add_event(EventSet, code);
		if ( retval != PAPI_OK ) {
	  		printf("Error can't add the event\n");
	  		break;
		}
		num_events++;
		//get the next event
    	enum_retval = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, cid );
    }

  	//if no RAPL were found:
	if(num_events == 0){
		printf("Error, no RAPL event were found\n");
		//don't interrupt the program to let computing 
    	//exit(1);
	}
	 

	init_flop();
	//opening the file to write data
	FILE* fptRAPL ;
	fptRAPL = fopen( "../out/rapl.csv", "w+");
	if (fptRAPL==NULL){
        printf("\nCan't open the file\n");
        exit(1);
    }
	//get the time of starting:
	start_time=PAPI_get_real_nsec();
	before_time=PAPI_get_real_nsec();
    retval = PAPI_start( EventSet);
    if (retval != PAPI_OK) {
		fprintf(stderr,"PAPI_start() failed\n");
		//don't interrupt the program to let computing 
    	//exit(1);
    }
	double oldPackageNrj = 0;
	double oldDramNrj = 0;
	double oldPp0Nrj = 0;
  	for (i = 0; i < NB_FOIS; i++){
    	vector_init(vec1, 1.0);
    	vector_init(vec2, 2.0);
    	res = 0.0;

   
    
    	start = _rdtsc();
    	res = mncblas_sdot(VECSIZE, vec1, 1, vec2, 1);
    	end = _rdtsc();
		//TODO: Read and reset the counter
		if(PAPI_read(EventSet, values) != PAPI_OK){
			printf("Error while reading values\n");
			//don't interrupt the program to let computing 
    		//exit(1);
		}

	
		//write in the CSV:
		long long tmpTime = PAPI_get_real_nsec();
		double packageNrj = 0;
		double dramNrj = 0;
		double pp0Nrj = 0;
		for(int i = 0; i < num_events; i++){
			if (strstr(units[i],"nJ")) {
				if(strstr(event_names[i], "PACKAGE_ENERGY:PACKAGE")){
					printf("Package energy found\n");
					packageNrj += (double)values[i]/1.0e9;
				}else if(strstr(event_names[i], "DRAM_ENERGY:PACKAGE")){
					printf("DRAM energy found\n");
					dramNrj += (double)values[i]/1.0e9;
				}else if (strstr(event_names[i], "PACKAGE_ENERGY:PACKAGE")){
					printf("PP0 energy found\n");
					pp0Nrj += (double)values[i]/1.0e9;
				}
				
			}
		}
		
		//timestamps
		fprintf(fptRAPL,"%f,", tmpTime/1.0e9);
		//Package_Energy:Package0+1
		fprintf(fptRAPL,"%f,", packageNrj-oldPackageNrj);
		//DRAM_Energy:Package0+1
		fprintf(fptRAPL,"%f,", dramNrj-oldDramNrj);
		//PP0_Energy:Package0+1
		fprintf(fptRAPL,"%f\n", pp0Nrj-oldPp0Nrj);

		oldPackageNrj = packageNrj;
		oldDramNrj = dramNrj;
		oldPp0Nrj = pp0Nrj;

		//printf("mncblas_sdot %d : res = %3.2f nombre de cycles: %Ld \n", i, res, end - start);
    	somme_double += calcul_flop_ret("sdot ", 2 * VECSIZE, end - start);
  	}

	printf("\n");
  	init_flop();
  	complexe_float_t resComplexe ;
  	for (i = 0; i < NB_FOIS; i++){
    	vector_init_complexe(vecComplexef1, 1.0, 0.0);
    	vector_init_complexe(vecComplexef2, 2.0, 0.0);
    	resComplexe.real = 0.0;
    	resComplexe.imaginary = 0.0;

	    start = _rdtsc();
	    mncblas_cdotu_sub(VECSIZE, vecComplexef1, 1, vecComplexef2, 1, &resComplexe);
	    end = _rdtsc();
		
		//TODO: Read and reset the counter
		if(PAPI_read(EventSet, values) != PAPI_OK){
			printf("Error while reading values\n");
			//don't interrupt the program to let computing 
    		//exit(1);
		}

	
		//write in the CSV:
		long long tmpTime = PAPI_get_real_nsec();
		double packageNrj = 0;
		double dramNrj = 0;
		double pp0Nrj = 0;
		for(int i = 0; i < num_events; i++){
			if (strstr(units[i],"nJ")) {
				if(strstr(event_names[i], "PACKAGE_ENERGY:PACKAGE")){
					printf("Package energy found\n");
					packageNrj += (double)values[i]/1.0e9;
				}else if(strstr(event_names[i], "DRAM_ENERGY:PACKAGE")){
					printf("DRAM energy found\n");
					dramNrj += (double)values[i]/1.0e9;
				}else if (strstr(event_names[i], "PACKAGE_ENERGY:PACKAGE")){
					printf("PP0 energy found\n");
					pp0Nrj += (double)values[i]/1.0e9;
				}
				
			}
		}
		//timestamps
		fprintf(fptRAPL,"%f,", tmpTime/1.0e9);
		//Package_Energy:Package0+1
		fprintf(fptRAPL,"%f,", packageNrj-oldPackageNrj);
		//DRAM_Energy:Package0+1
		fprintf(fptRAPL,"%f,", dramNrj-oldDramNrj);
		//PP0_Energy:Package0+1
		fprintf(fptRAPL,"%f\n", pp0Nrj-oldPp0Nrj);

		oldPackageNrj = packageNrj;
		oldDramNrj = dramNrj;
		oldPp0Nrj = pp0Nrj;


    	//printf("mncblas_cdotu_sub %d : res = %3.2f +i %3.2f nombre de cycles: %Ld \n", i, resComplexe.real, resComplexe.imaginary, end - start);

    	somme_double_cp += calcul_flop_ret("sdot ", 8 * VECSIZE, end - start);
  	}
	fclose(fptRAPL);
	printf("\n");
	printf("=======================================================\n");

	after_time=PAPI_get_real_nsec();
    retval = PAPI_stop( EventSet, values);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI_stop() failed\n");
    }
	total_time=((double)(after_time-start_time))/1.0e9;
    elapsed_time=((double)(after_time-before_time))/1.0e9;
	for(i=0;i<num_events;i++) {
		printf("%.4f %.1f (* Average Power for %s *)\n",total_time,((double)values[i]/1.0e9)/elapsed_time,events[i]);
    }

	printf("\n");

    printf("\n=========== Résultats DOT double précision ===========\n");
    printf("En moyenne on a une performance de %5.3f GFlop/s\n", somme_double/NB_FOIS);
    printf("\n=========== Résultats DOT double précision complexe ===========\n");
    printf("En moyenne on a une performance de %5.3f GFlop/s\n", somme_double_cp/NB_FOIS);
    return 0;
}
