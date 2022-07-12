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
// $Id: LcPMTSD.cc,v 1.9 2006/06/29 17:48:27 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//#include "Event.hh"
#include "LcPhotonCounterSD.hh"
#include "LcPMTHit.hh"
#include "LcDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Trajectory.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "LcVars.hh"

LcPhotonCounterSD::LcPhotonCounterSD(
        G4String name):G4VSensitiveDetector(name), fPhotonCounterHitCollection(0)
{ 
    G4String HCname;
    collectionName.insert(HCname="photonCounterCollection");
}
LcPhotonCounterSD::~LcPhotonCounterSD(){ }

void LcPhotonCounterSD::Initialize(G4HCofThisEvent* HCE)
{ 
    fPhotonCounterHitCollection = new LcPMTHitsCollection(SensitiveDetectorName, collectionName[0]); 
    static G4int HCID = -1;
    if(HCID < 0) 
    { 
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    }
    HCE->AddHitsCollection(HCID, fPhotonCounterHitCollection);

    LcPhotonCounterSD::SetNumberOfHits(0);
}

G4bool LcPhotonCounterSD::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
    if (aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;
    G4double edep = aStep->GetTotalEnergyDeposit();
    
    LcPMTHit* newHit = new LcPMTHit();
#ifndef NOREFLECTOR
    newHit->SetWavelength(1.2398/edep/1000.);//conversion of photon energy to wavelength: 
                                             //lambda=hbar*c/E
#else
    newHit->SetWavelength(1234.8/aStep->GetTrack()->GetTotalEnergy());
#endif /*NOREFLECTOR*/
    newHit->SetTrackLength(aStep->GetTrack()->GetTrackLength());
    newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    fPhotonCounterHitCollection->insert(newHit);

    LcPhotonCounterSD::IncNumberOfHits();

    //G4cout << "Volume: " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
    //       << "; Hits: " << LcPhotonCounterSD::GetNumberOfHits() << G4endl;

    return true;
}

G4bool LcPhotonCounterSD::ProcessHits_constStep( const G4Step* aStep, G4TouchableHistory*)
{  
    return false;
}

void LcPhotonCounterSD::EndOfEvent(G4HCofThisEvent* )
{  
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

