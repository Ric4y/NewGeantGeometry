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
// $Id: LcPhysicsList.hh,v 1.7 2006/06/29 17:53:59 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef LcPhysicsList_h
#define LcPhysicsList_h 1

#include "globals.hh"
#include "G4VUserPhysicsList.hh"

//------------------------------------------------------------------------------
//g4.10.03 modification
class G4Cerenkov;
//------------------------------------------------------------------------------
class G4Scintillation;
class G4OpAbsorption;
class G4OpRayleigh;
class G4OpBoundaryProcess;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//optical processes modification
class G4OpMieHG;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class LcPhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LcPhysicsList : public G4VUserPhysicsList
{
  public:
    LcPhysicsList();
   ~LcPhysicsList();

  public:
    void ConstructParticle();
    void ConstructProcess();

    void SetCuts();

    //these methods Construct particles
    /*void ConstructBosons();
    void ConstructLeptons();
    void ConstructMesons();
    void ConstructBaryons();*/

    //these methods Construct physics processes and register them
    //void ConstructGeneral();
    void ConstructEM();
    void ConstructOp();
    //--------------------------------------------------------------------------
    //g4.10.03 modification
    void ConstructDecay();
    //--------------------------------------------------------------------------
    
    //for the Messenger 
    void SetVerbose(G4int);
    //--------------------------------------------------------------------------
    //g4.10.03
    void SetNbOfPhotonsCerenkov( G4int );
    //--------------------------------------------------------------------------

///   void SetNbOfPhotonsCerenkov(G4int);
    
  private:
    
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //optical processes modification
    static G4int fVerboseLevel;
    static G4int fMaxNumPhotonStep;

    static G4Cerenkov* fCerenkovProcess;
    static G4Scintillation* fScintillationProcess;
    static G4OpAbsorption* fAbsorptionProcess;
    static G4OpRayleigh* fRayleighScatteringProcess;
    static G4OpMieHG* fMieHGScatteringProcess;
    static G4OpBoundaryProcess* fBoundaryProcess;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
///    G4Cerenkov*          theCerenkovProcess;
    /*G4Scintillation*     theScintillationProcess;
    G4OpAbsorption*      theAbsorptionProcess;
    G4OpRayleigh*        theRayleighScatteringProcess;
    G4OpBoundaryProcess* theBoundaryProcess;*/
    
    LcPhysicsListMessenger* pMessenger;   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /* LcPhysicsList_h */

