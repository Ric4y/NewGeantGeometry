#ifndef LcTreeHandler_h
#define LcTreehandler_h 1

//---------------------
// class name :LcTreeHandler
// Description: Utility class to hold and manipulate root histograms and files

#include "globals.hh"
#include <vector>

#include "LcEvent.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "TApplication.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TObject.h"
#include <Riostream.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include <vector>

#include "LcVars.hh"

class LcTreeHandler
{
private:

    LcEventStruct_t LcEvent;

    TTree* LcTree;
    TFile* hfileROOT;

public:

    LcTreeHandler();
    ~LcTreeHandler();

    void book (int typeCr, int tapered, int matFrac, int matType, int lambda, int detType, int pos)
    {//the root file
        char filename[256];
        int crlength;
        if (typeCr == 1)
            crlength = 17;
        else if (typeCr==2)
            crlength = 18;
        else if (typeCr==3)
            crlength = 22;

        int matfrac;
        matfrac = matFrac*25;
        /*
           if (detType==1 || detType==3){ // PMT files
           if (tapered==0)
           sprintf( filename, 
           "/nfs/users/joochunpk/jp_csi_sim/pmt/cr%dcm_rect_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
           crlength, matfrac, matType, 
           lambda, detType, pos); // lambda test for rectangular prism crystal
           else
           sprintf( filename, 
           "/nfs/users/joochunpk/jp_csi_sim/pmt/cr%dcm_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
           crlength, matfrac, matType, 
           lambda, detType, pos); // lambda test for tapered crystal, no specific header
           }
           else{ // APD files
           if (tapered==0)
           sprintf( filename, 
           "/nfs/users/joochunpk/jp_csi_sim/apd/cr%dcm_rect_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
           crlength, matfrac, matType, 
           lambda, detType, pos); // lambda test for rectangular prism crystal
           else
           sprintf( filename, 
           "/nfs/users/joochunpk/jp_csi_sim/apd/cr%dcm_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
           crlength, matfrac, matType, 
           lambda, detType, pos); // lambda test for tapered crystal, no specific header
           }
        */
        //if (detType==1 || detType==3)
        if (detType==1)
        { // PMT files
            if (tapered==0)
                sprintf( filename, 
                        "pmt_cr%dcm_rect_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
                        crlength, matfrac, matType, 
                        lambda, detType, pos); // lambda test for rectangular prism crystal
            else
                sprintf( filename, 
                        "pmt_cr%dcm_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
                        crlength, matfrac, matType, 
                        lambda, detType, pos); // lambda test for tapered crystal, no specific header
        }
        //else
        else if (detType == 3 || detType == 7)
        { // APD files
            if (tapered==0)
                sprintf( filename, 
                        "apd_cr%dcm_rect_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
                        crlength, matfrac, matType, 
                        lambda, detType, pos); // lambda test for rectangular prism crystal
            else
                sprintf( filename, 
                        "apd_cr%dcm_mat%dpc_type%d_lamb%dcm_det%d_pos%d.root", 
                        crlength, matfrac, matType, 
                        lambda, detType, pos); // lambda test for tapered crystal, no specific header

        }

        //    G4String fileNameROOT = G4String( filename );
        //    hfileROOT = new TFile(fileNameROOT.c_str(),"RECREATE","ROOT file for Lc");
        //hfileROOT = new TFile(filename,"CREATE");
        hfileROOT = new TFile(filename,"RECREATE");

        LcTree = new TTree("LcTree","LcTree");
        G4cout<< "Entering Tree Booking"<<G4endl;
        LcTree->Branch("eventID",&LcEvent.eventID,"eventID/I");
#ifdef TRACES
        LcTree->Branch("originX",&LcEvent.originX,"originX/D");
        LcTree->Branch("originY",&LcEvent.originY,"originY/D");
        LcTree->Branch("originZ",&LcEvent.originZ,"originZ/D");
        LcTree->Branch("primaryPhot",&LcEvent.primaryPhot,"primaryPhot/I");
        LcTree->Branch("primaryCompt",&LcEvent.primaryCompt,"primaryCompt/I");
#endif /*TRACES*/
        LcTree->Branch("npp",&LcEvent.npp,"npp/I");
        LcTree->Branch("wpp",LcEvent.wpp,"wpp[npp]/D");

        LcTree->Branch("npd",&LcEvent.npd,"npd/I");
        LcTree->Branch("wpd",LcEvent.wpd,"wpd[npd]/D");
        LcTree->Branch("lpd",LcEvent.lpd,"lpd[npd]/D");
#ifdef TRACES
        LcTree->Branch("distanceToOrigin",&(LcEvent.distanceToOrigin));
        LcTree->Branch("creatorProcessID",&(LcEvent.creatorProcessID));
        LcTree->Branch("creatorOfCreatorProcessID",&(LcEvent.creatorOfCreatorProcessID));

        LcTree->Branch("reflections",&(LcEvent.reflections));
        LcTree->Branch("processID",&(LcEvent.processID));
        LcTree->Branch("rx",&(LcEvent.rx));
        LcTree->Branch("ry",&(LcEvent.ry));
        LcTree->Branch("rz",&(LcEvent.rz));
        
        LcTree->Branch("parentGammaTrackID",&(LcEvent.parentGammaTrackID));

        LcTree->Branch("gx",&(LcEvent.gx));
        LcTree->Branch("gy",&(LcEvent.gy));
        LcTree->Branch("gz",&(LcEvent.gz));
        
        LcTree->Branch("parentGammaProcessID",&(LcEvent.parentGammaProcessID));
#endif /*TRACES*/
    }

    //Save tree to file

    void clear();
    
    void SetNpp(G4int PhotonsCount){
        LcEvent.npp = PhotonsCount;
    }
    
    void SetWpp(G4double* Wavelengths){
        for(G4int i = 0; i < LcEvent.npp; i++){
            LcEvent.wpp[i] = Wavelengths[i];
            //      G4cout<<i<<" "<<Wavelengths[i]<<" "<<LcEvent.wpp[i]<<G4endl;
        }
    } 
    // void SetIpp(G4double* ParentIDs){
    //   for(G4int i = 0; i < LcEvent.npp; i++){
    //     LcEvent.ipp[i] = ParentIDs[i];
    //     //      G4cout<<i<<" "<<Wavelengths[i]<<" "<<LcEvent.wpp[i]<<G4endl;
    //   }
    // }
    
    //Fill Event and put to tree
    void pushEvent(LcEventStruct_t Event)
    {
#ifdef TRACES
        LcEvent.eventID=-1;
        LcEvent.originX=0.0;
        LcEvent.originY=0.0;
        LcEvent.originZ=0.0;
        LcEvent.primaryPhot=-1;
        LcEvent.primaryCompt=-1;
        LcEvent.npd=-1;
        (LcEvent.distanceToOrigin).clear();
        (LcEvent.creatorProcessID).clear();
        (LcEvent.creatorOfCreatorProcessID).clear();
        (LcEvent.reflections).clear();
        (LcEvent.rx).clear();
        (LcEvent.ry).clear();
        (LcEvent.rz).clear();
        (LcEvent.processID).clear();
        (LcEvent.parentGammaTrackID).clear();
        (LcEvent.gx).clear();
        (LcEvent.gy).clear();
        (LcEvent.gz).clear();
        (LcEvent.parentGammaProcessID).clear();
#endif /*TRACES*/
#ifdef TRACES
        LcEvent.originX=Event.originX;
        LcEvent.originY=Event.originY;
        LcEvent.originZ=Event.originZ;
        LcEvent.primaryPhot=Event.primaryPhot;
        LcEvent.primaryCompt=Event.primaryCompt;
#endif /*TRACES*/
        LcEvent.eventID=Event.eventID;
        LcEvent.npd=Event.npd;
        
        for(G4int i = 0; i < LcEvent.npd; i++)
        {
            LcEvent.wpd[i] = Event.wpd[i];
            LcEvent.lpd[i] = Event.lpd[i];
            /*
               LcEvent.reflections[i]=Event.reflections[i];
               for (int j=0;i<LcEvent.reflections[i];i++)
               {
               LcEvent.rx[i][j]=Event.rx[i][j];
               LcEvent.ry[i][j]=Event.ry[i][j];
               LcEvent.rz[i][j]=Event.rz[i][j];
               }
            */
#ifdef TRACES
            (LcEvent.distanceToOrigin).push_back((Event.distanceToOrigin[i]));
            (LcEvent.creatorProcessID).push_back((Event.creatorProcessID[i]));
            (LcEvent.creatorOfCreatorProcessID).push_back((Event.creatorOfCreatorProcessID[i]));
            (LcEvent.reflections).push_back((Event.reflections[i]));
            std::vector<double> xContainer;
            std::vector<double> yContainer;
            std::vector<double> zContainer;
            std::vector<double> processIdContainer;
            /*
               G4cout<<"\t\tEvent.reflections["<<i<<"]="<<Event.reflections[i]<<
               ";(Event.ry[i]).size()="<<(Event.ry[i]).size()<<
               ";(Event.rz[i]).size()="<<(Event.rz[i]).size()<<G4endl;
               */
            for(int j=0;j<Event.reflections[i];j++)
            {
                xContainer.push_back(Event.rx[i][j]);
                yContainer.push_back(Event.ry[i][j]);
                zContainer.push_back(Event.rz[i][j]);
                processIdContainer.push_back(Event.processID[i][j]);
            }
            (LcEvent.rx).push_back(xContainer);
            (LcEvent.ry).push_back(yContainer);
            (LcEvent.rz).push_back(zContainer);
            (LcEvent.processID).push_back(processIdContainer);
#endif /*TRACES*/
        }
#ifdef TRACES
        for (int i=0;i<(int)(Event.parentGammaTrackID).size();i++)
        {
            (LcEvent.parentGammaTrackID).push_back(Event.parentGammaTrackID[i]);
            std::vector<double> xgContainer;
            std::vector<double> ygContainer;
            std::vector<double> zgContainer;
            std::vector<double> pGammaProcessIdContainer;
            for (int j=0;j<(int)(Event.gx[i]).size();j++)
            {
                xgContainer.push_back(Event.gx[i][j]);
                ygContainer.push_back(Event.gy[i][j]);
                zgContainer.push_back(Event.gz[i][j]);
                pGammaProcessIdContainer.push_back(Event.parentGammaProcessID[i][j]);
            }
            (LcEvent.gx).push_back(xgContainer);
            (LcEvent.gy).push_back(ygContainer);
            (LcEvent.gz).push_back(zgContainer);
            (LcEvent.parentGammaProcessID).push_back(pGammaProcessIdContainer);
        }
#endif /*TRACES*/
        // LcEvent.rpd=Event.rpd;
        LcTree->Fill();
    }
    
    void close() 
    {  
        LcTree->Write();
        hfileROOT->Close();
    }
};
#endif /*LcTreeHandler_h*/
