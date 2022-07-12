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
// $Id: LcEventAction.cc,v 1.3 2006/06/29 17:54:20 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "globals.hh"
#include "LcEventAction.hh"
#include "LcPMTHit.hh"
#include "LcPMTSD.hh"
#include "LcAPDHit.hh"
#include "LcAPDSD.hh"
#include "LcCsIHit.hh"
#include "LcCsISD.hh"
#include "LcRunAction.hh"
#include "LcUserEventInformation.hh"
#include "LcStackingAction.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4TrackingManager.hh"
#include "G4RunManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "LcTreeHandler.hh"
#include "LcEvent.hh"
#include "TTree.h"

#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <math.h>

#include "LcUserTraceCollection.hh"
#include "LcUserTrace.hh"

#include "LcVars.hh"

#ifdef PHOTON_COUNTER
#include "LcPhotonCounterSD.hh"
#endif /*PHOTON_COUNTER*/

extern LcTreeHandler* RootTreeHandler;
extern LcUserTraceCollection* userTraceCollection;
extern LcUserTraceCollection* primTraceCollection;
//extern LcUserTraceCollection* gammTraceCollection;
#ifdef NOREFLECTOR
int detectedCounter = 0;
#endif /*NOREFLECTOR*/

#ifndef PHOTON_COUNTER
LcEventAction::LcEventAction(int a):pmtCollID(-1),apdCollID(-1),csiCollID(-1),verbose(0),pmtThreshold(1)
#else
                                    LcEventAction::LcEventAction(int a): pmtCollID(-1), apdCollID(-1), csiCollID(-1), photonCounterCollID(-1),
                                    verbose(0), pmtThreshold(1)
#endif /*PHOTON_COUNTER*/
{detType = a;}
LcEventAction::~LcEventAction(){;}

void LcEventAction::BeginOfEventAction(const G4Event* anEvent)
{
    G4int EventID=anEvent->GetEventID();
    //if(EventID % 300==0){  G4cout<< "Event "<<EventID<<G4endl;} //ADDED LATER
    G4SDManager* SDman = G4SDManager::GetSDMpointer();   
    if ((detType == 1 || detType == 3 || detType == 5 || detType == 7) && pmtCollID < 0)     
        pmtCollID = SDman->GetCollectionID("PMTCollection");
    else if ((detType == 2 || detType == 4) && apdCollID < 0)
        apdCollID = SDman->GetCollectionID("APDCollection");

    if(csiCollID < 0)
    {
        csiCollID=SDman->GetCollectionID("CsICollection");
    } // if(csiCollID<0)
#ifdef PHOTON_COUNTER
    if (photonCounterCollID < 0)
    {
        photonCounterCollID = SDman->GetCollectionID("photonCounterCollection");
    }
#endif /*PHOTON_COUNTER*/
    G4cout<<"**************Event "<<EventID<<"********************\n";
}
void LcEventAction::EndOfEventAction(const G4Event* anEvent)
{
    //G4int i,j;
    //G4VTrajectory* theTrajectory; 
    G4String pName;
    //G4int nr_entries; 
    //G4VTrajectoryPoint* theTrajPoint;
    G4ThreeVector xyz_1,xyz_2;
    //G4double TrackL;
    //G4double DeltaL;
    //G4int TrackID;
    G4double eventID=0.0;

    //     G4int npd;
    LcEventStruct_t HitEvent;
#ifdef TRACES
    (HitEvent.distanceToOrigin).clear();
    (HitEvent.creatorProcessID).clear();
    (HitEvent.creatorOfCreatorProcessID).clear();
    (HitEvent.reflections).clear();
    (HitEvent.rx).clear();
    (HitEvent.ry).clear();
    (HitEvent.rz).clear();
    (HitEvent.processID).clear();
    (HitEvent.parentGammaTrackID).clear();
    (HitEvent.gx).clear();
    (HitEvent.gy).clear();
    (HitEvent.gz).clear();
    (HitEvent.parentGammaProcessID).clear();
#endif /*TRACES*/

    G4HCofThisEvent* HCE = anEvent->GetHCofThisEvent();
    // Get number of created optical photons:
    //G4int opPhotonCounter = 0;
    //std::vector<G4int> vScintTrackID;
    //LcCsIHitsCollection* hcCsI = (LcCsIHitsCollection*)HCE->GetHC(csiCollID);
    //if (hcCsI)
    //{
    //    G4int hitsCsI = hcCsI->entries();
    //    for (G4int i = 0; i < hitsCsI; i++)
    //    {
    //        LcCsIHit* scintHit = (LcCsIHit*)(*hcCsI)[i];
    //        if (scintHit->CheckScintillation())
    //        {
    //            G4bool idOK = true;
    //            G4int scintTrackID = scintHit->GetTrackID();
    //            G4int scintTrackIDSize = (G4int)vScintTrackID.size();
    //            if (scintTrackIDSize > 0)
    //            {
    //                for (G4int j = 0; j < scintTrackIDSize; j++)
    //                {
    //                    if (scintTrackID == vScintTrackID[j])
    //                    {
    //                        idOK = false;
    //                        break;
    //                    } // if (scintTrackID == vScintTrackID[j])
    //                } // for (G4int j = 0; j < scintTrackIDSize; j++)
    //            } // if (scintTrackIDSize) > 0)
    //            if (idOK)
    //            {
    //                opPhotonCounter++;
    //            } // if (idOK)
    //        } // if (((*hcCsI)[i])->CheckScintillation())
    //    } // for (G4int i = 0; i < hitsCsI; i++)
    //} // if (hcCsI)
    //vScintTrackID.clear();
    //G4cout << "Optical photons created by scintillation: " << opPhotonCounter << G4endl;

    LcPMTHitsCollection* PHC = 0;  
    LcAPDHitsCollection* AHC = 0;
#ifdef PHOTON_COUNTER
    LcPMTHitsCollection* phcHC = 0;
#endif /*PHOTON_COUNTER*/
    // LcCsIHitsCollection* CHC = 0;

    //Get the hit collections
    if(HCE)
    { 
        if((detType==1 || detType==3 || detType == 5 || detType == 7) && pmtCollID>=0)
            PHC = (LcPMTHitsCollection*)(HCE->GetHC(pmtCollID)); 
        else if((detType==2|| detType==4) && apdCollID>=0)
            AHC = (LcAPDHitsCollection*)(HCE->GetHC(apdCollID)); 
        //   if(csiCollID>=0)CHC = (LcCsIHitsCollection*)(HCE->GetHC(csiCollID)); 
#ifdef PHOTON_COUNTER
        if (photonCounterCollID >= 0)
        {
            phcHC = (LcPMTHitsCollection*)HCE->GetHC(photonCounterCollID);
        }
#endif /*PHOTON_COUNTER*/
    }
    //we zero the variables before before @ event

    eventID= (G4double) (anEvent->GetEventID());
    //HitEvent.eventID=eventID;
    G4int nr_hits = 0;
    //G4int pmt_Hits = 0;
    HitEvent.npd=0;
    HitEvent.eventID=-1;
    //G4int	csi_Hits = 0;
#ifdef TRACES
    primTraceCollection->FindAndSetOrigin();
    LcUserTraceCollection* gammTraceCollection = new LcUserTraceCollection();
#endif /*TRACES*/
    if(PHC)
    {
        nr_hits =PHC->entries();
        if (nr_hits>59999)
        {
            G4cout << ">>>> Warning! nr_hits is out of range! nr_hits=" << nr_hits << G4endl;
            nr_hits=59999;
        }
        G4cout<<"PMT hits:"<<nr_hits<<G4endl;
#ifdef PHOTON_COUNTER
        if (phcHC)
        {
            G4cout << "Photons leaked: " << phcHC->entries() << G4endl;
        }
#endif /*PHOTON_COUNTER*/
        HitEvent.eventID=(int)anEvent->GetEventID();
#ifdef TRACES
        HitEvent.originX=(primTraceCollection->GetOrigin()).x();
        HitEvent.originY=(primTraceCollection->GetOrigin()).y();
        HitEvent.originZ=(primTraceCollection->GetOrigin()).z();
        int phot=0;
        int compt=0;
        for (int i=0;i<primTraceCollection->GetTraceByIndex(0)->GetEntries();i++)
        {
            if (primTraceCollection->GetTraceByIndex(0)->GetProcessName(i)=="phot")
            {
                phot++;
            }
            if (primTraceCollection->GetTraceByIndex(0)->GetProcessName(i)=="compt")
            {
                compt++;
            }
        }
        HitEvent.primaryPhot=phot;
        HitEvent.primaryCompt=compt;
#endif /*TRACES*/
        HitEvent.npd=nr_hits;
        for (G4int i=0;i<nr_hits;i++)
        {
            LcPMTHit* aHit = (*PHC)[i];
            HitEvent.wpd[i] = aHit->GetWavelength();
            HitEvent.lpd[i] = aHit->GetTrackLength();
#ifdef TRACES
            G4cout
                <<"index="<<i
                <<"; reflections.size()="<<(HitEvent.reflections).size()
                <<"; aHit->GetTrackID()="<<aHit->GetTrackID()
                <<"; trace->Entries()="<<primTraceCollection->GetTrace(aHit->GetTrackID())->GetEntries()
                <<G4endl;
            (HitEvent.reflections).push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetEntries());
            G4ThreeVector trackOrigin=primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(0);
            (HitEvent.distanceToOrigin).push_back(
                    trackOrigin.deltaR(primTraceCollection->GetOrigin()));
            (HitEvent.creatorProcessID).push_back(
                    primTraceCollection->GetTrace(aHit->GetTrackID())->GetCreatorProcessID());
            //G4cout << "check1" << G4endl;
#ifndef NOREFLECTOR
            LcUserTrace* parentTrace=primTraceCollection->GetTrace(
                    primTraceCollection->GetTrace(aHit->GetTrackID())->GetParentTrackID());
            (HitEvent.creatorOfCreatorProcessID).push_back(parentTrace->GetCreatorProcessID());
#else
            (HitEvent.creatorOfCreatorProcessID).push_back(-1);
#endif /*NOREFLECTOR*/
            //G4cout << "check2" << G4endl;
            std::vector<double> xContainer;
            std::vector<double> yContainer;
            std::vector<double> zContainer;
            std::vector<double> processIdContainer;
            for (int j=0;j<(HitEvent.reflections)[i];j++)
            {
                xContainer.push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(j).x());
                yContainer.push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(j).y());
                zContainer.push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(j).z());
                processIdContainer.push_back(
                        (double)primTraceCollection->GetTrace(aHit->GetTrackID())->GetProcessID(j));
                //if (primTraceCollection->GetTrace(aHit->GetTrackID())->GetProcessID(j) == 20)
#ifdef TRACES
#ifdef NOREFLECTOR
                if (primTraceCollection->GetTrace(aHit->GetTrackID())->GetProcessID(j) == 
                        G4OpBoundaryProcessStatus::Detection + 10)
                {// detection
                    detectedCounter++;
                }
#endif /*NOREFLECTOR*/
#endif /*TRACES*/
            }
            (HitEvent.rx).push_back(xContainer);
            (HitEvent.ry).push_back(yContainer);
            (HitEvent.rz).push_back(zContainer);
            (HitEvent.processID).push_back(processIdContainer);

#ifndef NOREFLECTOR
            LcUserTrace* gammaParentTrace=primTraceCollection->GetTrace(aHit->GetTrackID());
            while (gammaParentTrace->GetParticleName()!="gamma")
            {
                gammaParentTrace=primTraceCollection->GetTrace(gammaParentTrace->GetParentTrackID());
            }
            gammTraceCollection->AddTrace(gammaParentTrace);
#endif /*NOREFLECTOR*/
#endif /*TRACES*/
        }
#ifdef TRACES
#ifdef NOREFLECTOR
        G4cout << "Photons shot: " << HitEvent.eventID + 1 << "; detected: " << detectedCounter << G4endl;
#endif /*NOREFLECTOR*/
#endif /*TRACES*/
    }
    if(AHC)
    { 
        nr_hits =AHC->entries();
        HitEvent.eventID=(int)anEvent->GetEventID();
#ifdef TRACES
        HitEvent.originX=(primTraceCollection->GetOrigin()).x();
        HitEvent.originY=(primTraceCollection->GetOrigin()).y();
        HitEvent.originZ=(primTraceCollection->GetOrigin()).z();
        int phot=0;
        int compt=0;
        for (int i=0;i<primTraceCollection->GetTraceByIndex(0)->GetEntries();i++)
        {
            if (primTraceCollection->GetTraceByIndex(0)->GetProcessName(i)=="phot")
            {
                phot++;
            }
            if (primTraceCollection->GetTraceByIndex(0)->GetProcessName(i)=="compt")
            {
                compt++;
            }
        }
        HitEvent.primaryPhot=phot;
        HitEvent.primaryCompt=compt;
#endif /*TRACES*/
        HitEvent.npd=nr_hits;
        G4cout<<"APD hits:"<<nr_hits<<G4endl;
        for(G4int i=0;i<nr_hits;i++){
            LcAPDHit* aHit = (*AHC)[i];
            HitEvent.wpd[i] = aHit->GetWavelength();
            HitEvent.lpd[i] = aHit->GetTrackLength();
#ifdef TRACES
            (HitEvent.reflections).push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetEntries());
            G4ThreeVector trackOrigin=primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(0);
            (HitEvent.distanceToOrigin).push_back(
                    trackOrigin.deltaR(primTraceCollection->GetOrigin()));
            (HitEvent.creatorProcessID).push_back(
                    primTraceCollection->GetTrace(aHit->GetTrackID())->GetCreatorProcessID());
            LcUserTrace* parentTrace=primTraceCollection->GetTrace(
                    primTraceCollection->GetTrace(aHit->GetTrackID())->GetParentTrackID());
            (HitEvent.creatorOfCreatorProcessID).push_back(parentTrace->GetCreatorProcessID());
            std::vector<double> xContainer;
            std::vector<double> yContainer;
            std::vector<double> zContainer;
            std::vector<double> processIdContainer;
            for (int j=0;j<(HitEvent.reflections)[i];j++)
            {
                xContainer.push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(j).x());
                yContainer.push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(j).y());
                zContainer.push_back(primTraceCollection->GetTrace(aHit->GetTrackID())->GetPoint(j).z());
                processIdContainer.push_back(
                        (double)primTraceCollection->GetTrace(aHit->GetTrackID())->GetProcessID(j));
            }
            (HitEvent.rx).push_back(xContainer);
            (HitEvent.ry).push_back(yContainer);
            (HitEvent.rz).push_back(zContainer);
            (HitEvent.processID).push_back(processIdContainer);

            LcUserTrace* gammaParentTrace=primTraceCollection->GetTrace(aHit->GetTrackID());
            while (gammaParentTrace->GetParticleName()!="gamma")
            {
                gammaParentTrace=primTraceCollection->GetTrace(gammaParentTrace->GetParentTrackID());
            }
            gammTraceCollection->AddTrace(gammaParentTrace);
#endif /*TRACES*/
        }        
    }
#ifdef TRACES
    for (int i=0;i<gammTraceCollection->GetEntries();i++)
    {
        LcUserTrace* theGammaTrace=gammTraceCollection->GetTraceByIndex(i);
        (HitEvent.parentGammaTrackID).push_back(theGammaTrace->GetTrackID());
        std::vector<double> xgContainer;
        std::vector<double> ygContainer;
        std::vector<double> zgContainer;
        std::vector<double> pGammaProcessIdContainer;
        for (int j=0;j<theGammaTrace->GetEntries();j++)
        {
            xgContainer.push_back(theGammaTrace->GetPoint(j).x());
            ygContainer.push_back(theGammaTrace->GetPoint(j).y());
            zgContainer.push_back(theGammaTrace->GetPoint(j).z());
            pGammaProcessIdContainer.push_back((double)theGammaTrace->GetProcessID(j));
        }
        (HitEvent.gx).push_back(xgContainer);
        (HitEvent.gy).push_back(ygContainer);
        (HitEvent.gz).push_back(zgContainer);
        (HitEvent.parentGammaProcessID).push_back(pGammaProcessIdContainer);
    }
#endif /*TRACES*/
    if(PHC||AHC)
    {
        //G4cout<<"Detector hit; G4TrajectoryContainer::entries()="<<
        //    anEvent->GetTrajectoryContainer()->entries()<<G4endl;
        //for (int i=0;i<anEvent->GetTrajectoryContainer()->entries();i++)
        {
            /*          
                        G4TrajectoryContainer* theTrajectoryContainer=anEvent->GetTrajectoryContainer();
                        G4VTrajectory* theTrajectory=(*theTrajectoryContainer)[i];
                        G4cout<<"\tTrackID="<<theTrajectory->GetTrackID()<<G4endl;
                        */
            //G4cout<<"\tTrackID="<<(*(anEvent->GetTrajectoryContainer()))[i]->GetTrackID()<<G4endl;
        }
    }
    RootTreeHandler->pushEvent(HitEvent);
#ifdef TRACES
    int primTCEntries=primTraceCollection->GetEntries();
    //int photTCEntries=userTraceCollection->GetEntries();
    //primTraceCollection->Print();
    //userTraceCollection->Print();
    //G4cout<<"photTraceCollection::GetEntries()="<<userTraceCollection->GetEntries()<<G4endl;
    G4cout<<"primTraceCollection::GetEntries()="<<primTraceCollection->GetEntries()<<G4endl;
    G4cout << "event id:" << G4double (eventID) << G4endl;
    (HitEvent.distanceToOrigin).clear();
    (HitEvent.creatorProcessID).clear();
    (HitEvent.creatorOfCreatorProcessID).clear();
    (HitEvent.reflections).clear();
    (HitEvent.rx).clear();
    (HitEvent.ry).clear();
    (HitEvent.rz).clear();
    (HitEvent.processID).clear();
    //if(nr_hits)
    //{
    //    G4cout
    //        <<"check reflections:"<<(HitEvent.reflections)[0]
    //        <<G4endl;
    //}
    //userTraceCollection->Clear();
    primTraceCollection->Clear();
#endif /*TRACES*/
}



