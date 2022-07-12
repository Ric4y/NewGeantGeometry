#ifndef LCUSERTRACEHEADERDEF_h
#define LCUSERTRACEHEADERDEF_h

//user trace class

#include <vector>
#include "G4ThreeVector.hh"
#include "G4String.hh"
#include <stdio.h>

class LcUserTrace
{
private:
    int mTrackID;
    std::vector<G4ThreeVector> mPoint;
    std::vector<int> mProcessID;
    int mParticleID;
    int mParentTrackID;
    int mCreatorProcessID;
    int ParseProcess(G4String processName);
    int ParseParticle(G4String particleName);
    G4String UnParseProcess(int processID);
    G4String UnParseParticle(int particleID);
public:
    LcUserTrace();
    void            AddPoint(G4ThreeVector point);
    void            AddProcess(G4ThreeVector point, G4String processName);
    void            AddProcess(G4ThreeVector point, int processID);
    bool            CheckTrackID(int id);
    int             Clear();
    int             GetCreatorProcessID();
    G4String        GetCreatorProcessName();
    int             GetEntries();
    int             GetParentTrackID();
    int             GetParticleID();
    G4String        GetParticleName();
    G4ThreeVector   GetPoint(int i);
    int             GetProcessID(int i);
    G4String        GetProcessName(int i);
    int             GetTrackID();
    bool            IsTrackIDFound(int id);
    void            Print();
    void            SetCreatorProcess(G4String creatorProcessName);
    void            SetParentTrackID(int parentTrackID);
    void            SetParticle(G4String particleName);
    void            SetTrackID(int id);
};

#endif /*LCUSERTRACEHEADERDEF_h*/
