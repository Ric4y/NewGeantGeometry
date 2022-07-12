#include "LcUserTrace.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"

LcUserTrace::LcUserTrace()
{
    mTrackID=-1;
    mPoint.clear();
    mProcessID.clear();
    mParticleID=-1;
    mParentTrackID=-1;
    mCreatorProcessID=-1;
}

int LcUserTrace::ParseProcess(G4String processName)
{
    if (processName=="particleGun")         return 1;
    else if (processName=="phot")           return 2;
    else if (processName=="compt")          return 3;
    else if (processName=="eBrem")          return 4;
    else if (processName=="Scintillation")  return 5;
    else if (processName=="Cerenkov")       return 6;
    else if (processName=="Transportation") return 7;
    else if (processName=="OpAbsorption")   return 8;
    else if (processName=="eIoni")          return 9;
    else
    {
        //G4cout<<"~~~~Unidentified process: "<<processName<<G4endl;
        return -1;
    }
}

G4String LcUserTrace::UnParseProcess(int processID)
{
    G4String processName;
    switch (processID)
    {
        case 1:
            processName="particleGun";
            break;
        case 2:
            processName="phot";
            break;
        case 3:
            processName="compt";
            break;
        case 4:
            processName="eBrem";
            break;
        case 5:
            processName="Scintillation";
            break;
        case 6:
            processName="Cerenkov";
            break;
        case 7:
            processName="Transportation";
            break;
        case 8:
            processName="OpAbsorption";
            break;
        case 9:
            processName="eIoni";
            break;
        case -1:
            processName="idFail";
            break;
        default:
            processName="parseFail";
            break;
    }
    return processName;
}

int LcUserTrace::ParseParticle(G4String particleName)
{
    if (particleName=="gamma")              return 1;
    else if (particleName=="e-")            return 2;
    else if (particleName=="e+")            return 3;
    else if (particleName=="opticalphoton") return 4;
    else
    {
        //G4cout<<"@@@@Unidentified particle: "<<particleName<<G4endl;
        return -1;
    }
}

G4String LcUserTrace::UnParseParticle(int particleID)
{
    G4String particleName;
    switch (particleID)
    {
        case 1:
            particleName="gamma";
            break;
        case 2:
            particleName="e-";
            break;
        case 3:
            particleName="e+";
            break;
        case 4:
            particleName="opticalphoton";
            break;
        case -1:
            particleName="idFail";
            break;
        default:
            particleName="parseFail";
            break;
    }
    return particleName;
}

bool LcUserTrace::CheckTrackID(int id)
{
    if (id==mTrackID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LcUserTrace::IsTrackIDFound(int id)
{
    if ((mTrackID==id)&&(mTrackID!=-1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void LcUserTrace::SetTrackID(int id)
{
    mTrackID=id;
}

int LcUserTrace::GetTrackID()
{
    return mTrackID;
}

int LcUserTrace::GetEntries()
{
    return (int)mPoint.size();
}

void LcUserTrace::AddPoint(G4ThreeVector point)
{
    mPoint.push_back(point);
}

void LcUserTrace::AddProcess(G4ThreeVector point, G4String processName)
{
    mPoint.push_back(point);
    mProcessID.push_back(ParseProcess(processName));
}

void LcUserTrace::AddProcess(G4ThreeVector point, int processID)
{
    mPoint.push_back(point);
    mProcessID.push_back(processID);
}

G4ThreeVector LcUserTrace::GetPoint(int i)
{
    if ((i>=0)&&(i<(int)mPoint.size()))
    {
        return mPoint[i];
    }
    else
    {
        G4ThreeVector nullVector=G4ThreeVector(0.0,0.0,0.0);
        return nullVector;
    }
}

int LcUserTrace::GetProcessID(int i)
{
    if ((i>=0)&&(i<(int)mProcessID.size()))
    {
        return mProcessID[i];
    }
    else
    {
        return 0;
    }
}

G4String LcUserTrace::GetProcessName(int i)
{
    if ((i>=0)&&(i<(int)mProcessID.size()))
    {
        return UnParseProcess(mProcessID[i]);
    }
    else
    {
        G4String outRange="outRange";
        return outRange;
    }
}

void LcUserTrace::SetParticle(G4String particleName)
{
    mParticleID=ParseParticle(particleName);
}

int LcUserTrace::GetParticleID()
{
    return mParticleID;
}

G4String LcUserTrace::GetParticleName()
{
    return UnParseParticle(mParticleID);
}

void LcUserTrace::SetCreatorProcess(G4String creatorProcessName)
{
    mCreatorProcessID=ParseProcess(creatorProcessName);
}

int LcUserTrace::GetCreatorProcessID()
{
    return mCreatorProcessID;
}

G4String LcUserTrace::GetCreatorProcessName()
{
    return UnParseProcess(mCreatorProcessID);
}

void LcUserTrace::SetParentTrackID(int parentTrackID)
{
    mParentTrackID=parentTrackID;
}

int LcUserTrace::GetParentTrackID()
{
    return mParentTrackID;
}

void LcUserTrace::Print()
{
    if (((int)mPoint.size())==((int)mProcessID.size()))
    {
        //printf("TrackID:%4d; ParentID:%4d; Particle:%s\n",
        //        mTrackID,mParentTrackID,(UnParseParticle(mParticleID)).c_str());
        printf("TrackID:%4d; ParentID:%4d; Track entries:%3d; Particle:%s\n",
                mTrackID,mParentTrackID,mPoint.size(),(UnParseParticle(mParticleID)).c_str());
        for (int i=0;i<(int)mPoint.size();i++)
        {
            if(mParticleID==4)//opticalphoton
            {
                printf("%6d:(%+4.3e,%+4.3e,%+4.3e): %3d\n",
                        i,(mPoint[i]).x(),(mPoint[i]).y(),(mPoint[i]).z(),mProcessID[i]);
            }
            else //not opticalphoton
            {
                printf("%6d:(%+4.3e,%+4.3e,%+4.3e): %s\n",
                        i,(mPoint[i]).x(),(mPoint[i]).y(),(mPoint[i]).z(),
                        (UnParseProcess(mProcessID[i])).c_str());
            }
        }
    }
    else
    {
        printf("!!! Vector size mismatch\n");
    }
}

int LcUserTrace::Clear()
{
    mTrackID=-1;
    mPoint.clear();
    mProcessID.clear();
    mParticleID=-1;
    mParentTrackID=-1;
    mCreatorProcessID=-1;
    return 0;
}
