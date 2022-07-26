//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include <time.h>    // For timestamp in usec
#include <string.h>
#include <vector>

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "LcDetectorConstruction.hh"
#include "LcPhysicsList.hh"
#include "LcPrimaryGeneratorAction.hh"
#include "LcRunAction.hh"
#include "LcEventAction.hh"
#include "LcStackingAction.hh"
#include "LcSteppingAction.hh"
#include "LcSteppingVerbose.hh"
#include "LcUserTrace.hh"
#include "LcUserTraceCollection.hh"

#include "LcVars.hh"

#include "Randomize.hh"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "LcTreeHandler.hh"
//#include "TObject.h"
#include "checkArgs.hh"

//#ifdef G4VIS_USE
// #undef G4VIS_USE
//#endif /*G4VIS_USE*/
#ifdef G4VIS_USE
 #include "G4VisExecutive.hh"
#endif /*G4VIS_USE*/
#ifdef G4UI_USE
 #include "G4UIExecutive.hh"
#endif /*G4UI_USE*/

using std::vector;

//initialise root
TROOT troot("Lc","Lc");
LcTreeHandler* RootTreeHandler;

//LcUserTrace* userTrace;
LcUserTraceCollection* userTraceCollection;
//LcUserTrace* primTrace;
LcUserTraceCollection* primTraceCollection;
//LcUserTraceCollection* gammTraceCallection;

G4int main(G4int argc, char* argv[])
{
    //this code works with the following arguments
    //typeCr, geometry of the crystal, 1-17cm, 2-18cm, 3-22cm  
    //tapered, 1 by default for tapered, no other value in use
    //matFrac, if matType 10 not used, otherwise 1 to 4, 1 - 1/4 (starting from apd), 2 - 1/2, 3 - 3/4, 4/4 full
    //matType, 10 by default, 1 - 9 different lapping strategies
    //posFrac, position of particle gun, 10 equi points starting from apd
    //detType, 1 - full PMT, 3 - APD 2 1cm2 surfaces (Hama)
    //nEvents, number of events
    //if matType == 10 then the following arguments need to be provided
    //fracTop, polished part of detector starting from furthest point from APD, 
    //         0 - full lapped, 1 - 1/4 , 2 - 1/2, 3 - 3/4, 4 - fully polished  
    //
    //fracRight, polished part of detector starting from furthest point from APD, 
    //         0 - full lapped, 1 - 1/4 , 2 - 1/2, 3 - 3/4, 4 - fully polished  
    //
    //fracBottom, polished part of detector starting from furthest point from APD, 
    //         0 - full lapped, 1 - 1/4 , 2 - 1/2, 3 - 3/4, 4 - fully polished  
    //
    //fracLeft, polished part of detector starting from furthest point from APD, 
    //         0 - full lapped, 1 - 1/4 , 2 - 1/2, 3 - 3/4, 4 - fully polished  
    //
    //lambda, absorption length, used if IFNOREFLECTOR is on in Lcvars.hh
    //modelSwitch, 0 - unified model, 1 - LUT of real surface using DAVIS model with LU LUTs 
    //             unified model parameters set in LCdetectorconstruction.cc
    //rindexOpticalGlue, ref. index of coupler
    //rindexEpoxyAPD, ref. index of APD enclosure

    G4int  typeCr,   tapered,   matFrac,    matType,  posFrac, detType, 
           nEvents,  fracTop, fracRight, fracBottom, fracLeft;
    G4double lambda = 0.0;
    G4bool modelSwitch;
    G4double rindexOpticalGlue;
    G4double rindexEpoxyAPD;
    vector<double>* vEnergy = new vector<double>();
    vector<double>* vLambda = new vector<double>();

    typeCr = tapered = matFrac = matType = posFrac = detType = nEvents = 
        fracTop = fracRight = fracBottom = fracLeft = 0;

    char cname[200];
    strcpy(cname, "");

    G4UImanager* UImanager = G4UImanager::GetUIpointer(); 
#ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc, argv, "qt");
#endif /*G4UI_USE*/

    G4int argsResult = checkArgs(&argc, argv,
            &typeCr, &tapered, &matFrac, &matType, &lambda,
            &detType, &posFrac, &nEvents,
            &fracTop, &fracRight, &fracBottom, &fracLeft,
            &modelSwitch,
            &rindexOpticalGlue, &rindexEpoxyAPD,
            vEnergy, vLambda);
    switch (argsResult)
    {
        case -1:
            G4cout << "Arguments checked, result = " << argsResult << G4endl;
            G4cout << "Failed to parse arguments, check input" << G4endl;
            return -1;
            break;
        case -2:
            return -1;
            break;
        default:
            G4cout << "Arguments checked, result = " << argsResult << G4endl;
            break;
    }
    //if (argsResult < 0)
    //{
    //    G4cout << "Failed to parse arguments, check input" << G4endl;
    //    return -1;
    //}

    //// Check number of arguments
    //if (argc < 12)
    //{
    //    G4cout << "Incorrect number of input arguments, using default" << G4endl;

    //    // set default values
    //    typeCr  = 1;
    //    tapered = 1; // 0 for rectangular prism geometry
    //    matFrac = 1;
    //    matType = 1;

    //    lambda = 30.0;// cm

    //    posFrac = 5; 
    //    detType = 3;

    //    nEvents = 100;

    //    fracTop    = 4;
    //    fracRight  = 4;
    //    fracBottom = 4;
    //    fracLeft   = 4;
    //}
    ////if (false)
    ////{
    ////    G4cout<<"Arguments: crystal type (1-3), tapered (0 or other), lapping fraction (1-4), \
    ////        lapping type (1-9), lambda (~30),  detector type (1-4), \
    ////        position fraction (1-10, 0 and 11 being endpoints), number of events"<<G4endl;
    ////    G4cout<<"Argument for tapered crystal is either 0 (rectangular prism) or \
    ////        any other integer (standard tapered crystal)"<<G4endl;
    ////    return 0;
    ////}
    //else
    //{
    //    typeCr  = atoi(argv[1]); // type of crystal
    //    tapered = atoi(argv[2]);
    //    matFrac = atoi(argv[3]); // matting fraction
    //    matType = atoi(argv[4]); // matting geometry

    //    lambda  = atof(argv[5]); // absorption length

    //    detType = atoi(argv[6]); // detector type
    //    posFrac = atoi(argv[7]); // position 

    //    nEvents = atoi(argv[8]); // number of events

    //    if ((argc > 9) && (argc < 14))
    //    {
    //        fracTop = atoi(argv[9]);
    //        fracRight = atoi(argv[10]);
    //        fracBottom = atoi(argv[11]);
    //        fracLeft = atoi(argv[12]);
    //    }
    //} // Check nuber of arguments

    // Check if args defined properly
    if (detType>=1 && detType <=7 
            && posFrac>=1 && posFrac<=10 
            && lambda > 0 
            && matType >=1 && matType <=12 
            && matFrac >=0 && matFrac <=4 
            && typeCr >=1 && typeCr <=3 
            && nEvents > 0)
    {
        // Seed the random number generator manually
        G4long myseed = 
            typeCr*234901+tapered*29+matFrac*17+matType*173+lambda*13+detType*19+posFrac*137;
        if ((argc > 9) && (argc < 14))
        {
            myseed += 48762*fracTop + 24300*fracRight + 231*fracBottom + 1237987*fracLeft;
        }

        // Get timestamp
        struct timespec timeStamp;
        //timespec_get(&timeStamp, TIME_UTC); // C11
        clock_gettime(CLOCK_REALTIME, &timeStamp); // POSIX.1-2008
        G4long timeUTCnsec = timeStamp.tv_sec*(G4long)1.0e9;
        timeUTCnsec += timeStamp.tv_nsec;

        myseed += timeUTCnsec;
        myseed = 1;
        CLHEP::HepRandom::setTheSeed(myseed);

        // User Verbose output class
        G4VSteppingVerbose* verbosity = new LcSteppingVerbose;
        G4VSteppingVerbose::SetInstance(verbosity);

        // Run manager
        G4RunManager* runManager = new G4RunManager;

        // UserInitialization classes - mandatory
        G4VUserDetectorConstruction* detector = new LcDetectorConstruction(
                typeCr, tapered,
                matFrac, matType, 
                lambda, 
                detType, 
                fracTop, fracRight, fracBottom, fracLeft,
                modelSwitch,
                rindexOpticalGlue, rindexEpoxyAPD,
                vEnergy, vLambda);

        runManager-> SetUserInitialization(detector);

        G4VUserPhysicsList* physics = new LcPhysicsList;
        runManager-> SetUserInitialization(physics);

        // UserAction classes
        G4UserRunAction* run_action = new LcRunAction;
        runManager->SetUserAction(run_action);

        G4VUserPrimaryGeneratorAction* gen_action = new LcPrimaryGeneratorAction(typeCr, posFrac);
        runManager->SetUserAction(gen_action);

        G4UserEventAction* event_action = new LcEventAction(detType);
        runManager->SetUserAction(event_action);

        G4UserStackingAction* stacking_action = new LcStackingAction;
        runManager->SetUserAction(stacking_action);

        G4UserSteppingAction* stepping_action = new LcSteppingAction(detType);
        runManager->SetUserAction(stepping_action);

        // Initialize G4 kernel
        runManager->Initialize();

        // Create a ROOT file for output
        RootTreeHandler = new LcTreeHandler();
        RootTreeHandler->book(typeCr, tapered, matFrac, matType, lambda, detType, posFrac);

        // Create a general trajectory collection
        primTraceCollection=new LcUserTraceCollection();

#ifdef G4VIS_USE
        G4VisManager* visManager = new G4VisExecutive;
        visManager->Initialize();
        UImanager->ApplyCommand("/control/execute vis.mac");     
        ui->SessionStart();
#endif
#ifdef G4VIS_USE
        delete visManager;
#endif  

        UImanager->ApplyCommand("/control/verbose 0");
        UImanager->ApplyCommand("/run/verbose 0");
        UImanager->ApplyCommand("/event/verbose 0");
        UImanager->ApplyCommand("/tracking/verbose 0");
        sprintf(cname, "/run/beamOn %d", nEvents);
        UImanager->ApplyCommand(cname);

        RootTreeHandler->close();
        delete runManager;
        delete verbosity;

        return 0;
    } // check args defined properly
    else
    {
        G4cout<<"One or more input parameters are not valid. Check them carefully."<<G4endl;
        return 0;
    }

}
//....oooOO1OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
