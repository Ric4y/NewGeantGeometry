#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "readLambda.hh"

using std::vector;

int readLambda(char* fileName, vector<double>* vEnergyIn, vector<double>* vLambdaIn, int* lineErrNr)
{
    //const char* fileName = "lambda_spec.txt";
    //int* lineErrNr = 0;
    vector<double>* vEnergy = new vector<double>();
    vector<double>* vLambda = new vector<double>();
    int exitStatus = 0;
    vEnergyIn->clear();
    vLambdaIn->clear();
    if (access(fileName, F_OK) != -1)
    {
        FILE* lambdaFile = fopen(fileName, "r");
        char chLine[1024], chEnergy[128], chLambda[128];
        int lineSize = sizeof(chLine);
        int lineNr = 0;
        while (fgets(chLine, lineSize, lambdaFile))
        {
            lineNr++;
            if (chLine[0] == '#')
            {
                continue;
            }
            else
            {
                sscanf(chLine, "%s %s", chEnergy, chLambda);
                //if (!isNumber(chEnergy) || !isNumber(chLambda))
                //{
                    //vEnergy->clear();
                    //vLambda->clear();
                    //*lineErrNr = lineNr;
                    //exitStatus = 2;
                    //break;
                //}
                //else
                //{
                    vEnergyIn->push_back((double)atof(chEnergy));
                    vLambdaIn->push_back((double)atof(chLambda));
                //}
            }
        }
    }
    else
    {
        exitStatus = 1;
    }
    printf(">>>>>>>>>>> readLambda: exitStatus = %d; lineNr = %d\n", exitStatus, *lineErrNr);
    //if (!exitStatus)
    //{
    //    vEnergyIn = vEnergy;
    //    vLambdaIn = vLambda;
    //}

    return exitStatus;
}

int isNumber(const char* line)
{
    int status = 1;
    int length = 0;
    while (*line)
    {
        if (!isdigit(*line++))
        {
            status = 0;
            break;
        }
        length++;
    }
    return status*length;
}
