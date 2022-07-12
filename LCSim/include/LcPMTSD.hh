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
// $Id: LcPMTSD.hh,v 1.7 2006/06/29 17:47:56 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LcPMTSD_h
#define LcPMTSD_h 1

#include "G4VSensitiveDetector.hh"
#include "LcPMTHit.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LcPMTSD : public G4VSensitiveDetector
{
    public:
        LcPMTSD(G4String);
        ~LcPMTSD();

        void Initialize(G4HCofThisEvent*);
        G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);

        //A version of processHits that keeps aStep constant
        G4bool ProcessHits_constStep(const G4Step* aStep,G4TouchableHistory* ROhist);//added in later from Lxe

        void EndOfEvent(G4HCofThisEvent*);
        G4int GetNumberOfHits(){return fNumberOfHits;};
        void  SetNumberOfHits(G4int numberOfHits){fNumberOfHits = numberOfHits;};
        void  IncNumberOfHits(){fNumberOfHits++;};

    private:
        LcPMTHitsCollection* PMTHitCollection;
        G4int fNumberOfHits = 0;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

