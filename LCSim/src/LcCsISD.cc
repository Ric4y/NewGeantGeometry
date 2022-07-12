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
// $Id: LcCsISD.cc,v 1.9 2006/06/29 17:48:27 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//#include "Event.hh"
#include "LcCsISD.hh"
#include "LcCsIHit.hh"
#include "LcDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

#include "G4ProcessManager.hh"
#include "G4SteppingManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Scintillation.hh"

//extern float exNbHits;
LcCsISD::LcCsISD(G4String name):G4VSensitiveDetector(name),CsIHitCollection(0)
{
    G4String HCname;
    collectionName.insert(HCname="CsICollection");
}
LcCsISD::~LcCsISD(){ }

void LcCsISD::Initialize(G4HCofThisEvent* HCE)
{ 
    CsIHitCollection = new LcCsIHitsCollection (SensitiveDetectorName,collectionName[0]); 
    static G4int HCID = -1;
    if(HCID<0) { 
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    }
    HCE->AddHitsCollection( HCID, CsIHitCollection ); 
    this->ev_nr_photons = 0;
}

G4bool LcCsISD::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
    G4Track* theTrack = aStep->GetTrack();
    //G4bool processScintillation = false;
    //if (theTrack->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    //{
    //    if (theTrack->GetCreatorProcess()->GetProcessName() != "Scintillation")
    //    //if ((theTrack->GetCreatorProcess()->GetProcessName() != "phot")
    //    //        || (theTrack->GetCreatorProcess()->GetProcessName() != "compt"))
    //    {
    //        return false;
    //    } // (theTrack->GetCreatorProcess()->GetProcessName() != "Scintillation")
    //    else // (theTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
    //    {
    //        processScintillation = true;
    //    } // (theTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
    //}
    if (theTrack->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        return false;
    }
    // get the volume hit 
    G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep == 0.0) return false;

    G4VPhysicalVolume* physVol = aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);
    LcCsIHit* newHit = new LcCsIHit();
    newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    newHit->SetEdep(edep);
    newHit->SetCsIPhysVol(physVol);
    newHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
    //if (processScintillation == true)
    //{
    //    newHit->SetScintillation();
    //} // if (processScintillation)
    //else // if (!processScintillation)
    //{
    //    //G4cout << "No scintillation!" << G4endl;
    //    newHit->SetNoScintillation();
    //} // if (!processScintillation)
    CsIHitCollection->insert(newHit);
    newHit->Print();
//find if scint photons were created and how many



  auto proc_man = aStep->GetTrack()->GetDynamicParticle()->GetParticleDefinition()->GetProcessManager();
  G4int n_proc = proc_man->GetPostStepProcessVector()->entries();
  G4ProcessVector* proc_vec = proc_man->GetPostStepProcessVector(typeDoIt);

  G4int n_scint = 0;
  for (G4int i = 0; i < n_proc; ++i) {
    if ((*proc_vec)[i]->GetProcessName().compare("Scintillation") == 0) {
      auto scint = (G4Scintillation*)(*proc_vec)[i];
      n_scint = scint->GetNumPhotons();
    }
  }
   //G4cout << "In this CsI hit, " << n_scint
   //        << " scintillation photons were produced." << G4endl;

          

      this->ev_nr_photons = this->ev_nr_photons + n_scint;
    return true;
}
//original one
G4bool LcCsISD::ProcessHits_constStep( const G4Step* ,G4TouchableHistory*)
{
    return false;
}

void LcCsISD::EndOfEvent(G4HCofThisEvent* )
{
    //     G4double NbHits = CsIHitCollection->entries();
    //   for (G4int i=0;i<NbHits;i++) {(*CsIHitCollection)[i]->Print();}
    G4cout << "In this event " << this->ev_nr_photons << " photons were created." << G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

