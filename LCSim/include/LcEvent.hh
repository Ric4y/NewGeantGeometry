#ifndef LcEvent_h
#define LcEvent_h 1

//---------------------
// class name :LcTreeHandler
// Description: Utility class to hold and manipulate root histograms and files

#include <vector>

#include "LcVars.hh"
const G4int hits_limit = 8000;
typedef struct{ 	
                        int eventID;

                        double originX;
                        double originY;
                        double originZ;

                        int primaryPhot;
                        int primaryCompt;
#ifdef NEW_GEOMETRY
                        int pmtID;
            G4double globalTime2[hits_limit];
            //G4double localTime[hits_limit];
            //G4double properTime[hits_limit];
            G4int npp2;
			G4double wpp2[hits_limit];

			G4int npd2;
			G4double wpd2[hits_limit];
			G4double lpd2[hits_limit];
            G4double globalTime[hits_limit];
#endif /*NEW_GEOMETRY*/
			G4int npp;
			G4double wpp[hits_limit];

			G4int npd;
			G4double wpd[hits_limit];
			G4double lpd[hits_limit];
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
