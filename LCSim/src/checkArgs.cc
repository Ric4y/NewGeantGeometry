#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

#include "checkArgs.hh"
#include "G4Types.hh"
#include "G4ios.hh"
#include "readLambda.hh"

using std::vector;

int printUsage()
{
    G4cout << "Usage: lcsim --crystal_size=SIZE --detector_type=D_TYPE --source_position=S_POS" << G4endl
           << "             --events=N_EVENTS --polished_top=P_TOP --polished_right=P_RIGHT" <<G4endl
           << "             --polished_bottom=P_BOTTOM --polished_left=P_LEFT" << G4endl 
           << "             [--model=MOD] [--rindex_epoxy=REPOXY] [--rindex_opglue=RGLUE]" << G4endl 
           << "             [--lambda_spectrum=/path/to/lambda_spectrum.txt]" << G4endl 
           << "             [--help]" << G4endl << G4endl
           << " -c SIZE,     --crystal_size=SIZE" << G4endl
           << "                    type of a crystal for simulation:" << G4endl
           << "                    SIZE=1 - 17 cm long CALIFA crystal" << G4endl
           << "                    SIZE=2 - 18 cm long CALIFA crystal" << G4endl
           << "                    SIZE=3 - 22 cm long CALIFA crystal" << G4endl << G4endl
           << " -d SIZE,     --detector_type=D_TYPE" << G4endl
           << "                    type of a light readout sensor:" << G4endl
           << "                    D_TYPE=1 - full face PMT" << G4endl
           << "                    D_TYPE=3 - Hamamatsu LAAPD" << G4endl << G4endl
           << " -p S_POS,    --source_position=S_POS" << G4endl
           << "                    one of 10 eqiudistant positions of a gamma source above the" << G4endl
           << "                    crystal, with 1 being the closest to the light read out sensor:" << G4endl
           << "                    S_POS=1..10" << G4endl << G4endl
           << " -N N_EVENTS, --events=N_EVENTS" << G4endl
           << "                    number of events to simulate:" << G4endl
           << "                    N_EVENTS > 0" << G4endl << G4endl
           << " -t P_TOP,    --polished_top=P_TOP" << G4endl
           << "                    polished fraction of the top surface of the crystal," << G4endl
           << "                    starting from the opposite to the sensor side:"  << G4endl
           << "                    P_TOP=0 -   0% polished" << G4endl
           << "                    P_TOP=1 -  25% polished" << G4endl
           << "                    P_TOP=2 -  50% polished" << G4endl
           << "                    P_TOP=3 -  75% polished" << G4endl
           << "                    P_TOP=4 - 100% polished" << G4endl << G4endl
           << " -r P_RIGHT,  --polished_right=P_RIGHT" << G4endl
           << "                    polished fraction of the right surface of the crystal" << G4endl
           << "                    (looking from the sensor),"  << G4endl
           << "                    starting from the opposite to the sensor side:"  << G4endl
           << "                    P_RIGHT=0 -   0% polished" << G4endl
           << "                    P_RIGHT=1 -  25% polished" << G4endl
           << "                    P_RIGHT=2 -  50% polished" << G4endl
           << "                    P_RIGHT=3 -  75% polished" << G4endl
           << "                    P_RIGHT=4 - 100% polished" << G4endl << G4endl
           << " -b P_BOTTOM, --polished_bottom=P_BOTTOM" << G4endl
           << "                    polished fraction of the bottom surface of the crystal," << G4endl
           << "                    starting from the opposite to the sensor side:"  << G4endl
           << "                    P_BOTTOM=0 -   0% polished" << G4endl
           << "                    P_BOTTOM=1 -  25% polished" << G4endl
           << "                    P_BOTTOM=2 -  50% polished" << G4endl
           << "                    P_BOTTOM=3 -  75% polished" << G4endl
           << "                    P_BOTTOM=4 - 100% polished" << G4endl << G4endl
           << " -l P_LEFT,   --polished_left=P_LEFT" << G4endl
           << "                    polished fraction of the left surface of the crystal" << G4endl
           << "                    (looking from the sensor),"  << G4endl
           << "                    starting from the opposite to the sensor side:"  << G4endl
           << "                    P_LEFT=0 -   0% polished" << G4endl
           << "                    P_LEFT=1 -  25% polished" << G4endl
           << "                    P_LEFT=2 -  50% polished" << G4endl
           << "                    P_LEFT=3 -  75% polished" << G4endl
           << "                    P_LEFT=4 - 100% polished" << G4endl << G4endl
           << " -m MOD,      --model=MOD" << G4endl
           << "                    optical model:" << G4endl
           << "                    MOD=0 - UNIFIED"  << G4endl
           << "                    MOD=1 - realistic surface defined by a look-up table" << G4endl
           << "                    default value: MOD=1" << G4endl << G4endl
           << " -e REPOXY,   --rindex_epoxy=REPOXY" << G4endl
           << "                    refractive index of epoxy casing of the Hamamatsu LAAPD" << G4endl
           << "                    default value: REPOXY=1.53" << G4endl << G4endl
           << " -g RGLUE,    --rindex_opglue=RGLUE" << G4endl
           << "                    refractive index of optical glue between the LAAPD and the crystal" 
           << G4endl
           << " -s /path/to/lambda_spectrum.txt," << G4endl
           << "              --lambda_spectrum=/path/to/lambda_spectrum.txt" << G4endl
           << "                    path to the file containing dependendy of absorption" << G4endl
           << "                    length of CsI(Tl) on the light wavelength" << G4endl
           << G4endl
           << "                    file format:" << G4endl
           << "                        wavelength[ev] absorption_length[cm]" << G4endl << G4endl
           << " -h,          --help" << G4endl
           << "                    display this help and exit." << G4endl;
    return 0;
}

int checkArgs(int* argc, char* argv[], 
        G4int* typeCr, G4int* tapered, G4int* matFrac, G4int* matType, G4double* lambda, 
        G4int* detType, G4int* posFrac, G4int* nEvents,
        G4int* fracTop, G4int* fracRight, G4int* fracBottom, G4int* fracLeft,
        G4bool* modelSwitch,
        G4double* rindexOpticalGlue, G4double* rindexEpoxyAPD, 
        vector<double>* vEnergy, vector<double>* vLambda)
{
    //setting defaults, arguments in order expected if no key given

    //-c --crystal_size
    *typeCr  = 1;  // geometry, 1 - 17 cm, 2 - 18 cm, 3-22 cm, default 17 cm
    //no key availble
    *tapered = 1;  // this variable is never used
    //no key available
    *matFrac = 0;  // only used for MatType < 10, default
    //no key availble
    *matType = 10; // this means that alex k's strategies are used, 1-9 Guillaume's
    //no key availble
    *lambda = 30.0;// cm
    //-d --detector_type
    *detType = 3;  // Hama 2x1cm2 surface APD 
    //-p --source_position
    *posFrac = 5;  // Gamma gun position at 5th point of 10
    //-N --events
    *nEvents = 100;// default nr of events is 100
    // if mattype == 10 these arguments are needed, defaults are 
    //-t --polished_top
    *fracTop    = 4; //fully polished top side
    //-r --polished_right
    *fracRight  = 4; //fully polished right side seen from APD
    //-b --polished_bottom
    *fracBottom = 4; //fully polished bottom side, non-tapered
    //-l --polished_left
    *fracLeft   = 4; //fully polished left side, seen from APD
    //the following arguments can only be given with the corresponfding key on the command line
    //-m, --model
    *modelSwitch = 1; // LUT
    //-g, --rindex_opglue
    *rindexOpticalGlue = 1.406; // from catalogue
    //-e, --rindex_epocy
    *rindexEpoxyAPD = 1.53; // from catalogue
    double energyLambda[] = 
    {
        1.6570,
        1.7049,
        1.7527,
        1.8006,
        1.8485,
        1.8963,
        1.9442,
        1.9921,
        2.0399,
        2.0878,
        2.1357,
        2.1836,
        2.2314,
        2.2793,
        2.3272,
        2.3750,
        2.4229,
        2.4708,
        2.5186,
        2.5665,
        2.6144,
        2.6622,
        2.7101,
        2.7580,
        2.8058,
        2.8537,
        2.9016,
        2.9495,
        2.9973,
        3.0452,
        3.0931,
        3.1409,
        3.1888,
        3.2367,
        3.2845,
        3.3324,
        3.3803,
        3.4281,
        3.4760,
        3.5239
    };
    double lambdaArr[] = 
    {
        42.375,
        41.324,
        40.338,
        39.541,
        38.664,
        37.836,
        37.036,
        36.278,
        35.556,
        34.856,
        34.179,
        33.507,
        32.936,
        32.379,
        31.756,
        31.001,
        29.875,
        29.069,
        28.643,
        28.307,
        27.691,
        27.175,
        26.717,
        26.237,
        25.791,
        25.334,
        24.891,
        24.450,
        24.007,
        23.528,
        23.031,
        22.547,
        22.096,
        21.663,
        21.284,
        20.831,
        20.295,
        19.584,
        18.597,
        17.054
    };
    int energyLambdaSize = sizeof(energyLambda)/sizeof(energyLambda[0]);
    int lambdaSize = sizeof(lambdaArr)/sizeof(lambdaArr[0]);
    lambdaSize = ((lambdaSize < energyLambdaSize)? lambdaSize : energyLambdaSize);
    vEnergy->clear();
    vLambda->clear();
    for (int i = 0; i < lambdaSize; i++)
    {
        vEnergy->push_back(energyLambda[i]);
        vLambda->push_back(lambdaArr[i]);
    }

    G4int locArgc = *argc;
    G4int obligatoryArgs = 8;
    G4int oldInputArgs = 8;
    G4int oldInputCustomGeometryArgs = 12;

    //int readLambda(char* fileName, vector<double>* vEnergyIn, vector<double>* vLambdaIn, int* lineErrNr)
    int lineErrNr = 0;
    int readLambdaStatus = 0;
    char* inFile = 0;

    static struct option inputOptions[] = 
    {
        {"crystal_size",    required_argument, 0, 'c'},
        {"detector_type",   required_argument, 0, 'd'},
        {"source_position", required_argument, 0, 'p'},
        {"events",          required_argument, 0, 'N'},
        {"polished_top",    required_argument, 0, 't'},
        {"polished_right",  required_argument, 0, 'r'},
        {"polished_bottom", required_argument, 0, 'b'},
        {"polished_left",   required_argument, 0, 'l'},
        {"model",           required_argument, 0, 'm'},
        {"rindex_epoxy",    required_argument, 0, 'e'},
        {"rindex_opglue",   required_argument, 0, 'g'},
        {"lambda_spectrum", required_argument, 0, 's'},
        {"help",            no_argument,       0, 'h'},
        {0,                 0,                 0,  0 },
    };

                    const char* suka = "lambda_spec.txt";
                    char* blyat = (char*)malloc(128);
                    strcpy(blyat, suka);
    G4int opt = 0;
    G4int argsCounter = 0;
    //G4bool missingOperand = false;
    if (locArgc == 1)
    {
        G4cout << "No arguments found, running with defaults" << G4endl;
        G4cout << "typeCr = " << *typeCr << G4endl;
        G4cout << "tapered = " << *tapered << G4endl;
        G4cout << "matFrac = " << *matFrac << G4endl;
        G4cout << "matType = " << *matType << G4endl;
        G4cout << "lambda = " << *lambda << G4endl;
        G4cout << "detType = " << *detType << G4endl;
        G4cout << "posFrac = " << *posFrac << G4endl;
        G4cout << "nEvents = " << *nEvents << G4endl;
        G4cout << "fracTop = " << *fracTop << G4endl;
        G4cout << "fracRight = " << *fracRight << G4endl;
        G4cout << "fracBottom = " << *fracBottom << G4endl;
        G4cout << "fracLeft = " << *fracLeft << G4endl;
        G4cout << "modelSwitch = " << *modelSwitch << G4endl;
        G4cout << "rindexEpoxyAPD = " << *rindexEpoxyAPD << G4endl;
        G4cout << "rindexOpticalGlue = " << *rindexOpticalGlue << G4endl;
        G4cout << "lambda spectrum:\n";
        for (int i = 0; i < lambdaSize; i++)
        {
            G4cout << "\t" << vEnergy->at(i) << " ev; " << vLambda->at(i) << " cm\n";
        }

        return 1;
    } // if (locArgc == 1)
    else
    {
        while ((opt = getopt_long(locArgc, argv, "c:d:p:N:t:r:b:l:m:e:g:s:h", inputOptions, 0)) != -1)
        {
            switch (opt)
            {
                case 'c':
                    argsCounter++;
                    *typeCr = atoi(optarg);
                    break;
                case 'd':
                    argsCounter++;
                    *detType = atoi(optarg);
                    break;
                case 'p':
                    argsCounter++;
                    *posFrac = atoi(optarg);
                    break;
                case 'N':
                    argsCounter++;
                    *nEvents = atoi(optarg);
                    break;
                case 't':
                    argsCounter++;
                    *fracTop = atoi(optarg);
                    break;
                case 'r':
                    argsCounter++;
                    *fracRight = atoi(optarg);
                    break;
                case 'b':
                    argsCounter++;
                    *fracBottom = atoi(optarg);
                    break;
                case 'l':
                    argsCounter++;
                    *fracLeft = atoi(optarg);
                    break;
                case 'm':
                    *modelSwitch = (bool)atoi(optarg);
                    break;
                case 'e':
                    *rindexEpoxyAPD = atof(optarg);
                    break;
                case 'g':
                    *rindexOpticalGlue = atof(optarg);
                    break;
                case 's':
                    readLambdaStatus = readLambda(blyat, vEnergy, vLambda, &lineErrNr);
                    break;
                case 'h':
                    printUsage();
                    return -2;
                    break;
                case ':':
                    G4cout << "Missing operand, check input." << G4endl;
                    return -1;
                    break;
            } // switch (opt)
        } // while ((opt = getopt_long(*argc, argv, "c:d:p:N:t:r:b:l:h", inputOptions, 0)) != -1)
    } // if (locArgc != 1)
        G4cout << "lambda spectrum:\n";
        for (int i = 0; i < lambdaSize; i++)
        {
            G4cout << "\t" << vEnergy->at(i) << " ev; " << vLambda->at(i) << " cm\n";
        }

    //if (readLambdaStatus > 0)
    //{
    //    switch(readLambdaStatus)
    //    {
    //        case 1:
    //            G4cout << "File " << inFile << " does not exist\n";
    //            break;
    //        case 2:
    //            G4cout << "Non-digit at line " << lineErrNr << G4endl;
    //            break;
    //    }
    //    G4cout << "Default values have not been changed:\n";
    //    for (int i = 0; i < lambdaSize; i++)
    //    {
    //        G4cout << "\t" << vEnergy->at(i) << " ev; " << vLambda->at(i) << " cm\n";
    //    }
    //}

    if ((optind == locArgc) && (obligatoryArgs - argsCounter > 0))
    {
        G4cout << "Some arguments missing, check input." << G4endl;
        return -1;
    } // if ((optind == locArgc) && (obligatoryArgs - argsCounter > 0))
    else if (locArgc != optind)
    {
        if (locArgc - optind < oldInputArgs)
        {
            G4cout << "Some arguments missing, check input." << G4endl;
            return -1;
        } // if (locArgc - optind < oldInputArgs)
        else
        {
            *typeCr = atoi(argv[1]);
            *matFrac = atoi(argv[3]);
            *matType = atoi(argv[4]);

            *lambda = atof(argv[5]);

            *detType = atoi(argv[6]);
            *posFrac = atoi(argv[7]);

            *nEvents = atoi(argv[8]);
        } // if (locArgc - optind >= oldInputArgs)
        if (locArgc - optind == oldInputCustomGeometryArgs)
        {
            *fracTop = atoi(argv[9]);
            *fracRight = atoi(argv[10]);
            *fracBottom = atoi(argv[11]);
            *fracLeft = atoi(argv[12]);
        } // if (locArgc - optind == oldInputCustomGeometryArgs)
    } // else if (locArgc != optind)

    return 0;
}
