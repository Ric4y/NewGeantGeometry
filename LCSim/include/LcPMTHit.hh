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
// $Id: LcPMTHit.hh,v 1.8 2006/06/29 17:47:53 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LcPMTHit_h
#define LcPMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LcPMTHit : public G4VHit
{
  public:

      LcPMTHit();
     ~LcPMTHit();
      LcPMTHit(const LcPMTHit&);
      const LcPMTHit& operator=(const LcPMTHit&);
      G4int operator==(const LcPMTHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

//  inline void IncPhotonCount(){photons++;}//added
//  inline G4int GetPhotonCount(){return photons;}//added

  // inline void SetPMTPhysVol(G4VPhysicalVolume* PhysVol){this->physVol=PhysVol;}
  // inline G4VPhysicalVolume* GetPMTPhysVol(){return physVol;}//added
  public:
  
      // void SetTrackID  (G4int track)      { trackID = track; };
      // void SetPos      (G4ThreeVector xyz){ pos = xyz; };
  void SetTrackLength (G4double l){length = l; };
  void SetWavelength     (G4double w)      { wavelength = w; };
  void SetTrackID(G4int aTrackID) {mTrackID=aTrackID;};

      // G4int GetTrackID()    { return trackID; };
      // G4double GetEdep()    { return edep; };      
      // G4ThreeVector GetPos(){ return pos; };
  G4double GetTrackLength(){ return length; };
  G4double GetWavelength(){ return wavelength; };
  G4int GetTrackID() {return mTrackID;};

  private:
  
      // G4int         trackID;
      // G4double      edep;
      // G4ThreeVector pos;
  G4double      length;
  G4double wavelength;
  G4int mTrackID;
  //  G4int photons;//added
//  G4ThreeVector pos;
//  G4VPhysicalVolume* physVol;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<LcPMTHit> LcPMTHitsCollection;

extern G4Allocator<LcPMTHit> LcPMTHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* LcPMTHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) LcPMTHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void LcPMTHit::operator delete(void *aHit)
{
  LcPMTHitAllocator.FreeSingle((LcPMTHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
