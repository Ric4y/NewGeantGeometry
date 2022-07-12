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
// $Id: LcDetectorConstruction.hh,v 1.5 2006/06/29 17:53:55 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LcDetectorConstruction_h
#define LcDetectorConstruction_h 1

#include <vector>

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include "LcVars.hh"

using std::vector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class LcPMTSD;
class LcAPDSD;
class LcCsISD;
#ifdef PHOTON_COUNTER
class LcPhotonCounterSD;
#endif /*PHOTON_COUNTER*/
class LcDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  //LcDetectorConstruction(int a, int b, int c, int d, int e, int f, 
  //        int inFracTop, int inFracRight, int inFracBottom, int inFracLeft);
  LcDetectorConstruction(G4int a, G4int b, G4int c, G4int d, G4double e, G4int f, 
          G4int inFracTop, G4int inFracRight, G4int inFracBottom, G4int inFracLeft,
          G4bool modelSwitch, 
          G4double rindexOpticalGlue, G4double rindexEpoxyAPD,
          vector<double>* vEnergy, vector<double>* vLambda); 
  // modelSwitch = 0 -> UNUFIED, modelSwitch = 1 -> LUT
   ~LcDetectorConstruction();

  public:
 //   void  AddMaterial();
    G4VPhysicalVolume* Construct();
  private:
  //int crType, tapered, matFrac, matType, lambda, detType, fracTop, fracRight, fracBottom, fracLeft;
  G4int crType, tapered, matFrac, matType, detType, fracTop, fracRight, fracBottom, fracLeft;
  G4double lambda;
  G4bool fModelSwitch;
  G4double fRindexOpticalGlue;
  G4double fRindexEpoxyAPD;

  vector<double>* fEnergy;
  vector<double>* fLambda;

  static LcPMTSD* pmt_SD;
//#ifdef NOREFLECTION
// #ifdef TRACES
//  static LcPMTSD* csiAbs_SD;
// #endif /*TRACES*/
//#endif /*NOREFLECTION*/
  static LcAPDSD* apd_SD;
  static LcCsISD* CsI_SD;
#ifdef PHOTON_COUNTER
  static LcPhotonCounterSD* photonCounterSD;
#endif /*PHOTON_COUNTER*/

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*LcDetectorConstruction_h*/
