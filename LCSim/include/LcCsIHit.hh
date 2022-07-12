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
// $Id: LcCsIHit.hh,v 1.8 2006/06/29 17:47:53 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LcCsIHit_h
#define LcCsIHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LcCsIHit : public G4VHit
{
    public:

        LcCsIHit();
        ~LcCsIHit();
        LcCsIHit(const LcCsIHit&);
        const LcCsIHit& operator=(const LcCsIHit&);
        G4int operator==(const LcCsIHit&) const;

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        void Draw();
        void Print();

        /// inline void IncPhotonCount(){photons++;}//added
        /// inline G4int GetPhotonCount(){return photons;}//added

        inline void SetCsIPhysVol(G4VPhysicalVolume* PhysVol){this->physVol=PhysVol;}
        /// inline G4VPhysicalVolume* GetCsIPhysVol(){return physVol;}//added
    public:

        void SetTrackID  (G4int track)      { trackID = track; };
        void SetEdep     (G4double de)      { edep = de; };
        void SetPos      (G4ThreeVector xyz){ pos = xyz; };

        G4int GetTrackID()    { return trackID; };
        G4double GetEdep()    { return edep; };      
        G4ThreeVector GetPos(){ return pos; };

        void SetScintillation()    { fScintillation = true; };
        void SetNoScintillation()  { fScintillation = false; };
        G4bool CheckScintillation(){ return fScintillation; };

    private:

        G4int         trackID;
        G4double      edep;
        G4ThreeVector pos;

        /// G4int photons;//added
        //  G4ThreeVector pos;
        G4VPhysicalVolume* physVol;
        G4bool        fScintillation = false;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<LcCsIHit> LcCsIHitsCollection;

extern G4Allocator<LcCsIHit> LcCsIHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* LcCsIHit::operator new(size_t)
{
    void *aHit;
    aHit = (void *) LcCsIHitAllocator.MallocSingle();
    return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void LcCsIHit::operator delete(void *aHit)
{
    LcCsIHitAllocator.FreeSingle((LcCsIHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
