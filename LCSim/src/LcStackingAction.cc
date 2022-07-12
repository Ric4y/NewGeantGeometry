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
// $Id: LcStackingAction.cc,v 1.5 2006/06/29 17:54:34 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LcStackingAction.hh"
#include "LcUserEventInformation.hh"
#include "LcSteppingAction.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

#include "LcTreeHandler.hh"
#include "LcEvent.hh"
#include "TTree.h"

 extern LcTreeHandler* RootTreeHandler;
 LcStackingAction::LcStackingAction(): PhotonsCount(0){}
 LcStackingAction::~LcStackingAction(){}

 G4ClassificationOfNewTrack
 LcStackingAction::ClassifyNewTrack(const G4Track * aTrack)
  {
   if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
      { // particle is optical photon
	//G4cout<<aTrack->GetParentID()<<G4endl;
        if(aTrack->GetParentID()> 0)
         { // particle is secondary
           //  Wavelengths[0]=0.0;
	   Wavelengths[PhotonsCount] = 1.2398/aTrack->GetKineticEnergy()/1000;
	   ////ParentIDs[PhotonsCount] = aTrack->GetParentID();
	   ////Wavelengths[PhotonsCount] = aTrack->GetKineticEnergy();
	   ////G4cout<<Wavelengths[PhotonsCount]<<G4endl;
           PhotonsCount++;
         }
     }
  return fUrgent;
 }

void LcStackingAction::NewStage()
{
//G4cout <<"     "<< PhotonsCount <<" Total Optical photons produced in this event "<< G4endl;
  RootTreeHandler->SetNpp(PhotonsCount);
  RootTreeHandler->SetWpp(Wavelengths);
  //RootTreeHandler->SetIpp(ParentIDs);

}
void LcStackingAction::PrepareNewEvent(){ PhotonsCount = 0; }

void LcStackingAction::EndofEvent()
      {       }


