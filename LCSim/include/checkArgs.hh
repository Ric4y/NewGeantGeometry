#ifndef checkArgs_h
 #define checkArgs_h 1
 #include <unistd.h>
 #include <getopt.h>
 #include <stdlib.h>
 #include <vector>
 #include <string.h>
 
 #include "G4Types.hh"
 #include "readLambda.hh"

int printUsage();
int checkArgs(G4int* argc, char* argv[],
        G4int* typeCr, G4int* tapered, G4int* matFrac, G4int* matType, G4double* lambda,
        G4int* detType, G4int* posFrac, G4int* nEvents,
        G4int* fracTop, G4int* fracRight, G4int* fracBottom, G4int* fracLeft,
        G4bool* model,
        G4double* rindexOpticalGlue, G4double* rindexEpoxyAPD,
        vector<double>* vEnergy, vector<double>* vLambda);
#endif /*checkArgs*/
