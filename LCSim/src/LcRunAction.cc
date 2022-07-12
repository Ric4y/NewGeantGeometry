//
// $Id: LcRunAction.cc,v 1.10 2006/06/29 17:54:31 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Make this appear first!
#include "LcStackingAction.hh"
#include "LcTreeHandler.hh"
#include "G4Timer.hh"
#include "LcRunAction.hh"
#include "G4Run.hh"
#include "G4ios.hh"

#include "G4UIcommand.hh"
#include "TApplication.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TObject.h"
#include <iostream>
#include <fstream>

extern LcTreeHandler* RootTreeHandler;

LcRunAction::LcRunAction()
  {timer = new G4Timer;  }

//LcRunAction::LcRunAction(const LcRunAction &run):G4UserRunAction()
//{}
LcRunAction::~LcRunAction()
    {      delete timer;   }

void LcRunAction::BeginOfRunAction(const G4Run* aRun)
  { G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl; 
    timer->Start();
  }

void LcRunAction::EndOfRunAction(const G4Run* aRun)
  {   
   timer->Stop();
      G4cout << "number of event = " << aRun->GetNumberOfEvent()<< " " << *timer << G4endl;
 
      // <<"     "<< PhotonsCount <<" Optical photons produces in this event "<<G4endl;
}

