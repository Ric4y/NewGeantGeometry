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
#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#ifndef LcUserEventInformation_h
#define LcUserEventInformation_h 1

class LcUserEventInformation : public G4VUserEventInformation
{
public:
  LcUserEventInformation();
  ~LcUserEventInformation();

  inline void Print()const{};

  void IncAbsorption(){absorptionCount++;}
  void IncBoundaryAbsorption(){boundaryAbsorptionCount++;}
  G4int GetAbsorptionCount()const {return absorptionCount;}
  G4int GetBoundaryAbsorptionCount() const {return boundaryAbsorptionCount;}

private:
  G4int absorptionCount;
  G4int boundaryAbsorptionCount;
};

#endif





