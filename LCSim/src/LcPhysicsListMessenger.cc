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
// $Id: LcPhysicsListMessenger.cc,v 1.3 2006/06/29 17:54:25 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LcPhysicsListMessenger.hh"

#include "LcPhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LcPhysicsListMessenger::LcPhysicsListMessenger(LcPhysicsList* pPhys)
:pPhysicsList(pPhys)
{
  LcDir = new G4UIdirectory("/Lc/");
  LcDir->SetGuidance("UI commands of this example");
  
  physDir = new G4UIdirectory("/Lc/phys/");
  physDir->SetGuidance("PhysicsList control");
 
  verboseCmd = new G4UIcmdWithAnInteger("/Lc/phys/verbose",this);  
  verboseCmd->SetGuidance("set verbose for physics processes");
  verboseCmd->SetParameterName("verbose",true);
  verboseCmd->SetDefaultValue(0);//was previously 1
  verboseCmd->SetRange("verbose>=0");
  verboseCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LcPhysicsListMessenger::~LcPhysicsListMessenger()
{
  delete verboseCmd;
  delete physDir;
  delete LcDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue)
{       
  if( command == verboseCmd )
   { pPhysicsList->SetVerbose(verboseCmd->GetNewIntValue(newValue));}
   
///  if( command == cerenkovCmd )
///   {pPhysicsList->SetNbOfPhotonsCerenkov(cerenkovCmd->GetNewIntValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
