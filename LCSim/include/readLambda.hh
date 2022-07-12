#ifndef readLambda_h
 #define readLambda_h 1
 #include <stdio.h>
 #include <unistd.h>
 #include <vector>
 #include <string.h>
 #include <ctype.h>

using std::vector;

int readLambda(char* fileName, vector<double>* vEnergy, vector<double>* vLambda, int* lineErrNr);
int isNumber(const char* line);

#endif /*readLambda_h*/
