#ifndef LcEvent_h
#define LcEvent_h 1

//---------------------
// class name :LcTreeHandler
// Description: Utility class to hold and manipulate root histograms and files

#include <vector>

#include "LcVars.hh"

typedef struct{ 	
                        int eventID;

                        double originX;
                        double originY;
                        double originZ;

                        int primaryPhot;
                        int primaryCompt;

			G4int npp;
			G4double wpp[60000];

			G4int npd;
			G4double wpd[60000];
			G4double lpd[60000];
#ifdef TRACES
                        std::vector<double> distanceToOrigin;
                        std::vector<int> creatorProcessID;
                        std::vector<int> creatorOfCreatorProcessID;
                        
                        std::vector<int> reflections;

                        std::vector<std::vector<double>> rx;
                        std::vector<std::vector<double>> ry;
                        std::vector<std::vector<double>> rz;

                        std::vector<std::vector<double>> processID;

                        std::vector<int> parentGammaTrackID;
                        
                        std::vector<std::vector<double>> gx;
                        std::vector<std::vector<double>> gy;
                        std::vector<std::vector<double>> gz;

                        std::vector<std::vector<double>> parentGammaProcessID;
#endif /*TRACES*/
	      		}  LcEventStruct_t;
#endif /*LcEvent_h*/
