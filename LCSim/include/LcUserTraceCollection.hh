#ifndef LCUSERTRACECOLLECTIONHEADERDEF_h
#define LCUSERTRACECOLLECTIONHEADERDEF_h

//user class for collection of trajectory points

#include "LcUserTrace.hh"
#include <vector>
#include "G4Track.hh"
#include <stdio.h>

class LcUserTraceCollection
{
private:
    std::vector<LcUserTrace*> mCollection;
    G4ThreeVector mOrigin;
public:
    LcUserTraceCollection();
    void            AddTrace(G4Track* theTrack, int opBoundaryProcessStatus=-1);
    void            AddTrace(LcUserTrace* aTrace, int opBoundaryProcessStatus=-1);
    int             Clear();
    void            FindAndSetOrigin();
    int             GetEntries();
    G4ThreeVector   GetOrigin();
    LcUserTrace*    GetTrace(int aTrackID);
    LcUserTrace*    GetTraceByIndex(int i);
    void            Print();
};

#endif /*LCUSERTRACECOLLECTIONHEADERDEF_h*/
