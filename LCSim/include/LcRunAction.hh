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
// $Id: LcRunAction.hh,v 1.9 2006/06/29 17:54:10 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LcRunAction_h
#define LcRunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"

#include "TTree.h"
#include <iostream>
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Timer;
class G4Run;

class LcRunAction : public G4UserRunAction
{
  public:
    LcRunAction();
    LcRunAction(const LcRunAction &run);
   ~LcRunAction();
  private:
 // TTree *tree;

 // int noofevents;
  int eventnumber;
  std::ifstream infile;

  float PhotonsCount;
 // float *NbHits;
 // float *edep;
 // float *EventID;

  public:

  float GetPhotonsCount();
 // float *GetNbHits();
 // float *Getedep();
 // float *GetEventID();

    void BeginOfRunAction(const G4Run* aRun);
    void EndOfRunAction(const G4Run* aRun);
 // TTree *GetTree();
  private:
    G4Timer* timer;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*LcRunAction_h*/
