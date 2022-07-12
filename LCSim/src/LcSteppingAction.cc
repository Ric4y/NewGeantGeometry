//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: LcSteppingAction.cc,v 1.9 2006/06/29 17:48:18 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
// 
#include "LcSteppingAction.hh"
#include "LcEventAction.hh"
//#include "LcTrajectory.hh"
#include "LcPMTSD.hh"
#include "LcAPDSD.hh"
#include "LcCsISD.hh"
#include "LcUserTrackInformation.hh"
#include "LcUserEventInformation.hh"

#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4SystemOfUnits.hh"

#include "LcUserTrace.hh"
#include "LcUserTraceCollection.hh"

#include "LcVars.hh"

#ifdef PHOTON_COUNTER
#include "LcPhotonCounterSD.hh"
#endif /*PHOTON_COUNTER*/

extern LcUserTraceCollection* userTraceCollection;
extern LcUserTraceCollection* primTraceCollection;

LcSteppingAction::LcSteppingAction(int a):oneStepPrimaries(false)
{
    detType = a;
}

void LcSteppingAction::UserSteppingAction(const G4Step* theStep)
{ 
    G4Track* theTrack = theStep->GetTrack();
    G4int TrackID = theTrack->GetTrackID();

    G4StepPoint* thePrePoint = theStep->GetPreStepPoint();

    G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
    G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
    const G4ThreeVector xyz = thePostPoint->GetPosition();

    G4OpBoundaryProcessStatus boundaryStatus=Undefined;
    static G4OpBoundaryProcess* boundary=NULL;

    //find the boundary process only once
    if(!boundary)
    {
        G4ProcessManager* pm 
            = theTrack->GetDefinition()->GetProcessManager();
        G4int nprocesses = pm->GetProcessListLength();
        G4ProcessVector* pv = pm->GetProcessList();
        G4int i;
        for( i=0;i<nprocesses;i++){
            if((*pv)[i]->GetProcessName()=="OpBoundary")
            {
                boundary = (G4OpBoundaryProcess*)(*pv)[i];
                break;
            }
        }
    }
    
    if(!thePostPV){//out of world
        return;
    }
    
    G4ParticleDefinition* particleType = theTrack->GetDefinition();
    if(particleType==G4OpticalPhoton::OpticalPhotonDefinition())
    {
        if(thePostPV->GetName()=="PhyWorld") // Kill photons entering expHall from something other than Slab
        {
            theTrack->SetTrackStatus(fStopAndKill);
        }
        
        boundaryStatus=boundary->GetStatus();
        //Check to see if the particle was actually at a boundary
        //Otherwise the boundary status may not be valid
        //Prior to Geant4.6.0-p1 this would not have been enough to check

#ifdef NOREFLECTOR
        if (thePostPoint->GetPhysicalVolume()->GetName() == "phyApd1")
        {
            primTraceCollection->AddTrace(theTrack, G4OpBoundaryProcessStatus::Detection);
            // manually set to detected
        }
        else if (boundary)
        {
            if (boundary->GetStatus() != 10) // not detected
                primTraceCollection->AddTrace(theTrack,boundary->GetStatus());
        }
        else
        {
            primTraceCollection->AddTrace(theTrack,-1);
        }
#endif /*NOREFLECTOR*/
        if(thePostPoint->GetStepStatus()==fGeomBoundary){;
            switch(boundaryStatus){
                case Absorption:
                case Detection:
                case FresnelReflection:
                case TotalInternalReflection:
                case SpikeReflection:
                    break;
                default:
                    break;
            } // switch(boundaryStatus)
        } // if(thePostPoint->GetStepStatus()==fGeomBoundary)
    } // if(particleType==G4OpticalPhoton::OpticalPhotonDefinition())
#ifndef NOREFLECTOR
#ifdef TRACES
    if (boundary)
    {
        primTraceCollection->AddTrace(theTrack,boundary->GetStatus());
    }
    else
    {
        primTraceCollection->AddTrace(theTrack,-1);
    }
#endif /*TRACES*/
#endif /*NOREFLECTOR*/
    if (TR_VERBOSE)
    {
        //if (thePrePoint->GetStepStatus()<7) // fUndefined=7
        if(theTrack->GetCreatorProcess()!=NULL)
        {
            G4cout 
                <<"TrackID: "<<theTrack->GetTrackID()
                <<"; ParentID: "<<theTrack->GetParentID()
                <<"; Particle: "<<theTrack->GetDefinition()->GetParticleName()
                <<"; CreatorProcess: " <<theTrack->GetCreatorProcess()->GetProcessName()
                <<"; Post Point: "<<thePostPoint->GetProcessDefinedStep()->GetProcessName()
                <<G4endl;
        }
        else
        {
            G4cout 
                <<"TrackID: "<<theTrack->GetTrackID()
                <<"; ParentID: "<<theTrack->GetParentID()
                <<"; Particle: "<<theTrack->GetDefinition()->GetParticleName()
                <<"; CreatorProcess: particleGun"
                <<"; Post Point: "<<thePostPoint->GetProcessDefinedStep()->GetProcessName()
                <<G4endl;
        }
    }
}


