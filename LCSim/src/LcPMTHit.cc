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
// $Id: LcPMTHit.cc,v 1.10 2006/06/29 17:48:24 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LcPMTHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

 G4Allocator<LcPMTHit> LcPMTHitAllocator;
 LcPMTHit::LcPMTHit() {}
 LcPMTHit::~LcPMTHit() {}
 LcPMTHit::LcPMTHit(const LcPMTHit& right) : G4VHit()
  {
    // trackID   = right.trackID;
    // edep      = right.edep;
    // pos       = right.pos;  
    wavelength    = right.wavelength;
    length    = right.length;
#ifdef NEW_GEOMETRY
    globalTime = right.globalTime;

    wavelength2    = right.wavelength2;
    length2    = right.length2;
    globalTime2 = right.globalTime2;
    //localTime = right.localTime;
    //properTime = right.properTime;
#endif /*NEW_GEOMETRY*/
  }
const LcPMTHit& LcPMTHit::operator=(const LcPMTHit& right)
  {
    // trackID   = right.trackID;
    // edep      = right.edep;
    // pos       = right.pos; 
    wavelength    = right.wavelength;
    length    = right.length;
#ifdef NEW_GEOMETRY
    globalTime = right.globalTime;

    wavelength2    = right.wavelength2;
    length2    = right.length2;
    globalTime2 = right.globalTime2;
    //localTime = right.localTime;
    //properTime = right.properTime;
#endif /*NEW_GEOMETRY*/
    return *this;
  }
G4int LcPMTHit::operator==(const LcPMTHit& right) const
  {
    return (this==&right) ? 1 : 0;
  }

void LcPMTHit::Draw(){
  // G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  // if(pVVisManager){
  //   G4Circle circle(pos);
  //   circle.SetScreenSize(2.);
  //   circle.SetFillStyle(G4Circle::filled);
  //   G4Colour colour(1.,0.,0.);
  //   G4VisAttributes attribs(colour);
  //   circle.SetVisAttributes(attribs);
  //   pVVisManager->Draw(circle);
  // }
}

void LcPMTHit::Print()
  {
//   G4cout << "  trackID: " << trackID << "  energy deposit: " << G4BestUnit(edep,"Energy") << "  position: " << G4BestUnit(pos,"Length") << G4endl;
  }

