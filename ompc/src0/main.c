#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "struct.h"
#include "pcg.h"
#include "input.h"
#include "pointer_init.h"
#include "boundary_cell.h"
#include "cell_metrics.h"
#include "poi_gen.h"
#include "solver_ICCG_mc.h"
#include "outucd.h"

int
main()
{
	double *WK;
	int NPL, NPU;
	int ISET, ITR, IER;
	int icel, ic0, i;
	double xN, xL, xU;
	double Stime, Etime;

/*********
 * INIT. *
 *********/
	if(INPUT()) goto error;
	if(POINTER_INIT()) goto error;
	if(BOUNDARY_CELL()) goto error;
	if(CELL_METRICS()) goto error;
	if(POI_GEN()) goto error;


/***************
 * MAIN SOLVER *
 ***************/
	memset(PHI, 0.0, sizeof(double)*ICELTOT);

	ISET = 0;

	WK = (double *)malloc(sizeof(double)*ICELTOT);
	if(WK == NULL) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
		goto error;
	}


	Stime = omp_get_wtime();
	if(solve_ICCG_mc(ICELTOT, NL, NU, indexL, itemL, indexU, itemU,
		D, BFORCE, PHI, AL, AU, NCOLORtot, PEsmpTOT,
		SMPindex, SMPindexG, EPSICCG, &ITR, &IER)) goto error;
	Etime = omp_get_wtime();

	fprintf(stdout, "\nN= %10d\n", ICELTOT);
	fprintf(stdout, "%16.6e sec. (solver)\n", Etime - Stime);

	for(ic0=0; ic0<ICELTOT; ic0++) {
		icel = NEWtoOLD[ic0];
		WK[icel-1] = PHI[ic0];
	}

	for(icel=0; icel<ICELTOT; icel++) {
		PHI[icel] = WK[icel];
	}

        fprintf(stdout, "%10d%16.6e\n", ICELTOT, PHI[ICELTOT-1]);
/*
        if(OUTUCD()) goto error;
*/
	return 0;

error:
	return -1;
}
