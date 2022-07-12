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

#include "LcPMTSD.hh"
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
#include "Randomize.hh"

#include "LcVars.hh"

LcPMTSD::LcPMTSD(G4String name):G4VSensitiveDetector(name),PMTHitCollection(0)
{
    G4String HCname;
    collectionName.insert(HCname="PMTCollection");
}
LcPMTSD::~LcPMTSD(){ }

void LcPMTSD::Initialize(G4HCofThisEvent* HCE)
{ 
    PMTHitCollection = new LcPMTHitsCollection (SensitiveDetectorName,collectionName[0]); 
    static G4int HCID = -1;
    if(HCID<0) { 
        HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    }
    HCE->AddHitsCollection( HCID, PMTHitCollection );
    LcPMTSD::SetNumberOfHits(0);
}

G4bool LcPMTSD::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{ 
    if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

    const G4int nEntries = 40;
    G4double photonEnergySpectrum[nEntries];
    for (G4int i = 0; i < nEntries; i++)
    {
        photonEnergySpectrum[i] = (1.675 + 0.0478687*i)*eV; // even-spaced energy
    } // for (G4int i = 0; i < nEntriesCsI; i++)
    G4double efficiencyApd[nEntries]   = 
    {
        0.848, 0.855, 0.859, 0.861, 0.861, 0.861, 0.859, 0.856, 0.854, 0.851, 
        0.847, 0.843, 0.840, 0.835, 0.830, 0.823, 0.817, 0.810, 0.801, 0.791, 
        0.783, 0.773, 0.761, 0.747, 0.732, 0.721, 0.707, 0.693, 0.681, 0.664, 
        0.646, 0.622, 0.594, 0.563, 0.546, 0.533, 0.524, 0.515, 0.509, 0.503
    }; // from Hamamatsu s8664 series apd 1012 data sheet
    G4double diceRoll = CLHEP::RandFlat::shoot();
    
    G4double photonEnergy = aStep->GetTrack()->GetTotalEnergy();
    G4int energyIndex = 0;
    for (G4int i = 0; i < nEntries; i++)
    {
        if (photonEnergy > photonEnergySpectrum[i])
        {
            energyIndex = i;
        } // if (photonEnergy > photonEnergySpectrum[i])
        else break;
    } // for (G4int i = 0; i < nEntries; i++)

    //G4cout << "Photon energy: " << photonEnergy/eV << " eV" 
    //       << "; Efficiency: " << efficiencyApd[energyIndex] 
    //       << "; Dice roll: " << diceRoll << G4endl;

    if (diceRoll > efficiencyApd[energyIndex])
    {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return false;
    } // if (diceRoll > efficiencyApd[energyIndex])

    //G4double edep = aStep->GetTotalEnergyDeposit();
    G4double edep = photonEnergy;
#ifndef NOREFLECTOR
    if(edep == 0.) return false;
#endif /*NOREFLECTOR*/
    LcPMTHit* newHit = new LcPMTHit();
#ifndef NOREFLECTOR
    newHit->SetWavelength(1.2398/edep/1000.);//conversion of photon energy to wavelength: 
                                             //lambda=hbar*c/E
#else
    newHit->SetWavelength(1234.8/aStep->GetTrack()->GetTotalEnergy());
#endif /*NOREFLECTOR*/
    newHit->SetTrackLength(aStep->GetTrack()->GetTrackLength());
    newHit->SetTrackID(aStep->GetTrack()->GetTrackID());

    PMTHitCollection->insert(newHit);
    LcPMTSD::IncNumberOfHits();
    //G4cout << "In volume: " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
    //       << "; Nomber of hits: " << LcPMTSD::GetNumberOfHits() <<  G4endl;
    
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);

    return true;
}
G4bool LcPMTSD::ProcessHits_constStep( const G4Step* aStep,G4TouchableHistory*)
{  
    return false;
}

void LcPMTSD::EndOfEvent(G4HCofThisEvent* )
{  
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
