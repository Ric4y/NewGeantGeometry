#include "LcUserTraceCollection.hh"
#include "LcUserTrace.hh"
#include <vector>
#include "G4Track.hh"
#include "G4String.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"

LcUserTraceCollection::LcUserTraceCollection()
{
    mCollection.clear();
}

void LcUserTraceCollection::AddTrace(G4Track* theTrack, int opBoundaryProcessStatus)
{
    opBoundaryProcessStatus+=10;
    bool found=false;
    for (int i=0;i<(int)mCollection.size();i++)
    {
        if (mCollection[i]->IsTrackIDFound(theTrack->GetTrackID()))
        {
            if(theTrack->GetDefinition()->GetParticleName()=="opticalphoton") //opticalphoton
            {
                mCollection[i]->AddProcess(theTrack->GetPosition(),opBoundaryProcessStatus);
            }
            else //not opticalphoton
            {
                mCollection[i]->AddProcess(theTrack->GetPosition(),
                        theTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
            }
            found=true;
            break;
        }
    }
    if (!found)
    {
        G4String process;
        LcUserTrace* aTrace=new LcUserTrace();
        aTrace->SetTrackID(theTrack->GetTrackID());
        aTrace->SetParentTrackID(theTrack->GetParentID());
        aTrace->SetParticle(theTrack->GetDefinition()->GetParticleName());
        if (theTrack->GetCreatorProcess()==NULL)
        {
            //G4String particleGun="particleGun";
            //aTrace->SetCreatorProcess(particleGun);
            //aTrace->AddProcess(theTrack->GetVertexPosition(),particleGun);
            process="particleGun";
        }
        else
        {
            //aTrace->SetCreatorProcess(theTrack->GetCreatorProcess()->GetProcessName());
            //aTrace->AddProcess(theTrack->GetVertexPosition(), 
            //        theTrack->GetCreatorProcess()->GetProcessName());
            process=theTrack->GetCreatorProcess()->GetProcessName();
        }
        aTrace->SetCreatorProcess(process);
        aTrace->AddProcess(theTrack->GetVertexPosition(),process);
        if(theTrack->GetDefinition()->GetParticleName()=="opticalphoton") //opticalphoton
        {
            //G4cout
            //    <<"preStep status:"<<theTrack->GetStep()->GetPreStepPoint()->GetStepStatus()
            //    <<";postStep status:"<<theTrack->GetStep()->GetPostStepPoint()->GetStepStatus()
            //    <<G4endl;
            aTrace->AddProcess(theTrack->GetVertexPosition(),opBoundaryProcessStatus);
        }
        else //not opticalphoton
        {
            aTrace->AddProcess(theTrack->GetPosition(),
                    theTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
        }
        mCollection.push_back(aTrace);
        //delete aTrace;
    }
}

void LcUserTraceCollection::AddTrace(LcUserTrace* aTrace, int opBoundaryProcessStatus)
{
    bool found=false;
    for (int i=0;i<(int)mCollection.size();i++)
    {
        if (mCollection[i]->IsTrackIDFound(aTrace->GetTrackID()))
        {
            //G4cout<<"traceCollection: trace "<<aTrace->GetTrackID()<<" already exists\n";
            found=true;
            break;
        }
    }
    if (!found)
    {
        mCollection.push_back(aTrace);
    }
}

LcUserTrace* LcUserTraceCollection::GetTrace(int aTrackID)
{
    bool found=false;
    int index=0;
    for (int i=0;i<(int)mCollection.size();i++)
    {
        if (mCollection[i]->IsTrackIDFound(aTrackID))
        {
            index=i;
            found=true;
            break;
        }
    }
    if (found)
    {
        return mCollection[index];
    }
    else
    {
        return NULL;
    }
}

LcUserTrace* LcUserTraceCollection::GetTraceByIndex(int i)
{
    if ((i>=0)&&(i<(int)mCollection.size()))
    {
        return mCollection[i];
    }
    else
    {
        return NULL;
    }
}

int LcUserTraceCollection::GetEntries()
{
    //printf("LcUserTraceCollection size check\n");
    return (int)mCollection.size();
}

void LcUserTraceCollection::Print()
{
    printf("oooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOooo\n");
    for (int i=0;i<(int)mCollection.size();i++)
    {
        (mCollection[i])->Print();
    }
    printf("oooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOoooOOOooo\n");
}

void LcUserTraceCollection::FindAndSetOrigin()
{
    for(int i=0;i<(mCollection[0])->GetEntries();i++)
    {
        //if((mCollection[0])->GetProcessID(i)!=7) // is not Transportation
        if(((mCollection[0])->GetProcessName(i)=="phot")||((mCollection[0])->GetProcessName(i)=="compt"))
        {
            mOrigin=(mCollection[0])->GetPoint(i);
            break;
        }
    }
}

G4ThreeVector LcUserTraceCollection::GetOrigin()
{
    return mOrigin;
}

int LcUserTraceCollection::Clear()
{
    //mOrigin=G4ThreeVector(0.0,0.0,0.0);
    mCollection.clear();
    return 0;
}
