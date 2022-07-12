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
// $Id: LcEventAction.hh,v 1.2 2006/06/29 17:53:57 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
#ifndef LcEventAction_h
#define LcEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "LcVars.hh"

class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class LcRunAction;
class LcEventAction;
class LcEventAction : public G4UserEventAction
{
  public:
    LcEventAction(int a);
   ~LcEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);

  void SetEventVerbose(G4int v){verbose=v;}

  void SetPMTThreshold(G4int t){pmtThreshold=t;}  
  void SetAPDThreshold(G4int a){apdThreshold=a;}
  void SetCSIThreshold(G4int c){csiThreshold=c;}

  void SetForceDrawPhotons(G4bool b){forcedrawphotons=b;}
  void SetForceDrawNoPhotons(G4bool b){forcenophotons=b;}
 private:
// LcRunAction* runAct;
  G4int detType;
  G4int  pmtCollID; 
  G4int  apdCollID;
  G4int  csiCollID;
#ifdef PHOTON_COUNTER
  G4int photonCounterCollID;
#endif /*PHOTON_COUNTER*/
  G4int  verbose;
  G4int  pmtThreshold;  
  G4int  apdThreshold;
  G4int  csiThreshold;
  G4bool forcedrawphotons;
  G4bool forcenophotons; 
  LcRunAction* runAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
