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
// $Id: LcPhysicsList.cc,v 1.17 2009/11/10 05:16:23 gum Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ios.hh"
#include <iomanip>

#include "globals.hh"
#include "LcPhysicsList.hh"
#include "LcPhysicsListMessenger.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"


#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
////////////////////////////////////////////////////////////////////////////////
//optical model modification
#include "G4Cerenkov.hh"
#include "G4OpMieHG.hh"
////////////////////////////////////////////////////////////////////////////////

#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"
 
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

#include "G4Decay.hh"

////////////////////////////////////////////////////////////////////////////////
// Low energy em processes
//#include "G4EmLowEPPhysics.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4LowEPComptonModel.hh"
#include "G4LivermoreGammaConversionModel.hh"
#include "G4RayleighScattering.hh"
#include "G4LowEWentzelVIModel.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4LivermoreBremsstrahlungModel.hh"
#include "G4Generator2BS.hh"
#include "G4SystemOfUnits.hh"
#include "G4SeltzerBergerModel.hh"
#include "G4UniversalFluctuation.hh"
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//g4.10.03 modification
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
//------------------------------------------------------------------------------

#include "LcVars.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//optics modification
G4int LcPhysicsList::fVerboseLevel = 0;
G4int LcPhysicsList::fMaxNumPhotonStep = 20;
G4Cerenkov* LcPhysicsList::fCerenkovProcess = 0;
G4Scintillation* LcPhysicsList::fScintillationProcess = 0;
#ifndef NOABSORPTION
G4OpAbsorption* LcPhysicsList::fAbsorptionProcess = 0;
#endif /*NOABSORPTION*/
#ifndef NORAYLEIGH
G4OpRayleigh* LcPhysicsList::fRayleighScatteringProcess = 0;
#endif /*NORAYLEIGH*/
#ifndef NOMIE
G4OpMieHG* LcPhysicsList::fMieHGScatteringProcess = 0;
#endif /*NOMIE*/
G4OpBoundaryProcess* LcPhysicsList::fBoundaryProcess = 0;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

LcPhysicsList::LcPhysicsList() :  G4VUserPhysicsList()
{

  /*theScintillationProcess      = 0;
  theAbsorptionProcess         = 0;
  theRayleighScatteringProcess = 0;
  theBoundaryProcess           = 0;*/
  
  pMessenger = new LcPhysicsListMessenger(this);  
  SetVerboseLevel(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LcPhysicsList::~LcPhysicsList() { delete pMessenger;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program.

  /*ConstructBosons();
  ConstructLeptons();
  ConstructMesons();
  ConstructBaryons();*/

  //----------------------------------------------------------------------------
  //g4.10.03 particles construction modification
  G4BosonConstructor bConstructor;
  bConstructor.ConstructParticle();

  G4LeptonConstructor lConstructor;
  lConstructor.ConstructParticle();

  G4MesonConstructor mConstructor;
  mConstructor.ConstructParticle();

  G4BaryonConstructor rConstructor;
  rConstructor.ConstructParticle();

  G4IonConstructor iConstructor;
  iConstructor.ConstructParticle();
  //----------------------------------------------------------------------------
                    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*void LcPhysicsList::ConstructBosons()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  // gamma
  G4Gamma::GammaDefinition();

  // optical photon
  G4OpticalPhoton::OpticalPhotonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructLeptons()
{
  // leptons
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  G4MuonPlus::MuonPlusDefinition();
  G4MuonMinus::MuonMinusDefinition();
  G4NeutrinoMu::NeutrinoMuDefinition();
  G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructMesons()
{
 //  mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4PionZero::PionZeroDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructBaryons()
{
//  barions
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();
}*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructProcess()
{
  AddTransportation();
  //ConstructGeneral();
  //----------------------------------------------------------------------------
  //g4.10.03 modification
  ConstructDecay();
  //----------------------------------------------------------------------------
  ConstructEM();
  ConstructOp();
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void LcPhysicsList::ConstructGeneral()
//------------------------------------------------------------------------------
//g4.10.03 modification
void LcPhysicsList::ConstructDecay()
//
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  //----------------------------------------------------------------------------
  //g4.10.03 modification
  auto theParticleIterator = GetParticleIterator();
  //----------------------------------------------------------------------------
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructEM()
{
  //****************************************************************************
  //lowem part
  //G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  //****************************************************************************
  //----------------------------------------------------------------------------
  //g4.10.03 modification
  auto theParticleIterator = GetParticleIterator();
  //----------------------------------------------------------------------------
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {
    // gamma
      // Construct processes for gamma
      /*pmanager->AddDiscreteProcess(new G4GammaConversion());
      pmanager->AddDiscreteProcess(new G4ComptonScattering());
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());*/

      //************************************************************************
      //lowem part
      //Photoelectric effect - Livermore model only
      /*G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
      thePhotoElectricEffect->SetEmModel(new G4LivermorePhotoElectricModel(), 1);
      ph->RegisterProcess(thePhotoElectricEffect, particle);
      
      // Compton scattering - Livermore model above 20 MeV, Monarsh's model below 
      G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
      theComptonScattering->SetEmModel(new G4LivermoreComptonModel(),1);
      G4LowEPComptonModel* theLowEPComptonModel = new G4LowEPComptonModel();
      theLowEPComptonModel->SetHighEnergyLimit(20*MeV);
      theComptonScattering->AddEmModel(0, theLowEPComptonModel);
      ph->RegisterProcess(theComptonScattering, particle);
      
      // gamma conversion - Livermore model below 80 GeV
      G4GammaConversion* theGammaConversion = new G4GammaConversion();
      theGammaConversion->SetEmModel(new G4LivermoreGammaConversionModel(),1);
      ph->RegisterProcess(theGammaConversion, particle);
      
      // default Rayleigh scattering is Livermore
      G4RayleighScattering* theRayleigh = new G4RayleighScattering();
      ph->RegisterProcess(theRayleigh, particle);*/
      //G4LowEnergyPhotoElectric *theLEPhotoElectric = new G4LowEnergyPhotoElectric();
      //auto theLECompton = new G4LowEnergyCompton();
      //auto theLEGammaConversion = new G4LowEnergyGammaConversion();
      //auto theLERayleigh = new G44LowEnergyRayleigh();
      
      //Photoelectric effect - Livermore model only
      G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
      thePhotoElectricEffect->SetEmModel(new G4LivermorePhotoElectricModel(), 1);

      // Compton scattering - Livermore model above 20 MeV, Monarsh's model below 
      G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
      theComptonScattering->SetEmModel(new G4LivermoreComptonModel(),1);
      G4LowEPComptonModel* theLowEPComptonModel = new G4LowEPComptonModel();
      theLowEPComptonModel->SetHighEnergyLimit(20*MeV);
      theComptonScattering->AddEmModel(0, theLowEPComptonModel);

      // gamma conversion - Livermore model below 80 GeV
      G4GammaConversion* theGammaConversion = new G4GammaConversion();
      theGammaConversion->SetEmModel(new G4LivermoreGammaConversionModel(),1);

      // default Rayleigh scattering is Livermore
      G4RayleighScattering* theRayleigh = new G4RayleighScattering();

      pmanager->AddDiscreteProcess( thePhotoElectricEffect );
      pmanager->AddDiscreteProcess( theComptonScattering );
      pmanager->AddDiscreteProcess( theGammaConversion );
      pmanager->AddDiscreteProcess( theRayleigh );
      //************************************************************************

    } else if (particleName == "e-") {
    //electron
      // Construct processes for electron
      /*pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
      pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);*/
      
      //************************************************************************
      //lowem processes
      //theLEIonization = new G4LowEnergyIonization();
      //theLEBremsstrahlung = new G4LowEnergyBremsstrahlung();
      //theeminusMultipleScattering = new G4MultipleScattering();

      // multiple scattering
      G4eMultipleScattering* msc = new G4eMultipleScattering();
      msc->SetEmModel(new G4LowEWentzelVIModel(), 1);

      // Ionisation - Livermore should be used only for low energies
      G4eIonisation* eIoni = new G4eIonisation();
      G4LivermoreIonisationModel* theIoniLivermore = new G4LivermoreIonisationModel();
      theIoniLivermore->SetHighEnergyLimit(0.1*MeV); 
      eIoni->AddEmModel(0, theIoniLivermore, new G4UniversalFluctuation() );
      eIoni->SetStepFunction(0.2, 100*um);

      // Bremsstrahlung
      G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
      G4VEmModel* theBrem = new G4SeltzerBergerModel();
      theBrem->SetHighEnergyLimit(1*GeV);
      theBrem->SetAngularDistribution(new G4Generator2BS());
      eBrem->SetEmModel(theBrem, 1);

      pmanager->AddProcess( eIoni, -1, 2, 2 );
      pmanager->AddProcess( eBrem, -1, -1, 3 );
      pmanager->AddProcess( msc, -1, -1, 3 );
      //************************************************************************

    } else if (particleName == "e+") {
    //positron
      // Construct processes for positron
      /*pmanager->AddProcess(new G4eMultipleScattering(),-1, 1, 1);
      pmanager->AddProcess(new G4eIonisation(),       -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung(),   -1, 3, 3);
      pmanager->AddProcess(new G4eplusAnnihilation(),  0,-1, 4);*/

      //************************************************************************
      //lowem processes
      //theeplusIonization = new G4Ionization();
      //theeplusBremstrahlung = new G4eBremstrahlung();
      //theeplusMultipleScattering = new G4MultipleScattering();
      //theeplusAnnihilation = new G4eplusAnnihilation();

      // multiple scattering
      G4eMultipleScattering* msc = new G4eMultipleScattering();
      msc->SetEmModel(new G4LowEWentzelVIModel(), 1);

      // Standard ionisation
      G4eIonisation* eIoni = new G4eIonisation();
      eIoni->SetStepFunction(0.2, 100*um);

      // Bremsstrahlung
      G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
      G4VEmModel* theBrem = new G4SeltzerBergerModel();
      theBrem->SetHighEnergyLimit(1*GeV);
      theBrem->SetAngularDistribution(new G4Generator2BS());
      eBrem->SetEmModel(theBrem, 1);

      pmanager->AddProcess( eIoni, -1, 2, 2 );
      pmanager->AddProcess( eBrem, -1, -1, 3 );
      pmanager->AddProcess( msc, -1, 1, 1 );
      pmanager->AddProcess( new G4eplusAnnihilation(), 0, -1, 4 );
      //************************************************************************

    } else if( particleName == "mu+" ||
               particleName == "mu-"    ) {
    //muon
      // Construct processes for muon
      G4MuIonisation* muIoni = new G4MuIonisation();
      muIoni->SetStepFunction(0.2, 50*um);

      pmanager->AddProcess(new G4MuMultipleScattering(),-1, 1, 1);
      pmanager->AddProcess( muIoni, -1, 2, 2 );
      pmanager->AddProcess(new G4MuBremsstrahlung(),  -1, 3, 3);
      pmanager->AddProcess(new G4MuPairProduction(),  -1, 4, 4);

    } else {
      if ((particle->GetPDGCharge() != 0.0) &&
          (particle->GetParticleName() != "chargedgeantino")) {
     // all others charged particles except geantino
       pmanager->AddProcess(new G4hMultipleScattering(),-1,1,1);
       pmanager->AddProcess(new G4hIonisation(),       -1,2,2);
     }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::ConstructOp()
{

  /*theScintillationProcess = new G4Scintillation("Scintillation");
  theAbsorptionProcess     = new G4OpAbsorption();
  theRayleighScatteringProcess = new G4OpRayleigh();
  theBoundaryProcess  = new G4OpBoundaryProcess();


//  theScintillationProcess->DumpPhysicsTable();
//  theAbsorptionProcess->DumpPhysicsTable();
//  theRayleighScatteringProcess->DumpPhysicsTable();

  SetVerbose(0);//was previously 1
  
  
  theScintillationProcess->SetScintillationYieldFactor(1.);
  theScintillationProcess->SetTrackSecondariesFirst(true);

  // Use Birks Correction in the Scintillation process

  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  theScintillationProcess->AddSaturation(emSaturation);

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (theScintillationProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(theScintillationProcess);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
//      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
      pmanager->AddDiscreteProcess(theAbsorptionProcess);
      pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(theBoundaryProcess);
    }
  }*/

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //optics modification
  fCerenkovProcess = new G4Cerenkov("Cerenkov");
  fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotonStep);
  fCerenkovProcess->SetMaxBetaChangePerStep(10.0);
  fCerenkovProcess->SetTrackSecondariesFirst(true);
  fScintillationProcess = new G4Scintillation("Scintillation");
  fScintillationProcess->SetScintillationYieldFactor(1.);
  fScintillationProcess->SetTrackSecondariesFirst(true);
#ifndef NOABSORPTION
  fAbsorptionProcess = new G4OpAbsorption();
#endif /*NOABSORPTION*/
#ifndef NORAYLEIGH
  fRayleighScatteringProcess = new G4OpRayleigh();
#endif /*NORAYLEIGH*/
#ifndef NOMIE
  fMieHGScatteringProcess = new G4OpMieHG();
#endif /*NOMIE*/
  fBoundaryProcess = new G4OpBoundaryProcess();

  fCerenkovProcess->SetVerboseLevel(fVerboseLevel);
  fScintillationProcess->SetVerboseLevel(fVerboseLevel);
#ifndef NOABSORPTION
  fAbsorptionProcess->SetVerboseLevel(fVerboseLevel);
#endif /*NOABSORPTION*/
#ifndef NORAYLEIGH
  fRayleighScatteringProcess->SetVerboseLevel(fVerboseLevel);
#endif /*NORAYLEIGH*/
#ifndef NOMIE
  fMieHGScatteringProcess->SetVerboseLevel(fVerboseLevel);
#endif /*NOMIE*/
  fBoundaryProcess->SetVerboseLevel(fVerboseLevel);

  // Use Birks Correction in the Scintillation process
  //if(G4Threading::IsMasterThread())
  //{
    G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    fScintillationProcess->AddSaturation(emSaturation);
  //}
  
  auto theParticleIterator=GetParticleIterator();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (fCerenkovProcess->IsApplicable(*particle)) {
      //pmanager->AddProcess(fCerenkovProcess);
      //pmanager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
    }
    if (fScintillationProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(fScintillationProcess);
      pmanager->SetProcessOrderingToLast(fScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(fScintillationProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
#ifndef NOABSORPTION
      pmanager->AddDiscreteProcess(fAbsorptionProcess);
#endif /*NOABSORPTION*/
#ifndef NORAYLEIGH
      pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
#endif /*NORAYLEIGH*/
#ifndef NOMIE
      pmanager->AddDiscreteProcess(fMieHGScatteringProcess);
#endif /*NOMIE*/
      pmanager->AddDiscreteProcess(fBoundaryProcess);
    }
  }
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPhysicsList::SetVerbose(G4int verbose)
{
  /*theScintillationProcess->SetVerboseLevel(verbose);
  theAbsorptionProcess->SetVerboseLevel(verbose);
  theRayleighScatteringProcess->SetVerboseLevel(verbose);
  theBoundaryProcess->SetVerboseLevel(verbose);  */

  //----------------------------------------------------------------------------
  //g4.10.03 modification
  fVerboseLevel = verbose;

  fCerenkovProcess->SetVerboseLevel(fVerboseLevel);
  fScintillationProcess->SetVerboseLevel(fVerboseLevel);
#ifndef NOABSORPTION
  fAbsorptionProcess->SetVerboseLevel(fVerboseLevel);
#endif /*NOABSORPTION*/
#ifndef NORAYLEIGH
  fRayleighScatteringProcess->SetVerboseLevel(fVerboseLevel);
#endif /*NORAYLEIGH*/
#ifndef NOMIE
  fMieHGScatteringProcess->SetVerboseLevel(fVerboseLevel);
#endif /*NOMIE*/
  fBoundaryProcess->SetVerboseLevel(fVerboseLevel);
  //----------------------------------------------------------------------------
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//------------------------------------------------------------------------------
//g4.10.03 modification
void LcPhysicsList::SetNbOfPhotonsCerenkov(G4int MaxNumber)
{
  fMaxNumPhotonStep = MaxNumber;

  fCerenkovProcess->SetMaxNumPhotonsPerStep(fMaxNumPhotonStep);
}
//------------------------------------------------------------------------------

void LcPhysicsList::SetCuts()
{
  SetCutsWithDefault();
  
  if (verboseLevel>0) DumpCutValuesTable();   // was previously > and not >=
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
