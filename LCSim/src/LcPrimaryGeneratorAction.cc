
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "LcPrimaryGeneratorAction.hh"
#include "LcPrimaryGeneratorMessenger.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <stdio.h>
#include <math.h>

#include "LcVars.hh"
#ifdef NOREFLECTOR
 #include <stdlib.h>
 #include <time.h>
 //#include "RandomEngine.h"
 //#include "RandFlat.h"
 //using namespace CLHEP;
#endif /*NOREFLECTOR*/

//LcPrimaryGeneratorAction::LcPrimaryGeneratorAction(int crType, int posFrac)
//{
//  G4double pos;
//  G4int n_particle = 1;
//  particleGun = new G4ParticleGun(n_particle);
//  gunMessenger = new LcPrimaryGeneratorMessenger(this);
//  //default kinematic  //
//  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//  G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
//  particleGun->SetParticleDefinition(particle);
//  particleGun->SetParticleTime(0.0*ns);
//  if (crType==1) // 17-cm crystal
//    pos = 170.*posFrac/11.; 
//  else if (crType==2) // 18-cm crystal
//    pos = 180.*posFrac/11.; 
//  else if (crType==3) // 22-cm crystal
//    pos = 220.*posFrac/11.; 
//  // x-coordinate is approximately symmetric to .5 mm
//  //particleGun->SetParticlePosition(G4ThreeVector(-10*mm,40.0*mm,pos*mm)); 
//  particleGun->SetParticlePosition(G4ThreeVector(-10.0*mm, 60.0*mm, pos*mm)); 
//  particleGun->SetParticleEnergy(0.662*MeV);//0.6620*MeV from the 137Cs source
//  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,-1.0,0.0));
//}
LcPrimaryGeneratorAction::LcPrimaryGeneratorAction(int crType, int posFrac)
{
  G4double pos;
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  gunMessenger = new LcPrimaryGeneratorMessenger(this);
  //default kinematic  //
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
#ifndef NOREFLECTOR 
  G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
#else
  G4ParticleDefinition* particle = particleTable->FindParticle("opticalphoton");
#endif /*NOREFLECTOR*/
  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleTime(0.0*ns);
  if (crType==1) // 17-cm crystal
    pos = 170.*posFrac/11.; 
  else if (crType==2) // 18-cm crystal
    pos = 180.*posFrac/11.; 
  else if (crType==3) // 22-cm crystal
    pos = 220.*posFrac/11.; 
  // x-coordinate is approximately symmetric to .5 mm
  //particleGun->SetParticlePosition(G4ThreeVector(-10*mm,40.0*mm,pos*mm));
  G4double xCoor = 0.0;
  G4double yCoor = 1.0;
  G4double zCoor = 0.0;
  G4double gammaDistY = -60.0;// mm
  G4double gammaDistX = -10.0;// mm
#ifdef COLL_GAMMA
  //G4double cosPhi = G4RandFlat::shoot(-1.0, 1.0);
  //G4double sinPhi = sqrt(1.0 - pow(cosPhi, 2.0));
  //G4double cosTheta = 0.998;// 1 - Omega/2pi, Omega = 10 msr
  //cosTheta = G4RandFlat::shoot(0.0, cosTheta);
  //G4double sinTheta = sqrt(1.0 - pow(cosTheta, 2.0));
  //xCoor = sinTheta*cosPhi;
  //yCoor = sinTheta*sinPhi;
  //zCoor = cosTheta;
  gammaDistY = -93.0;// mm
  gammaDistX =  -7.5;// mm
#endif /*COLL_GAMMA*/
#ifndef NOREFLECTOR 
  //particleGun->SetParticlePosition(G4ThreeVector(-10.0*mm, 60.0*mm, pos*mm)); 
  //particleGun->SetParticlePosition(G4ThreeVector(-10.0*mm, -60.0*mm, pos*mm)); 
  particleGun->SetParticlePosition(G4ThreeVector(gammaDistX*mm, gammaDistY*mm, pos*mm)); 
  particleGun->SetParticleEnergy(0.662*MeV);//0.6620*MeV from the 137Cs source
  //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,-1.0,0.0));
  //particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 1.0, 0.0));
  particleGun->SetParticleMomentumDirection(G4ThreeVector(xCoor, yCoor, zCoor));
  printf(">>>>>> ParticleGun origin: %3.2f, %3.2f, %3.2f; ParticleGun direction: %3.2f, %3.2f, %3.2f\n",
          gammaDistX, gammaDistY, pos, xCoor, yCoor, zCoor);
#else
  particleGun->SetParticlePosition(G4ThreeVector(-11.4125*mm, 17.045*mm, (170.0 + 20.0*(double)crType)*mm)); 
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,-1.0));
#endif /*NOREFLECTOR*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LcPrimaryGeneratorAction::~LcPrimaryGeneratorAction()
{
  delete particleGun;
  delete gunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LcPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
#ifdef NOREFLECTOR
 #ifndef SIMPLE_OPTICS
  G4double redLimit = 800.0;// nm
  G4double blueLimit = 300.0;// nm
  //HepRandomEngine* anRndEngine;
  //anRndEngine->SetSeed(1234567890);
  //RandFlat* aRandFlat = new RandFlat(anRndEngine);
  //srand((unsigned int)time(NULL));
  //double rnd = (blueLimit + (redLimit - blueLimit)*(double)rand()/RAND_MAX);
  double rnd = G4RandFlat::shoot(blueLimit, redLimit);
 #else
  double rnd = 550.0;// nm
 #endif /*SIMPLE_OPTICS*/
  printf("wl = %f\n", rnd);
  particleGun->SetParticleEnergy(
          1239.8/rnd);// optical photon with 550 nm wavelength
#endif /*NOREFLECTOR*/
  G4double xCoor = 0.0;
  G4double yCoor = 1.0;
  G4double zCoor = 0.0;
#ifdef COLL_GAMMA
  G4double omega = 0.010;// 10 msr
  //G4double omega = 9.08e-5;// 1 mm diameter spot on the entrance surface
  omega = G4RandFlat::shoot(0.0, omega);
  G4double cosTheta = 1.0 - (omega/(2.0*M_PI));// 1 - Omega/2pi, Omega = 10 msr
  G4double theta = acos(cosTheta);
  //cosTheta = cos(theta);
  //G4double sinTheta = sqrt(1.0 - pow(cos(theta), 2.0));
  G4double phi = G4RandFlat::shoot(0.0, 2.0*M_PI);
  //G4double cosPhi = cos(G4RandFlat::shoot(0.0, 2.0*M_PI));
  //G4double sinPhi = sqrt(1.0 - pow(cosPhi, 2.0));
  xCoor = sin(theta)*sin(phi);
  zCoor = sin(theta)*cos(phi);
  yCoor = cos(theta);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(xCoor, yCoor, zCoor));
  //printf(">>>>>> omega = %4.3f\n", omega);
#endif /*COLL_GAMMA*/
  //printf(">>>>>> xCoor = %3.2f, yCoor = %3.2f, zCoor = %3.2f\n", xCoor, yCoor, zCoor);
  //printf(">>>>>> xCoor^2 + yCoor^2 + zCoor^2 = %3.2f\n", 
  //        pow(xCoor, 2.0) + pow(yCoor, 2.0) + pow(zCoor, 2.0));
  printf(">>>>>> ParticleGun direction: %3.2f, %3.2f, %3.2f\n", 
          particleGun->GetParticleMomentumDirection().getX(), 
          particleGun->GetParticleMomentumDirection().getY(), 
          particleGun->GetParticleMomentumDirection().getZ());
    particleGun->GeneratePrimaryVertex(anEvent);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void LcPrimaryGeneratorAction::SetOptPhotonPolar()
{
 G4double angle = G4UniformRand() * 360.0*deg;
 SetOptPhotonPolar(angle);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LcPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
 if (particleGun->GetParticleDefinition()->GetParticleName() != "opticalphoton")
   {
     G4cout << "--> warning from PrimaryGeneratorAction::SetOptPhotonPolar() :"
               "the particleGun is not an opticalphoton" << G4endl;
     return;
   }
     	       
 G4ThreeVector normal (1., 0., 0.);
 G4ThreeVector kphoton = particleGun->GetParticleMomentumDirection();
 G4ThreeVector product = normal.cross(kphoton); 
 G4double modul2       = product*product;
 
 G4ThreeVector e_perpend (0., 0., 1.);
 if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product; 
 G4ThreeVector e_paralle    = e_perpend.cross(kphoton);
 
 G4ThreeVector polar = std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
 particleGun->SetParticlePolarization(polar);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
