//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include <math.h>
#include <vector>
#include <string>

#include "LcDetectorConstruction.hh"
#include "LcPMTSD.hh"
#include "LcCsISD.hh"
#include "LcPhysicsList.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"
#include "G4ios.hh"
#include "G4String.hh"
#include "G4ReflectionFactory.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4MultiUnion.hh"
#include "G4Polyhedra.hh"

#include "G4Para.hh"

#include "LcVars.hh"

#ifdef PHOTON_COUNTER
 #include "LcPhotonCounterSD.hh"
#endif /*PHOTON_COUNTER*/

using std::vector;
using std::string;

void trapezeCsI(G4double *data, G4double *output);
void trapezeLeft(G4double *dataCsI, G4double *output, G4double ESRThickness, G4double cut);
void trapezeTop(G4double *dataCsI, G4double *output, G4double ESRThickness, G4double cut);
G4int arg;
/* 
   This class zorks using a code giving by the value of arg. It is a 3 digit number.
   The first number corresponds to geometry of the crystal:
   config 1 = 22 cm length cristal 
   config 2 = 18 cm length cristal
   config 3 = 17 cm length cristal

   The second number corresponds to the fraction of treating surface:				
   1 = 1/4 of the length cristal 
   2 = 1/2 of the length cristal
   3 = 3/4 of the length cristal

   The third number corresponds to the surfaces treated:				
   1 = no treatment
   2 = top treated
   3 = left treated
   4 = top bottom treated
   5 = top left treated 
   6 = left right treated
   7 = top bottom left treated
   8 = top left right treated
   9 = 4 surfaces treated  
   */
//--------------------------------------------------------------------------
LcPMTSD* LcDetectorConstruction::pmt_SD;//SD
G4SDManager* SDman = G4SDManager::GetSDMpointer();// is G4manager class for sensitive detectors
#ifdef NOREFLECTOR
 #ifdef TRACES
LcPMTSD* LcDetectorConstruction::csiAbs_SD;//SD
 #endif /*TRACES*/
#endif /*NOREFLECTOR*/
LcCsISD* LcDetectorConstruction::CsI_SD;//SD
#ifdef PHOTON_COUNTER
LcPhotonCounterSD* LcDetectorConstruction::photonCounterSD;
#endif /*PHOTON_COUNTER*/

//LcDetectorConstruction::LcDetectorConstruction(
//        int inCrType, int inTapered, int inMatFrac, int inMatType, int inLambda, int inDetType, 
//        int inFracTop, int inFracRight, int inFracBottom, int inFracLeft)
LcDetectorConstruction::LcDetectorConstruction(
        G4int inCrType, G4int inTapered, G4int inMatFrac, G4int inMatType, G4double inLambda, G4int inDetType, 
        G4int inFracTop, G4int inFracRight, G4int inFracBottom, G4int inFracLeft,
        G4bool modelSwitch,
        G4double rindexOpticalGlue, G4double rindexEpoxyAPD,
        vector<double>* vEnergy, vector<double>* vLambda)
{
    crType = inCrType;
    tapered = inTapered;
    matFrac = inMatFrac;
    matType = inMatType;
    lambda = inLambda;
    detType = inDetType;
    fracTop = inFracTop;
    fracRight = inFracRight;
    fracBottom = inFracBottom;
    fracLeft = inFracLeft;

    fModelSwitch = modelSwitch;

    fRindexOpticalGlue = rindexOpticalGlue;
    fRindexEpoxyAPD = rindexEpoxyAPD;

    fEnergy = new vector<double>();
    fLambda = new vector<double>();
    fEnergy = vEnergy;
    fLambda = vLambda;
}

LcDetectorConstruction::~LcDetectorConstruction(){;}

G4VPhysicalVolume* LcDetectorConstruction::Construct(){
    //G4cout << ">>>>>>>>>>> Input lambda:\n";
    //for (int i = 0; i < (int)fEnergy->size(); i++)
    //{
    //    G4cout << fEnergy->at(i) << " eV; " << fLambda->at(i) << " cm\n";
    //}
    //--------------------------------------------------------------------------
    ///	                MATERIALS
    //--------------------------------------------------------------------------
    //G4double a, z, density;
    G4double density;
    G4int nel;
    G4NistManager* man = G4NistManager::Instance();

    G4Element* elH = man->FindOrBuildElement(1);
    G4Element* elC = man->FindOrBuildElement(6);
    G4Element* elO = man->FindOrBuildElement(8);
    G4Element* elSi = man->FindOrBuildElement(14);
    
    //------------Air
    G4Material* Air = man->FindOrBuildMaterial("G4_AIR");

    //------------Vacuum
    G4Material* galVacuum = man->FindOrBuildMaterial("G4_Galactic");
    
    // -----------CsI (Scintillator)
    G4Material* CsI = man->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    
    // -----------ESR as Mylar(Reflector) --Oxygen above to be used
    G4Material* Mylar = man->FindOrBuildMaterial("G4_MYLAR");
    
    //------------PlexiGlass of the PMT
    G4Material* Glass = man->FindOrBuildMaterial("G4_PLEXIGLASS");
    
    //------------Ceramic casing of APD
    G4Material* Ceramic = man->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");

    //------------Silicone rubber
    G4Material* siliconeRubber = new G4Material("siliconeRubber", density = 1.02*g/cm3, nel = 4);
    siliconeRubber->AddElement(elSi, 2);
    siliconeRubber->AddElement(elO,  2);
    siliconeRubber->AddElement(elC,  4);
    siliconeRubber->AddElement(elH, 12);

    //------------Silicon APD
    G4Material* silicon = man->FindOrBuildMaterial("G4_Si");
    
    //--------------------------------------------------------------------------
    // ------------ Generate & Add Material Properties Table ------------
    //--------------------------------------------------------------------------
    const G4int nEntries = 1;
    G4double PhotonEnergy[nEntries] =
    {1.778*eV};
    G4double csiFlashCentroid[nEntries] = {2.25426*eV};// 550.0 nm
    // CsI
    //G4double RefractiveIndex1[nEntries] =
    //{1.8};
    //G4double Absorption1[nEntries] =
    //{(G4double)lambda*cm};
    const G4int nEntriesCsI = 40;
    G4double ScintilFastSpectrum[nEntriesCsI] = {0.014, 0.078, 0.146, 0.216, 0.296,
        0.392, 0.490, 0.588, 0.678, 0.759, 0.844, 0.918, 0.968, 0.978, 0.951,
        0.867, 0.816, 0.726, 0.607, 0.484, 0.423, 0.365, 0.316, 0.273, 0.232,
        0.202, 0.180, 0.161, 0.142, 0.132, 0.121, 0.104, 0.088, 0.072, 0.063,
        0.056, 0.048, 0.041, 0.034, 0.028}; // from Saint-Gobain datasheet

    G4double PhotonEnergySpectrum[nEntriesCsI];
    for(G4int i = 0; i < nEntriesCsI; i++){
        PhotonEnergySpectrum[i] = (1.675+0.0478687*i)*eV; // even-spaced energy
    }
    int nEntriesCsIlambda = (int)fLambda->size();
    G4double* PhotonEnergySpectrumLambda = &fEnergy->at(0);
    G4double RefractiveIndexSpectrum[nEntriesCsI] = {1.770, 1.772, 1.774, 1.775, 1.777,
        1.779, 1.781, 1.783, 1.785, 1.787, 1.790, 1.792, 1.794, 1.797, 1.799,
        1.802, 1.804, 1.807, 1.810, 1.813, 1.816, 1.819, 1.822, 1.826, 1.829,
        1.833, 1.836, 1.840, 1.844, 1.848, 1.852, 1.856, 1.860, 1.865, 1.869,
        1.874, 1.879, 1.884, 1.889, 1.895}; // Li 1976, in nm
    
    //G4double AbsorptionSpectrum[nEntriesCsI] = {44.446*cm,43.940*cm,43.462*cm,42.956*cm,42.458*cm,
    //    41.992*cm,41.541*cm,41.069*cm,40.578*cm,40.098*cm,39.596*cm,39.092*cm,38.581*cm,38.074*cm,37.556*cm,
    //    37.045*cm,36.527*cm,35.984*cm,34.434*cm,34.937*cm,43.483*cm,33.966*cm,33.343*cm,32.370*cm,31.376*cm,
    //    30.873*cm,30.608*cm,30.019*cm,29.241*cm,28.483*cm,27.837*cm,27.144*cm,26.442*cm,25.774*cm,25.002*cm,
    //    24.178*cm,23.325*cm,22.512*cm,21.310*cm,18.725*cm}; // correct order
    //G4double AbsorptionSpectrum[nEntriesCsI] = {
    //    44.452*cm, 43.446*cm, 42.467*cm, 41.623*cm, 40.776*cm, 
    //    39.975*cm, 39.176*cm, 38.416*cm, 37.684*cm, 36.975*cm, 
    //    36.295*cm, 35.613*cm, 35.024*cm, 34.467*cm, 33.894*cm, 
    //    33.193*cm, 32.151*cm, 31.254*cm, 30.836*cm, 30.595*cm, 
    //    30.084*cm, 29.452*cm, 28.805*cm, 28.257*cm, 27.772*cm, 
    //    27.270*cm, 26.776*cm, 26.306*cm, 25.863*cm, 25.369*cm, 
    //    24.885*cm, 24.388*cm, 23.894*cm, 23.405*cm, 22.972*cm, 
    //    22.517*cm, 21.971*cm, 21.272*cm, 20.291*cm, 18.746*cm
    //};// correct order, correct spacing, measured sample
#ifndef SIMPLE_OPTICS
    //G4double AbsorptionSpectrum[nEntriesCsI] = {
    //    42.375*cm, 41.324*cm, 40.338*cm, 39.541*cm, 38.664*cm, 
    //    37.836*cm, 37.036*cm, 36.278*cm, 35.556*cm, 34.856*cm, 
    //    34.179*cm, 33.507*cm, 32.936*cm, 32.379*cm, 31.756*cm, 
    //    31.001*cm, 29.875*cm, 29.069*cm, 28.643*cm, 28.307*cm, 
    //    27.691*cm, 27.175*cm, 26.717*cm, 26.237*cm, 25.791*cm, 
    //    25.334*cm, 24.891*cm, 24.450*cm, 24.007*cm, 23.528*cm, 
    //    23.031*cm, 22.547*cm, 22.096*cm, 21.663*cm, 21.284*cm, 
    //    20.831*cm, 20.295*cm, 19.584*cm, 18.597*cm, 17.054*cm
    //};// correct order, correct spacing, measured crystal
    G4double* AbsorptionSpectrum = &fLambda->at(0);
#else
    G4double AbsorptionSpectrum[nEntriesCsI] = {lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,
        lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,
        lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,
        lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm,
        lambda*cm,lambda*cm,lambda*cm,lambda*cm,lambda*cm}; // all identical for now
#endif /*SIMPLE_OPTICS*/

    G4MaterialPropertiesTable* CsITable = new G4MaterialPropertiesTable();
    //CsITable->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex1,nEntries);
    CsITable->AddProperty("RINDEX", PhotonEnergySpectrum, RefractiveIndexSpectrum, nEntriesCsI);
    //CsITable->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption1,     nEntries);
    G4cout << ">>>>>>>>>>> Input lambda:\n";
    for (int i = 0; i < nEntriesCsIlambda; i++)
    {
        PhotonEnergySpectrum[i] *= eV;
        AbsorptionSpectrum[i] *= cm;
        //G4cout << PhotonEnergySpectrumLambda[i] << " eV; " << AbsorptionSpectrum[i] << " cm\n";
    }
    CsITable->AddProperty("ABSLENGTH", PhotonEnergySpectrumLambda, AbsorptionSpectrum, nEntriesCsIlambda);
    //CsITable->AddProperty("FASTCOMPONENT",PhotonEnergy, ScintilFast,     nEntries);
    //CsITable->AddProperty("SLOWCOMPONENT",PhotonEnergy, ScintilSlow,     nEntries);
    CsITable->AddProperty("FASTCOMPONENT", PhotonEnergySpectrum, ScintilFastSpectrum, nEntriesCsI);
    CsITable->AddProperty("SLOWCOMPONENT", PhotonEnergySpectrum, ScintilFastSpectrum, nEntriesCsI);
    //CsITable->AddConstProperty("SCINTILLATIONYIELD", 54000.0/MeV);
    CsITable->AddConstProperty("SCINTILLATIONYIELD", 3.5748e4/(0.662*MeV));
    //CsITable->AddConstProperty("SCINTILLATIONYIELD", 1.0e0/(0.662*MeV));
    CsITable->AddConstProperty("RESOLUTIONSCALE",0.01);
    CsITable->AddConstProperty("YIELDRATIO",1.00); 
    //CsITable->AddConstProperty("FASTTIMECONSTANT", 6.*ns);
    //CsITable->AddConstProperty("SLOWTIMECONSTANT",28.*ns);
    CsITable->AddConstProperty("FASTTIMECONSTANT", 0.68*us);
    CsITable->AddConstProperty("SLOWTIMECONSTANT", 3.34*us);
    CsI->SetMaterialPropertiesTable(CsITable);

    // Air
    G4double RefractiveIndex2[nEntries] =
    {1.00};
    G4MaterialPropertiesTable* AirTable = new G4MaterialPropertiesTable();
    AirTable->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex2, nEntries);
    Air->SetMaterialPropertiesTable(AirTable);

    // Vacuum
    G4double refractiveIndexVacuum[nEntries] = {1.0};
    G4MaterialPropertiesTable* galVacuumTable = new G4MaterialPropertiesTable();
    galVacuumTable->AddProperty("RINDEX", csiFlashCentroid, refractiveIndexVacuum, nEntries);
    galVacuum->SetMaterialPropertiesTable(galVacuumTable);
    
    // Mylar
    G4double RefractiveIndexESR[nEntries] = {1.65};
    G4MaterialPropertiesTable* MylarTable = new G4MaterialPropertiesTable();
    MylarTable->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexESR, nEntries);
    Mylar->SetMaterialPropertiesTable(MylarTable);
    
    // Plexiglass
    //G4double RefractiveIndexPlexi[nEntries] = {1.465};
    //G4double RefractiveIndexPlexi[nEntries] = {1.53};
    //G4double RefractiveIndexPlexi[nEntries] = {1.0};
    G4double RefractiveIndexPlexi[nEntries] = {fRindexEpoxyAPD};
    G4MaterialPropertiesTable* PlexiTable = new G4MaterialPropertiesTable();
    PlexiTable->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexPlexi, nEntries);
    //PlexiTable->AddProperty("RINDEX", PhotonEnergySpectrum, RefractiveIndexSpectrum, nEntriesCsI);
    Glass->SetMaterialPropertiesTable(PlexiTable);

    //Silicone rubber
    //double refractiveIndexSiliconeRubber[nEntries] = {1.406};
    //double refractiveIndexSiliconeRubber[nEntries] = {1.0};
    G4double refractiveIndexSiliconeRubber[nEntries] = {fRindexOpticalGlue};
    G4MaterialPropertiesTable* siliconeRubberTable = new G4MaterialPropertiesTable();
    siliconeRubberTable->AddProperty("RINDEX", PhotonEnergy, refractiveIndexSiliconeRubber, nEntries);
    //siliconeRubberTable->AddProperty("RINDEX", PhotonEnergySpectrum, RefractiveIndexSpectrum, nEntriesCsI);
    siliconeRubber->SetMaterialPropertiesTable(siliconeRubberTable);

    //---------------------------------------------------------
    //---------defining visualisation parameters to use--------
    //---------------------------------------------------------
    G4VisAttributes* BWire= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    BWire->SetForceWireframe(true);

    G4VisAttributes* BSolid= new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    BSolid->SetForceSolid(true);

    G4VisAttributes* YSolid= new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.5));
    YSolid->SetForceSolid(true);

    G4VisAttributes* RWire= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    RWire->SetForceWireframe(true);
    G4VisAttributes* RSolid= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    RSolid->SetForceSolid(true);

    G4VisAttributes* GWire= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    GWire->SetForceWireframe(true);

    G4VisAttributes* GSolid= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    GSolid->SetForceSolid(true);

    G4VisAttributes* RBWire= new G4VisAttributes(G4Colour(1.0,0.0,1.0));
    RBWire->SetForceWireframe(true);

    G4VisAttributes* Gr1Solid= new G4VisAttributes(G4Colour(0.781,0.781,0.781));
    Gr1Solid->SetForceSolid(true);

    G4VisAttributes* Gr2Solid= new G4VisAttributes(G4Colour(0.645,0.645,0.645));
    Gr2Solid->SetForceSolid(true);

    G4VisAttributes* Gr3Solid= new G4VisAttributes(G4Colour(0.915,0.915,0.915));
    Gr3Solid->SetForceSolid(true);

    G4VisAttributes* Gr4Solid= new G4VisAttributes(G4Colour(0.97,0.97,0.97));
    Gr4Solid->SetForceSolid(true);

    G4VisAttributes* Gr5Solid= new G4VisAttributes(G4Colour(0.84,0.84,0.84));
    Gr5Solid->SetForceSolid(true);

    //--------------------------------------------------------------------------
    ///	------------- Volumes --------------
    //--------------------------------------------------------------------------
    // The World volume
    G4double worldWidth  = 3.0*m;
    G4double worldHeight = 3.0*m;
    G4double worldLength = 3.0*m;
    G4Box* SldWorld= new G4Box("World", worldWidth/2.0, worldHeight/2.0, worldLength/2.0);
//#ifndef NOREFLECTOR
//    //G4Box* SldWorld= new G4Box("World", 0.5*m, 0.5*m, 0.5*m);//DO NOT USE!!!
//#else

    //Logical Volume
    G4LogicalVolume* worldLog = new G4LogicalVolume(SldWorld, Air, "World");

    //Physical Volume
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), worldLog, "World", 0, false, 0);







#ifdef PHOTON_COUNTER
    //G4Box* SldWorld= new G4Box("World",4.5*m,4.5*m,4.5*m);
    G4Box* sldOuterFrame = new G4Box(
            "sldOuterFrame", 0.95*worldWidth/2.0, 0.95*worldHeight/2.0, 0.95*worldLength/2.0);
    G4Box* sldInnerFrame = new G4Box(
            "sldInnerFrame", 0.90*worldWidth/2.0, 0.90*worldHeight/2.0, 0.90*worldLength/2.0);
    G4SubtractionSolid* sldPhotonCounter = new G4SubtractionSolid(
            "sldPhotonCounter", sldOuterFrame, sldInnerFrame);
#endif /*PHOTON_COUNTER*/
#ifdef WORLD_VACUUM
    G4LogicalVolume* logWorld= new G4LogicalVolume(SldWorld, galVacuum, "World", 0, 0, 0);
 #ifdef PHOTON_COUNTER
    G4LogicalVolume* logPhotonCounter = new G4LogicalVolume(
            sldPhotonCounter, galVacuum, "logPhotonCounter", 0, 0, 0);
 #endif /*PHOTON_COUNTER*/
#else
    G4LogicalVolume* logWorld= new G4LogicalVolume(SldWorld,Air,"World",0,0,0);
 #ifdef PHOTON_COUNTER
    G4LogicalVolume* logPhotonCounter = new G4LogicalVolume(
            sldPhotonCounter, Air, "logPhotonCounter", 0, 0, 0);
 #endif /*PHOTON_COUNTER*/
#endif /*WORLD_VACUUM*/
    G4VPhysicalVolume* PhyWorld= new G4PVPlacement(0,G4ThreeVector(),logWorld,"World",0,false,0);
#ifdef PHOTON_COUNTER
    G4VPhysicalVolume* phyPhotonCounter = new G4PVPlacement(
            0, G4ThreeVector(0.0, 0.0, 0.0), logPhotonCounter, "phyPhotonCounter", logWorld, false, 0);
    //G4VPhysicalVolume* PhyCsI1 = new G4PVPlacement(
    //        zRot,G4ThreeVector(-outputCsI[7],outputCsI[8],outputCsI[9]),logCsI1,"CsI",logWorld,false,0);
#endif /*PHOTON_COUNTER*/




   
#ifdef NEW_GEOMETRY
//////********************Using materials*********************/////////
    G4Material* Stilbene = man->FindOrBuildMaterial("G4_STILBENE");
    G4Material* Al = man->FindOrBuildMaterial("G4_Al");
    //G4Material* Gl = man->FindOrBuildMaterial("G4_GLASS_PLATE");
        //------------BC400
    G4Material* PlastBC400 = new G4Material("PlastBC400", density = 1.023*g/cm3, nel = 2);
    PlastBC400->AddElement(elH, 5);
    PlastBC400->AddElement(elC,  5);

    //Standart Tube
    G4double standInRadSize = 0.0*mm;
    G4double standOutRadSize = 43.5*mm;
    G4double standTubSizeZ = 57.0*mm;

    G4Tubs* solidStandTub = new G4Tubs("solidStandTub", standInRadSize, standOutRadSize, standTubSizeZ/2.0, 0.0, 2.0 * M_PI);

    //Solid for side shell
    G4double layer = 150.0*um;
    G4double InRadSizeSide = standOutRadSize;
    G4double OutRadSizeSide = standOutRadSize + 2.0 * layer;
    G4double ShellSizeZSide = standTubSizeZ;

    G4Tubs* solidShell = new G4Tubs("solidShell", InRadSizeSide, OutRadSizeSide, ShellSizeZSide/2.0, 0.0, 2.0 * M_PI);

    //Solid for bottom
    G4Tubs* solidBottom = new G4Tubs("solidBottom", standInRadSize, OutRadSizeSide, layer / 2.0, 0.0, 2.0 * M_PI);

    //Solid for glue
    G4double standGlueSizeZ = 20*um;

    G4Tubs* solidGlue = new G4Tubs("solidGlue", standInRadSize, standOutRadSize, standGlueSizeZ/2.0, 0.0, 2.0 * M_PI);

    //Optical properties of BC400
    const G4int numPlast = 1;
    G4double photEnergyPlast[numPlast] = {2.115*eV}; //from Saint Gobain for BC400
    G4double fastSpectrumPlast[numPlast] = {1.0};
    G4double PhotonEnergySpectrLambdaPlast[numPlast] = {423*nm}; ///from Saint Gobain for BC400
    G4double AbsorbtionSpectrumPlast[numPlast] = {1.6*m}; //from Saint Gobain for BC400
    G4double RefractIndSpectrumPlast[numPlast] = {1.58}; //from Saint Gobain for BC400

    G4MaterialPropertiesTable* PlastTable = new G4MaterialPropertiesTable();
    PlastTable->AddProperty("RINDEX", photEnergyPlast, RefractIndSpectrumPlast, numPlast);
    PlastTable->AddProperty("ABSLENGTH", PhotonEnergySpectrLambdaPlast, AbsorbtionSpectrumPlast, numPlast);
    PlastTable->AddProperty("FASTCOMPONENT", photEnergyPlast, fastSpectrumPlast, numPlast);
    PlastTable->AddProperty("SLOWCOMPONENT", photEnergyPlast, fastSpectrumPlast, numPlast);
    //StilbTable->AddConstProperty("SCINTILLATIONYIELD", 6720.0/(1.0*MeV));
    PlastTable->AddConstProperty("SCINTILLATIONYIELD", 2.225e3/(0.662*MeV));
    PlastTable->AddConstProperty("RESOLUTIONSCALE", 0.01);
    PlastTable->AddConstProperty("YIELDRATIO", 1.00);
    PlastTable->AddConstProperty("FASTTIMECONSTANT", 0.39*ns); //from Saint Gobain for BC400
    PlastTable->AddConstProperty("SLOWTIMECONSTANT", 2.4*ns);//from Saint Gobain for BC400
    PlastBC400->SetMaterialPropertiesTable(PlastTable);

    //Optical properties of Stilbene
    const G4int numStilb = 1;
    G4double photEnergyStilb[numStilb] = {1.9*eV}; //from Inradoptics, article: "Stilbene for Fast Neutron Detection"
    G4double fastSpectrumStilb[numStilb] = {1.0};
    G4double PhotonEnergySpectrLambdaStilb[numStilb] = {380*nm}; // photEnergyStilb/200
    G4double AbsorbtionSpectrumStilb[numStilb] = {1.5*m};
    G4double RefractIndSpectrumStilb[numStilb] = {1.626}; //from Leo

    G4MaterialPropertiesTable* StilbTable = new G4MaterialPropertiesTable();
    StilbTable->AddProperty("RINDEX", photEnergyStilb, RefractIndSpectrumStilb, numStilb);
    StilbTable->AddProperty("ABSLENGTH", PhotonEnergySpectrLambdaStilb, AbsorbtionSpectrumStilb, numStilb);
    StilbTable->AddProperty("FASTCOMPONENT", photEnergyStilb, fastSpectrumStilb, numStilb);
    StilbTable->AddProperty("SLOWCOMPONENT", photEnergyStilb, fastSpectrumStilb, numStilb);
    //StilbTable->AddConstProperty("SCINTILLATIONYIELD", 6720.0/(1.0*MeV));
    StilbTable->AddConstProperty("SCINTILLATIONYIELD", 4.449e3/(0.662*MeV));
    StilbTable->AddConstProperty("RESOLUTIONSCALE", 0.01);
    StilbTable->AddConstProperty("YIELDRATIO", 1.00);
    StilbTable->AddConstProperty("FASTTIMECONSTANT", 0.36*ns); //from Leo
    StilbTable->AddConstProperty("SLOWTIMECONSTANT", 4.5*ns);
    Stilbene->SetMaterialPropertiesTable(StilbTable);

//////***************first detector with stilbene********************////////
    
    //Logical Volume for Standart tube

    G4LogicalVolume* logicStandTub = new G4LogicalVolume(solidStandTub, Stilbene, "logicStandTub");

    //Logical Volume for glue

    G4LogicalVolume* logicGlue = new G4LogicalVolume(solidGlue, siliconeRubber, "logicGlue");

    //Logical Volume for side shell

    G4LogicalVolume* logicShell = new G4LogicalVolume(solidShell, Al, "logicShell");

    //Logical volume for bottom

    G4LogicalVolume* logicBottom = new G4LogicalVolume(solidBottom, Al, "logicBottom");

    //Physical Volume for Standart tube

    G4VPhysicalVolume* physStandTub = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicStandTub, "physTube", logWorld, false, 0, 0);

    //logicStandTub->SetVisAttributes(GSolid);

    //Physical Volume for shell side

    G4VPhysicalVolume* physShell = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicShell, "physShell", logWorld, false, 0, 0);

    //logicShell->SetVisAttributes(YSolid);

    //Physical Volume for shell side

    G4VPhysicalVolume* physBottom = new G4PVPlacement(0, G4ThreeVector(0, 0, - (standTubSizeZ + layer) / 2.0), logicBottom, "physBottom", logWorld, false, 0, 0);

    //logicBottom->SetVisAttributes(YSolid);


    //Creating properties of optical surface dielectric - metal
    const G4int number = 1;
    G4double OptPhoton[number] = {1.9*eV};
    //G4double sigma_alpha=0.1;
    G4double sigma_alpha_polishedDM = 0.077;
    G4double sigma_alpha_unpolishedDM = 0.162;
    //G4double OptRefractivityDM[number] = {1.0};
    //G4double SpecularLobe[num]    = {1};//refl. about facet normal //1
    //G4double SpecularSpike[num]   = {0};//refl. about avg suface normal //0
    G4double OptSpecularLobe[number]    = {0.01};//refl. about facet normal //1
    G4double OptSpecularSpike[number]   = {1 - OptSpecularLobe[number]};//refl. about avg suface normal //0
    G4double OptBackscatter[number]     = {0};//refl. in groove, //diffuse lobe constant
    G4double AbsorptionSpectrumDM[number] = {};
    G4double ScintilFastSpectrumDM[number] = {};

    //G4double Reflectivity[num] = {0.98};
    //G4double Efficiency[num]   = {0.02};
    G4double OptReflectivityDM[number] = {0.998};
    G4double OptEfficiencyDM[number]   = {1 - OptReflectivityDM[number]};
    //G4double Reflectivity[num] = {1.0};
    //G4double Efficiency[num]   = {0.0};

    G4MaterialPropertiesTable* PmtOpticsTableDM = new G4MaterialPropertiesTable();
    //PmtOpticsTableDM->AddProperty("RINDEXOPTDM", OptPhoton, OptRefractivityDM, number);
    PmtOpticsTableDM->AddProperty("REFLECTIVITY", OptPhoton, OptReflectivityDM, number);
    PmtOpticsTableDM->AddProperty("EFFICIENCY",   OptPhoton, OptEfficiencyDM, number);

    G4OpticalSurface* OpticsPmtSurfaceDM = new G4OpticalSurface("OpticsPmtSurfaceDM");
    OpticsPmtSurfaceDM->SetType(dielectric_metal);
    OpticsPmtSurfaceDM->SetFinish(polished);
    OpticsPmtSurfaceDM->SetModel(unified);
    OpticsPmtSurfaceDM->SetMaterialPropertiesTable(PmtOpticsTableDM);


        /// ******************** PMT surface******************
    const G4int numbr = 1;
    G4OpticalSurface* OpPmtSurfaceNew = new G4OpticalSurface("PmtSurfaceNew");
    OpPmtSurfaceNew->SetType(dielectric_metal);
    OpPmtSurfaceNew->SetFinish(polished);
    OpPmtSurfaceNew->SetModel(glisur);
    G4double Reflectivity_pmtNew[numbr] = {0.0};
    G4double Efficiency_pmtNew[numbr]   = {1.0};
    G4double RefractiveIndex_pmtNew[numbr] = {1.48};


    G4MaterialPropertiesTable *PmtOpTableNew = new G4MaterialPropertiesTable();
    PmtOpTableNew->AddProperty("REFLECTIVITY", OptPhoton, Reflectivity_pmtNew, numbr);
    PmtOpTableNew->AddProperty("EFFICIENCY", OptPhoton, Efficiency_pmtNew,   numbr);
    PmtOpTableNew->AddProperty("RINDEX", OptPhoton, RefractiveIndex_pmtNew, numbr);
    OpPmtSurfaceNew->SetMaterialPropertiesTable(PmtOpTableNew);


    //Creating properties of optical surface dielectric - dielectric
     const G4int numb = 1;
    G4double OptPhoton1[numb] = {1.9*eV};
    //G4double sigma_alpha=0.1;
    G4double sigma_alpha_polishedDD = 0.077;
    G4double sigma_alpha_unpolishedDD = 0.162;
    G4double OptRefractivityDD[numb] = {1.5};
    //G4double SpecularLobe[num]    = {1};//refl. about facet normal //1
    //G4double SpecularSpike[num]   = {0};//refl. about avg suface normal //0
    G4double OptSpecularLobeDD[numb]    = {0.5};//refl. about facet normal //1
    G4double OptSpecularSpikeDD[numb]   = {0.5};//refl. about avg suface normal //0
    G4double OptBackscatterDD[numb]     = {0};//refl. in groove, //diffuse lobe constant
    G4double ReflectivityDD[numb] = {0.98};
    G4double EfficiencyDD[numb]   = {0.02};
    //G4double OptReflectivityDD[number] = {0.0};
    //G4double OptEfficiencyDD[number]   = {1 - OptReflectivityDD[number]};
    //G4double Reflectivity[num] = {1.0};
    //G4double Efficiency[num]   = {0.0};

    G4MaterialPropertiesTable* PmtOpticsTableDD = new G4MaterialPropertiesTable();
    PmtOpticsTableDD->AddProperty("RINDEX", OptPhoton, OptRefractivityDD, numb);
    PmtOpticsTableDD->AddProperty("SPECULARLOBECONSTANT", OptPhoton1, OptSpecularLobeDD, numb);
    PmtOpticsTableDD->AddProperty("SPECULARSPIKECONSTANT", OptPhoton1, OptSpecularSpikeDD, numb);
    PmtOpticsTableDD->AddProperty("BACKSCATTERCONSTANT", OptPhoton1, OptBackscatterDD, numb);
    PmtOpticsTableDD->AddProperty("REFLECTIVITY", OptPhoton1, ReflectivityDD, numb);
    PmtOpticsTableDD->AddProperty("EFFICIENCY",   OptPhoton1, EfficiencyDD,   numb);

    G4OpticalSurface* OpticsPmtSurfaceDD = new G4OpticalSurface("OpticsPmtSurfaceDD");
    OpticsPmtSurfaceDD->SetType(dielectric_dielectric);
    OpticsPmtSurfaceDD->SetFinish(polished);
    OpticsPmtSurfaceDD->SetModel(unified);
    OpticsPmtSurfaceDD->SetMaterialPropertiesTable(PmtOpticsTableDD);


    //Solid for glass window of pmt
    G4double glassSizeZ = 100.0*um;

    G4Tubs* solidWinPmt = new G4Tubs("solidWinPmt", standInRadSize, standOutRadSize, glassSizeZ/2.0, 0.0, 2.0 * M_PI);

    //Solid for pmt
    G4double pmtSizeZ = 2.0*cm;

    G4Tubs* solidPmt = new G4Tubs("solidPmt", standInRadSize, standOutRadSize, pmtSizeZ/2.0, 0.0, 2.0 * M_PI);

    //Logical volume for glass window
    G4LogicalVolume* logicWinPmt = new G4LogicalVolume(solidWinPmt, Glass, "logicWinPmt");

    //Logical volume for pmt
    G4LogicalVolume* logicPmt = new G4LogicalVolume(solidPmt, Air, "logicWinPmt");

    //Physical volume for glue between pmt and its window
    G4VPhysicalVolume* PhysGluePmt = new G4PVPlacement(0, G4ThreeVector(0, 0, (standTubSizeZ + standGlueSizeZ)/ 2.0), logicGlue, "PhysGluePmt", logWorld, false, 0, 0);

    //Physical volume for glass window
    G4VPhysicalVolume* PhysWinPmt = new G4PVPlacement(0, G4ThreeVector(0, 0, (standTubSizeZ + glassSizeZ) / 2.0 + standGlueSizeZ), logicWinPmt, "PhysWinPmt", logWorld, false, 0, 0);

    //Physical volume for pmt
    G4VPhysicalVolume* PhysPmt = new G4PVPlacement(0, G4ThreeVector(0, 0, (standTubSizeZ + pmtSizeZ) / 2.0 + standGlueSizeZ + glassSizeZ), logicPmt, "PhysPmt", logWorld, false, 0, 0);

    logicPmt->SetVisAttributes(RSolid);

    logicWinPmt->SetVisAttributes(GSolid);

    logicGlue->SetVisAttributes(BSolid);
    //Optical properties between physical volumes

    G4LogicalBorderSurface* ShellScin = new G4LogicalBorderSurface("ShellScin", physShell, physStandTub, OpticsPmtSurfaceDM);

    G4LogicalBorderSurface* BottomScin = new G4LogicalBorderSurface("BottomScin", physBottom, physStandTub, OpticsPmtSurfaceDM);

    G4LogicalBorderSurface* GlueScin = new G4LogicalBorderSurface("GlueScin", PhysGluePmt, physStandTub, OpticsPmtSurfaceDD);

    G4LogicalBorderSurface* GlueGlass = new G4LogicalBorderSurface("GlueGlass", PhysGluePmt, PhysWinPmt, OpticsPmtSurfaceDD);

    G4LogicalBorderSurface* PmtGlass = new G4LogicalBorderSurface("PmtGlass", PhysWinPmt, PhysPmt, OpPmtSurfaceNew);
    
    
////**********NEW DETECTOR*********////////



//Solid for new detector, its main part of body is trapezoid
    G4double size = 52.0*mm;
    G4double trdSizeX = 75.0*mm;
    G4double trdSizeY1 = 2.0 * size;
    G4double trdSizeY2 = size;
    G4double trdSizeZ = size * sqrt(3.0)/2.0;


    G4double step = 5.0*cm;

    G4RotationMatrix* rotMatr = new G4RotationMatrix();
    rotMatr->rotateX(0.0);

    G4RotationMatrix rotMatr11 = G4RotationMatrix();
    rotMatr11.rotateX(0.0);
    G4ThreeVector position1 = G4ThreeVector(0.0, 0.0, 0.0);

    G4RotationMatrix rotMatr2 = G4RotationMatrix();
    rotMatr2.rotateX(0.0);
    G4ThreeVector position2 = G4ThreeVector(2.0*um, 2.0*um, trdSizeZ - 2.0*um);
    G4Transform3D tr1 = G4Transform3D(rotMatr11, position1);
    G4Transform3D tr2 = G4Transform3D(rotMatr2, position2);

    G4Trd* solidTrd1 = new G4Trd("solidTrd1", trdSizeX / 2.0, trdSizeX / 2.0, trdSizeY2 / 2.0, trdSizeY1 / 2.0, trdSizeZ / 2.0);
    
    G4Trd* solidTrd2 = new G4Trd("solidTrd2", trdSizeX / 2.0, trdSizeX / 2.0, trdSizeY1 / 2.0, trdSizeY2 / 2.0, trdSizeZ / 2.0);

    G4MultiUnion* solidHex= new G4MultiUnion("solidHex");

    solidHex->AddNode(*solidTrd1, tr1);
    solidHex->AddNode(*solidTrd2, tr2);
    solidHex->Voxelize();

    //G4VSolid* solidHex = new G4UnionSolid("solidHex1", solidTrd1, solidTrd1, rotMatr, G4ThreeVector(0.0, 0.0, trdSizeZ - 10.0*um));

    G4RotationMatrix* rotMatrY = new G4RotationMatrix();
    rotMatrY->rotateY(M_PI/2.0);

    //G4VSolid* solidHex = new G4SubtractionSolid("solidHex", solidHex1, solidCutout, rotMatrY, G4ThreeVector(trdSizeX/2.0, 0.0, OutRadSizeSide/2.0));

    G4LogicalVolume* logicHex = new G4LogicalVolume(solidHex, PlastBC400, "logicHex");

    G4RotationMatrix* rotMatr1 = new G4RotationMatrix();
    rotMatr1->rotateY(M_PI/2.0);

    //creating shell for hexagon
    G4double layerNew = 200.0*um;
    G4double forcut = layerNew;
    G4double sizeNew = size + 2.0*layerNew/sqrt(3.0);
    G4double layerSizeX = trdSizeX + 2.0*layerNew;
    G4double layerSizeY1 = 2.0 * sizeNew;
    G4double layerSizeY2 = sizeNew;
    G4double layerSizeZ = sizeNew * sqrt(3.0)/2.0;
    
    G4ThreeVector position22 = G4ThreeVector(2.0*um, 2.0*um, layerSizeZ - 2.0*um);
    G4Transform3D tr22 = G4Transform3D(rotMatr2, position2);
    
    G4Trd* solidLayerAndHex1 = new G4Trd("solidLayerAndHex1", layerSizeX / 2.0, layerSizeX / 2.0, layerSizeY2 / 2.0, layerSizeY1 / 2.0, layerSizeZ / 2.0);
    
    G4Trd* solidLayerAndHex2 = new G4Trd("solidLayerAndHex2", layerSizeX / 2.0, layerSizeX / 2.0, layerSizeY1 / 2.0, layerSizeY2 / 2.0, layerSizeZ / 2.0);
    
    G4MultiUnion* solidLayerAndHex= new G4MultiUnion("solidLayerAndHex");

    solidLayerAndHex->AddNode(*solidLayerAndHex1, tr1);
    solidLayerAndHex->AddNode(*solidLayerAndHex2, tr22);
    solidLayerAndHex->Voxelize();

    G4VSolid* solidLayer1 = new G4SubtractionSolid("solidLayer1", solidLayerAndHex, solidHex, rotMatr, G4ThreeVector(0.0, 0.0, 0.0));

    //creating hole inside the hexagon's shell'
    G4Tubs* solidCutout1 = new G4Tubs("solidCutout1", InRadSizeSide, OutRadSizeSide + 200*um, forcut/2.0, 0.0, 2.0 * M_PI);

    G4VSolid* solidLayer = new G4SubtractionSolid("solidLayer", solidLayer1, solidCutout1, rotMatrY, G4ThreeVector(layerSizeX/2.0 - forcut/2.0, 0.0, OutRadSizeSide/2.0));

    G4LogicalVolume* logicLayer = new G4LogicalVolume(solidLayer, Al, "logicLayer");

    G4VPhysicalVolume* physLayer = new G4PVPlacement(rotMatr1, G4ThreeVector(3.0*layerSizeX/2.0 + step, 0.0, 0.0), logicLayer, "physLayer", logWorld, false, 0, 0);
    
    //logicLayer->SetVisAttributes(GSolid);
    
    G4VPhysicalVolume* physHex = new G4PVPlacement(rotMatr1, G4ThreeVector(3.0*layerSizeX/2.0 + step, 0.0, 0.0), logicHex, "physHex", logWorld, false, 0, 0);
    
    //logicHex->SetVisAttributes(YSolid);
    

    //Physical volume for glue between pmt and its window for new detector
   G4VPhysicalVolume* PhysGluePmtNew = new G4PVPlacement(0, G4ThreeVector(sizeNew*sqrt(3.0) + step, 0.0, layerSizeX/2.0 - forcut + standGlueSizeZ/2.0), logicGlue, "PhysGluePmtNew", logWorld, false, 0, 0);

    ///Physical volume for glass window for new detector
    G4VPhysicalVolume* PhysWinPmtNew = new G4PVPlacement(0, G4ThreeVector(sizeNew*sqrt(3.0) + step, 0.0, layerSizeX/2.0 - forcut + glassSizeZ/2.0 + standGlueSizeZ), logicWinPmt, "PhysWinPmtNew", logWorld, false, 0, 0);

    //Physical volume for pmt
    G4VPhysicalVolume* PhysPmtNew = new G4PVPlacement(0, G4ThreeVector(sizeNew*sqrt(3.0) + step, 0.0, layerSizeX/2.0 - forcut + glassSizeZ + standGlueSizeZ + pmtSizeZ/2.0), logicPmt, "PhysPmtNew", logWorld, false, 0, 0);

    //Logical surface
    G4LogicalSkinSurface* logicPmtSurface = new G4LogicalSkinSurface("logicPmtSurface", logicPmt, OpticsPmtSurfaceDD);

    //Optical properties between physical volumes for new detector
   G4LogicalBorderSurface* HexLayer = new G4LogicalBorderSurface("HexLayer", physLayer, physHex, OpticsPmtSurfaceDM);

    G4LogicalBorderSurface* GlueScinNew = new G4LogicalBorderSurface("GlueScinNew", PhysGluePmtNew, physHex, OpticsPmtSurfaceDD);

    G4LogicalBorderSurface* PmtWinshell = new G4LogicalBorderSurface("GlueScinNew", PhysWinPmtNew, physLayer, OpticsPmtSurfaceDD);
    
    G4LogicalBorderSurface* GlueShell = new G4LogicalBorderSurface("GlueShell", PhysGluePmtNew, physLayer, OpticsPmtSurfaceDM);
    
    //G4LogicalBorderSurface* PmtShell = new G4LogicalBorderSurface("PmtShell", PhysPmtNew, physLayer, OpticsPmtSurfaceDM);
    
    G4LogicalBorderSurface* GlueGlassNew = new G4LogicalBorderSurface("GlueGlassNew", PhysGluePmtNew, PhysWinPmtNew, OpticsPmtSurfaceDD);

    G4LogicalBorderSurface* PmtGlassNew = new G4LogicalBorderSurface("PmtGlassNew", PhysWinPmtNew,  PhysPmtNew, OpPmtSurfaceNew);
    
    //Sensitive detector for pmt and stilbene
    G4SDManager* SDman = G4SDManager::GetSDMpointer();// is G4manager class for sensitive detectors
        if(!pmt_SD){//check if pmt_SD does not exists otherwise create it
            pmt_SD = new LcPMTSD("PMT1");
            SDman->AddNewDetector(pmt_SD); //now we've created the SD so it exists(no doubt)
        }
        logicPmt->SetSensitiveDetector(pmt_SD);
        if(!CsI_SD){//check if CsI_SD does not exists otherwise create it
                CsI_SD = new LcCsISD("CsI1");
                SDman->AddNewDetector(CsI_SD);//now we've created the SD so it exists(no doubt)
                logicStandTub->SetSensitiveDetector(CsI_SD);
         }

             //Sensitive detector for new pmt and plastic
        if(!CsI_SD){//check if CsI_SD does not exists otherwise create it
                CsI_SD = new LcCsISD("CsI1");
                SDman->AddNewDetector(CsI_SD);//now we've created the SD so it exists(no doubt)
                logicHex->SetSensitiveDetector(CsI_SD);
         }


    //Checking geometry

    //G4CSGSolid* newCSG = (G4CSGSolid*)PhysCub->GetLogicalVolume()->GetSolid();
    //G4Box* newBox = (G4Box*)newCSG;
    //G4double lenx = newBox->GetXHalfLength();
    //G4double leny = newBox->GetYHalfLength();
    //G4double lenz = newBox->GetZHalfLength();

    //G4cout << "Cube length" << " " << lenx << " " << leny << " " << lenz << "\n";

    //G4ThreeVector centre = PhysCub->GetObjectTranslation();

    //G4cout << centre;

#endif /*NEW_GEOMETRY*/


    //--------------------------------------------------------------------------
    // DATA OF THE SYSTEM

    G4double x1, x2, y1 , y2, z1, z2, zs4, GlassThickness, PMTThickness, ESRThickness, cut;
    x1 = 0.0;
    x2 = 0.0;
    y1 = 0.0;
    y2 = 0.0;
    z1 = 0.0;
    z2 = 0.0;
    zs4 = 0.0;
    GlassThickness = 0.01*mm;
    PMTThickness = 5.00*mm;
    ESRThickness = 0.065*mm;
    cut = 0.0;

    //int config = (int) arg/100;
    /* config 1 = 22 cm length cristal 
       config 2 = 18 cm length cristal
       config 3 = 17 cm length cristal
       */
    G4int config = crType;
    if(config == 1)
    {
        x1 = 16.3*mm;
        x2 = 15.43*mm;
        y1 = 31.38*mm;
        y2 = 29.48*mm;
        z1 = 150*mm;
        z2 = 170*mm;
        zs4 = 42.5*mm;
    }
    else if(config == 2)
    {
        x1 = 16.3*mm;
        x2 = 15.43*mm;
        y1 = 31.15*mm;
        y2 = 29.45*mm;
        z1 = 160*mm;
        z2 = 180*mm;
        zs4 = 45*mm;
    }
    else if(config == 3)
    {
        x1 = 16.28*mm;
        x2 = 15.41*mm;
        y1 = 30.77*mm;
        y2 = 29.38*mm;
        z1 = 200*mm;
        z2 = 220*mm;
        zs4 = 55*mm;
    }
    arg = arg - config*100;
    /* 1 = 1/4 of the length cristal 
       2 = 1/2 of the length cristal
       3 = 3/4 of the length cristal
       */
    G4int distance = (G4int) arg/10;
    if (matFrac == 0)
    {
        cut = z2 - 1.0 * um;
    }
    else if (matFrac < 4)
    {
        cut = z2 - ((G4double)matFrac) * zs4; // distance of treatment from the reflector
    }
    else // full size treatment
    {
        cut = 1.0 * um;
    }
    arg = arg - distance*10;

    //---------------------------------CsI CRISTAL--------------------------

    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot ->rotateZ(M_PI*rad);

    G4double dataCsI[6] = {x1, x2, y1, y2, z1, z2};
    G4double outputCsI[10] = {0};
    trapezeCsI(dataCsI, outputCsI);

    G4Trap* CsITra1 = new G4Trap("CsI",
            outputCsI[0]*mm, outputCsI[1],
            outputCsI[2], outputCsI[3]*mm,
            outputCsI[4]*mm, outputCsI[4]*mm,
            0.0*deg, outputCsI[5]*mm,
            outputCsI[6]*mm, outputCsI[6]*mm,
            0.0*deg);
#ifdef MATERIAL_VACUUM
    G4LogicalVolume* logCsI1 = new G4LogicalVolume(CsITra1, galVacuum, "CsI", 0, 0, 0);
#else
    G4LogicalVolume* logCsI1 = new G4LogicalVolume(CsITra1,CsI,"CsI",0,0,0);
#endif /*MATERIAL_VACUUM*/
#ifndef NEW_GEOMETRY
    G4VPhysicalVolume* PhyCsI1 = new G4PVPlacement(
            zRot,G4ThreeVector(-outputCsI[7],outputCsI[8],outputCsI[9]),logCsI1,"CsI",logWorld,false,0);
#endif /*NEW_GEOMETRY*/
    G4cout << "**************************************************\n";
    G4cout << ">>>>>>>>>> CsI geometry:\n";
    G4cout 
        << "2*pDz=" << 2.0*outputCsI[0]
        << "; 2*pDy1 = " << 2.0*outputCsI[3]
        << "; 2*pDx1 = " << 2.0*outputCsI[4]
        << "; 2*pDy2 = " << 2.0*outputCsI[5]
        << "; 2*pDx3 = " << 2.0*outputCsI[6]
        << G4endl
        << ">>>>>>>>>> CsI position:\n"
        << "x = " << -outputCsI[7]
        << "; y = " << outputCsI[8]
        << "; z = " << outputCsI[9]
        << G4endl;

    G4ThreeVector v1(1.0, 0.0);
    G4ThreeVector v2(0.0, 1.0);
    G4ThreeVector v3(0.0, 0.0);
    v3 = v2+v1;
    G4cout
        << "v1 = " << v1
        << "; v2 = " << v2
        << "; v3 = " << v3
        << "; v3.phi() = " << v3.phi()/deg
        << " deg"
        << G4endl;
///////////////////////////////////////////////////////////////////////////////
// geometry test
///////////////////////////////////////////////////////////////////////////////
//    
//    G4double rotAngle = 5.0*deg;
//    G4double testLength = 100.0*mm;
//    G4double testThck = 10.0*mm;
//    G4double testBoxSize = 10.0*mm;
//    G4Para* testPara = new G4Para(
//            "testPara", testThck/2.0, 30.0*mm, testLength/2.0, 0.0*deg, rotAngle, 0.0*deg);
//    G4LogicalVolume* logPara = new G4LogicalVolume(testPara, galVacuum, "logPara", 0, 0, 0);
//    G4RotationMatrix* testRot = new G4RotationMatrix;
//    testRot ->rotateY(0.0*deg);
//    G4VPhysicalVolume* phyPara = new G4PVPlacement(
//            testRot, 
//            G4ThreeVector(-1.0*(0.5*testLength*tan(rotAngle) + testThck/2.0), 0.0, 0.0), 
//            logPara, "phyPara", logWorld, false, 0);
//    G4Box* testBox = new G4Box("testbox", testBoxSize/2.0, testBoxSize/2.0, testBoxSize/2.0);
//    G4LogicalVolume* logTestBox = new G4LogicalVolume(testBox, galVacuum, "logTestBox", 0, 0, 0);
//    G4VPhysicalVolume* phytestBox = new G4PVPlacement(
//            0, G4ThreeVector(0.0, 0.0, testLength/2.0 + testBoxSize/2.0), 
//            logTestBox, "phytestBox", logWorld, false, 0);
//
///////////////////////////////////////////////////////////////////////////////
    //---------------------------------REFLECTORS------------------------------
    G4double cutTop    = fabs(z2*(1 - ((G4double)fracTop)/4.0) - 1.0*um);
    G4double cutRight  = fabs(z2*(1 - ((G4double)fracRight)/4.0) - 1.0*um);
    G4double cutBottom = fabs(z2*(1 - ((G4double)fracBottom)/4.0) - 1.0*um);
    G4double cutLeft   = fabs(z2*(1 - ((G4double)fracLeft)/4.0) - 1.0*um);

#ifndef NOREFLECTOR
    // FRONT REFLECTOR 
    G4Box* SldESRFront = new G4Box(
            "ESRFront",x2/2.0+ESRThickness, y2/2.0+ESRThickness,ESRThickness/2.0);
    G4LogicalVolume*   logESRFront  = new G4LogicalVolume(SldESRFront,Mylar,"ESRFront",0,0,0);
    G4VPhysicalVolume* PhyESRFront  = new G4PVPlacement(
            zRot,G4ThreeVector(-x2/2.0, y2/2.0, z2+ESRThickness/2.0),
            logESRFront,"PhyESRFront",logWorld,false,0);
    
    G4LogicalVolume*   logESRFrontTopLayer  = new G4LogicalVolume(
		    SldESRFront, Mylar, "ESRFrontTopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRFrontTopLayer  = new G4PVPlacement(
            zRot, G4ThreeVector(-x2/2.0, y2/2.0, z2 + ESRThickness*1.5),
            logESRFrontTopLayer, "PhyESRFrontTopLayer", logWorld, false, 0);

    // RIGHT1 REFLECTOR
    G4double outputR1[10] = {0.0};
    G4double zCutRight;
    if (matType < 10)
    {
        zCutRight = cut;
    }
    else
    {
        zCutRight = cutRight;
    }
    G4double dataR1[6] = {ESRThickness, ESRThickness, y1, y2, zCutRight - (z2 - z1), zCutRight};
    trapezeCsI(dataR1, outputR1);
    G4Trap* SldESRRight1 = new G4Trap("ESRright1",
            outputR1[0]*mm, outputR1[1],
            outputR1[2], outputR1[3]*mm,
            outputR1[4]*mm, outputR1[4]*mm,
            0.0*deg, outputR1[5]*mm,
            outputR1[6]*mm, outputR1[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRRight1  = new G4LogicalVolume (SldESRRight1,Mylar,"ESRr1",0,0,0);
    G4VPhysicalVolume* PhyESRRight1  = new G4PVPlacement(
            zRot,G4ThreeVector(outputR1[7],outputR1[8],2.0*outputCsI[9]-outputR1[9]),
            logESRRight1,"ESRRight1",logWorld,false,0);
    
    G4LogicalVolume*   logESRRight1TopLayer  = new G4LogicalVolume(
		    SldESRRight1, Mylar, "ESRr1TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRRight1TopLayer  = new G4PVPlacement(
            zRot,
	    G4ThreeVector(outputR1[7] + ESRThickness, outputR1[8], 2.0*outputCsI[9] - outputR1[9]),
            logESRRight1TopLayer, "ESRRight1TopLayer", logWorld, false, 0);

    // RIGHT2 REFLECTOR
    G4double outputR2[10] = {0.0};
    G4double dataR2[6] = {
        ESRThickness, ESRThickness, outputCsI[3]*2.0, outputR1[3]*2.0, 0, z2 - zCutRight
    };
    trapezeCsI(dataR2, outputR2);
    G4Trap* SldESRRight2 = new G4Trap("ESRright2",
            outputR2[0]*mm, outputR2[1],
            outputR2[2], outputR2[3]*mm,
            outputR2[4]*mm, outputR2[4]*mm,
            0.0*deg, outputR2[5]*mm,
            outputR2[6]*mm, outputR2[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRRight2  = new G4LogicalVolume (SldESRRight2,Mylar,"ESRr2",0,0,0);
    G4VPhysicalVolume* PhyESRRight2  = new G4PVPlacement(
            zRot,G4ThreeVector(outputR2[7],outputR2[8],outputR2[9]),
            logESRRight2,"ESRRight2",logWorld,false,0);
    
    G4LogicalVolume*   logESRRight2TopLayer  = new G4LogicalVolume(
		    SldESRRight2, Mylar, "ESRr2TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRRight2TopLayer  = new G4PVPlacement(
            zRot,
	    G4ThreeVector(outputR2[7] + ESRThickness, outputR2[8], outputR2[9]),
            logESRRight2TopLayer, "ESRRight2TopLayer", logWorld, false, 0);

    // BOTTOM1 REFLECTOR
    G4double outputB1[10] = {0.0};
    //G4double dataB1[6];
    G4double zCutBottom;
    if (matType < 10)
    {
        zCutBottom = cut;
    }
    else
    {
        zCutBottom = cutBottom;
    }
    G4double dataB1[6] = {
        x1 + ESRThickness, x2 + ESRThickness, ESRThickness, ESRThickness, zCutBottom - (z2 - z1), zCutBottom
    };
    trapezeCsI(dataB1, outputB1);
    G4Trap* SldESRBottom1 = new G4Trap("ESRright1",
            outputB1[0]*mm, outputB1[1],
            outputB1[2], outputB1[3]*mm,
            outputB1[4]*mm, outputB1[4]*mm,
            0.0*deg, outputB1[5]*mm,
            outputB1[6]*mm, outputB1[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRBottom1  = new G4LogicalVolume (SldESRBottom1,Mylar,"ESRb1",0,0,0);
    G4VPhysicalVolume* PhyESRBottom1  = new G4PVPlacement(
            zRot,G4ThreeVector(-outputB1[7],-outputB1[8],2*outputCsI[9]-outputB1[9]),
            logESRBottom1,"ESRBottom1",logWorld,false,0);
    
    G4LogicalVolume*   logESRBottom1TopLayer  = new G4LogicalVolume(
		    SldESRBottom1, Mylar, "ESRb1TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRBottom1TopLayer  = new G4PVPlacement(
            zRot, 
	    G4ThreeVector(-outputB1[7], -outputB1[8] - ESRThickness, 2*outputCsI[9] - outputB1[9]),
            logESRBottom1TopLayer, "ESRBottom1", logWorld, false, 0);

    // BOTTOM2 REFLECTOR
    G4double outputB2[10] = {100.0};
    G4double dataB2[6] = {
        outputCsI[4]*2.0 + ESRThickness, outputB1[4]*2.0, ESRThickness, ESRThickness, 0, z2-zCutBottom
    };
    trapezeCsI(dataB2, outputB2);
    G4Trap* SldESRBottom2 = new G4Trap("ESRBottom2",
            outputB2[0]*mm, outputB2[1],
            outputB2[2], outputB2[3]*mm,
            outputB2[4]*mm, outputB2[4]*mm,
            0.0*deg, outputB2[5]*mm,
            outputB2[6]*mm, outputB2[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRBottom2  = new G4LogicalVolume (SldESRBottom2,Mylar,"ESRb2",0,0,0);
    G4VPhysicalVolume* PhyESRBottom2  = new G4PVPlacement(
            zRot,G4ThreeVector(-outputB2[7],-outputB2[8],outputB2[9]),
            logESRBottom2,"ESRBottom2",logWorld,false,0);
    
    G4LogicalVolume*   logESRBottom2TopLayer  = new G4LogicalVolume(
		    SldESRBottom2, Mylar, "ESRb2TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRBottom2TopLayer  = new G4PVPlacement(
            zRot, 
	    G4ThreeVector(-outputB2[7], -outputB2[8] - ESRThickness, outputB2[9]),
            logESRBottom2TopLayer, "ESRBottom2", logWorld, false, 0);

    // LEFT1 REFLECTOR
    G4double outputL1[14] = {0.0};
    G4double zCutLeft;
    if (matType < 10)
    {
        zCutLeft = cut;
    }
    else
    {
        zCutLeft = cutLeft;
    }
    trapezeLeft(dataCsI, outputL1, ESRThickness, zCutLeft);
    G4RotationMatrix* yzRot = new G4RotationMatrix;
    yzRot ->rotateY(outputL1[10]);
    yzRot ->rotateZ(M_PI*rad);
    G4Trap* SldESRLeft1 = new G4Trap("ESRleft1",
            outputL1[0]*mm, outputL1[1],
            outputL1[2], outputL1[3]*mm,
            outputL1[4]*mm, outputL1[4]*mm,
            0.0*deg, outputL1[5]*mm,
            outputL1[6]*mm, outputL1[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRLeft1  = new G4LogicalVolume (SldESRLeft1,Mylar,"ESRl1",0,0,0);

    G4VPhysicalVolume* PhyESRLeft1 = new G4PVPlacement(
                yzRot,
                G4ThreeVector(outputL1[11] - outputL1[7], outputL1[8], 2.0*outputCsI[9] - zCutLeft/2.0),
                logESRLeft1,"ESRLeft1",logWorld,false,0);
    
    G4LogicalVolume* logESRLeft1TopLayer  = new G4LogicalVolume(
		    SldESRLeft1, Mylar, "ESRl1TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRLeft1TopLayer = new G4PVPlacement(
                yzRot,
                G4ThreeVector(
                        outputL1[11] - outputL1[7] - ESRThickness, 
                        outputL1[8], 
                        2.0*outputCsI[9] - zCutLeft/2.0),
                logESRLeft1TopLayer, "ESRLeft1TopLayer", logWorld, false, 0);

    // LEFT2 REFLECTOR
    G4double outputL2[10] = {100};
    G4double dataL2[6] = {
        ESRThickness, ESRThickness, outputCsI[3]*2.0, outputL1[3]*2.0, 0, outputL1[13]
    };
    trapezeCsI(dataL2, outputL2);
    G4Trap* SldESRLeft2 = new G4Trap("ESRleft2",
            outputL2[0]*mm, outputL2[1],
            outputL2[2], outputL2[3]*mm,
            outputL2[4]*mm, outputL2[4]*mm,
            0.0*deg, outputL2[5]*mm,
            outputL2[6]*mm, outputL2[6]*mm,
            0.0*deg);
    G4LogicalVolume* logESRLeft2  = new G4LogicalVolume (SldESRLeft2,Mylar,"ESRl2",0,0,0);
    G4LogicalVolume* logESRLeft2TopLayer  = new G4LogicalVolume(
		    SldESRLeft2, Mylar, "ESRl2TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRLeft2 = new G4PVPlacement(
                yzRot,G4ThreeVector(outputL1[12]-outputL2[7],outputL2[8],outputCsI[9]-zCutLeft/2.0),
                logESRLeft2,"ESRLeft2",logWorld,false,0);
    G4VPhysicalVolume* PhyESRLeft2TopLayer = new G4PVPlacement(
                yzRot,
                G4ThreeVector(
                        outputL1[12] - outputL2[7] - ESRThickness, 
                        outputL2[8], 
                        outputCsI[9] - zCutLeft/2.0),
                logESRLeft2TopLayer, "ESRLeft2TopLayer", logWorld, false, 0);


    // TOP1 REFLECTOR
    G4double outputT1[14] = {0};
    G4double zCutTop;
    if (matType < 10)
    {
        zCutTop = cut;
    }
    else
    {
        zCutTop = cutTop;
    }
    trapezeTop(dataCsI,outputT1, ESRThickness, zCutTop);
    G4RotationMatrix* xzRot = new G4RotationMatrix;
    xzRot ->rotateX(outputT1[10]);
    xzRot ->rotateZ(M_PI*rad);
    G4Trap* SldESRTop1 = new G4Trap("ESRTop1",
            outputT1[0]*mm, outputT1[1],
            outputT1[2], outputT1[3]*mm,
            outputT1[4]*mm, outputT1[4]*mm,
            0.0*deg, outputT1[5]*mm,
            outputT1[6]*mm, outputT1[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRTop1  = new G4LogicalVolume (SldESRTop1,Mylar,"ESRt1",0,0,0);
    
    G4LogicalVolume*   logESRTop1TopLayer  = new G4LogicalVolume(
		    SldESRTop1, Mylar, "ESRt1TopLayer", 0, 0, 0);
    G4VPhysicalVolume* PhyESRTop1 = new G4PVPlacement(
                xzRot,G4ThreeVector(-outputT1[7],outputT1[11]+outputT1[8],2.0*outputCsI[9]-zCutTop/2.0),
                logESRTop1,"ESRTop1",logWorld,false,0);
    G4VPhysicalVolume* PhyESRTop1TopLayer = new G4PVPlacement(
                xzRot,
                G4ThreeVector(
                        -outputT1[7], 
                        outputT1[11] + outputT1[8] + ESRThickness, 
                        2.0*outputCsI[9] - zCutTop/2.0),
                logESRTop1TopLayer, "ESRtop1TopLayer", logWorld, false, 0);

    // TOP2 REFLECTOR
    G4double outputT2[10] = {100};
    G4double dataT2[6] = {
        outputCsI[4]*2.0, outputT1[4]*2.0, ESRThickness, ESRThickness, 0, outputT1[13]
    };
    trapezeCsI(dataT2, outputT2);
    G4Trap* SldESRTop2 = new G4Trap("ESRTop2",
            outputT2[0]*mm, outputT2[1],
            outputT2[2], outputT2[3]*mm,
            outputT2[4]*mm, outputT2[4]*mm,
            0.0*deg, outputT2[5]*mm,
            outputT2[6]*mm, outputT2[6]*mm,
            0.0*deg);
    G4LogicalVolume*   logESRTop2  = new G4LogicalVolume (SldESRTop2,Mylar,"ESRt2",0,0,0);
    
    G4LogicalVolume*   logESRTop2TopLayer  = new G4LogicalVolume(
		    SldESRTop2, Mylar, "ESRt2TopLayer", 0, 0, 0);
    
    G4VPhysicalVolume* PhyESRTop2 = new G4PVPlacement(
                xzRot,G4ThreeVector(-outputT2[7],outputT1[12]+outputT2[8],outputCsI[9]-zCutTop/2.0),
                logESRTop2,"ESRtop2",logWorld,false,0);
    G4VPhysicalVolume* PhyESRTop2TopLayer = new G4PVPlacement(
                xzRot,
                G4ThreeVector(
                        -outputT2[7], 
                        outputT1[12] + outputT2[8] + ESRThickness, 
                        outputCsI[9] - zCutTop/2.0),
                logESRTop2TopLayer, "ESRtop2TopLayer", logWorld, false, 0);
#endif /*NOREFLECTOR*/


    //------------------------------------------------------------------------------------

    const G4int num = 1;
    G4double Ephoton[num] = {1.7*eV};
    //G4double sigma_alpha=0.1;
    G4double sigma_alpha_polished = 0.077;
    G4double sigma_alpha_unpolished = 0.162;
    G4double RefractiveIndex[num] = {1.0};  
    //G4double SpecularLobe[num]    = {1};//refl. about facet normal //1
    //G4double SpecularSpike[num]   = {0};//refl. about avg suface normal //0
    G4double SpecularLobe[num]    = {0.5};//refl. about facet normal //1
    G4double SpecularSpike[num]   = {0.5};//refl. about avg suface normal //0
    G4double Backscatter[num]     = {0};//refl. in groove, //diffuse lobe constant 
    //G4double Reflectivity[num] = {0.98};
    //G4double Efficiency[num]   = {0.02};
    G4double Reflectivity[num] = {0.9974};
    G4double Efficiency[num]   = {2.6e-3};
    //G4double Reflectivity[num] = {1.0};
    //G4double Efficiency[num]   = {0.0};

    G4MaterialPropertiesTable* ESROpTable = new G4MaterialPropertiesTable();
    ESROpTable->AddProperty("RINDEX", Ephoton, RefractiveIndex, num);
    ESROpTable->AddProperty("SPECULARLOBECONSTANT", Ephoton, SpecularLobe,   num);
    ESROpTable->AddProperty("SPECULARSPIKECONSTANT", Ephoton, SpecularSpike,   num);
    ESROpTable->AddProperty("BACKSCATTERCONSTANT", Ephoton, Backscatter,     num);
    ESROpTable->AddProperty("REFLECTIVITY", Ephoton, Reflectivity, num);
    ESROpTable->AddProperty("EFFICIENCY",   Ephoton, Efficiency,   num);
    
    G4MaterialPropertiesTable* ESROpTableLUT = new G4MaterialPropertiesTable();
    ESROpTableLUT->AddProperty("RINDEX", Ephoton, RefractiveIndex, num);
    ESROpTableLUT->AddProperty("REFLECTIVITY", Ephoton, Reflectivity, num);
    ESROpTableLUT->AddProperty("EFFICIENCY",   Ephoton, Efficiency,   num);

    G4OpticalSurface* OpESRSurfaceTop_polished = new G4OpticalSurface("OpFoilSurface_polished");
    OpESRSurfaceTop_polished->SetType(dielectric_metal);
    OpESRSurfaceTop_polished->SetFinish(polished);
    OpESRSurfaceTop_polished->SetModel(unified); 
    OpESRSurfaceTop_polished->SetMaterialPropertiesTable(ESROpTable);

    G4OpticalSurface* OpESRSurface_polished;
    G4OpticalSurface* OpESRSurface_ground;
    switch (fModelSwitch)
    {
        case 0:
            ////////////////////////////////////////////////////////////////////////
            //UNUFIED
            ////////////////////////////////////////////////////////////////////////
            OpESRSurface_polished = new G4OpticalSurface("ESRSurface_polished");
            OpESRSurface_polished->SetType(dielectric_metal);
            OpESRSurface_polished->SetFinish(polished);
            //OpESRSurface_polished->SetFinish(ground);
            OpESRSurface_polished->SetModel(unified); 
            //OpESRSurface_polished->SetSigmaAlpha(sigma_alpha);
            //OpESRSurface_polished->SetSigmaAlpha(sigma_alpha_polished);
            OpESRSurface_polished->SetMaterialPropertiesTable(ESROpTable);

            OpESRSurface_ground = new G4OpticalSurface("ESRSurface_ground");
            OpESRSurface_ground->SetType(dielectric_metal);
            OpESRSurface_ground->SetFinish(ground);
            OpESRSurface_ground->SetModel(unified); 
            //OpESRSurface_ground->SetSigmaAlpha(sigma_alpha);
            OpESRSurface_ground->SetSigmaAlpha(sigma_alpha_unpolished);
            OpESRSurface_ground->SetMaterialPropertiesTable(ESROpTable);
            break;
        case 1:
            //////////////////////////////////////////////////////////
            //LUT_DAVIS
            //////////////////////////////////////////////////////////
            OpESRSurface_polished = new G4OpticalSurface("ESRSurface_polished");
            OpESRSurface_polished->SetType(dielectric_LUTDAVIS);
            OpESRSurface_polished->SetFinish(PolishedESR_LUT);
            //OpESRSurface_polished->SetFinish(Polished_LUT);
            OpESRSurface_polished->SetMaterialPropertiesTable(ESROpTableLUT);
            OpESRSurface_polished->SetModel(DAVIS);
            
            //G4OpticalSurface* OpESRSurface_polished_back = new G4OpticalSurface("ESRSurface_polished_back");
            //OpESRSurface_polished_back->SetType(dielectric_LUTDAVIS);
            //OpESRSurface_polished_back->SetFinish(Polished_LUT);
            //OpESRSurface_polished_back->SetMaterialPropertiesTable(ESROpTableLUT);
            //OpESRSurface_polished_back->SetModel(DAVIS);
            
            OpESRSurface_ground = new G4OpticalSurface("ESRSurface_ground");
            OpESRSurface_ground->SetType(dielectric_LUTDAVIS);
            OpESRSurface_ground->SetFinish(RoughESR_LUT);
            //OpESRSurface_ground->SetFinish(Rough_LUT);
            //OpESRSurface_ground->SetMaterialPropertiesTable(ESROpTable);
            OpESRSurface_ground->SetMaterialPropertiesTable(ESROpTableLUT);
            OpESRSurface_ground->SetModel(DAVIS);
            
            //G4OpticalSurface* OpESRSurface_ground_back = new G4OpticalSurface("ESRSurface_ground_back");
            //OpESRSurface_ground_back->SetType(dielectric_LUTDAVIS);
            //OpESRSurface_ground_back->SetFinish(Rough_LUT);
            ////OpESRSurface_ground->SetMaterialPropertiesTable(ESROpTable);
            //OpESRSurface_ground_back->SetMaterialPropertiesTable(ESROpTableLUT);
            //OpESRSurface_ground_back->SetModel(DAVIS);
            ////////////////////////////////////////////////////////////////////////
            break;
    } // switch(fModelSwitch)

    G4double Reflectivity_gl[num] = {0.05};
    G4double Efficiency_gl[num]   = {0.95};
    G4double RefractiveIndex_gl[num] = {1.465};

    G4MaterialPropertiesTable* GLOpTable = new G4MaterialPropertiesTable();
    GLOpTable->AddProperty("RINDEX", Ephoton, RefractiveIndex_gl, num);
    GLOpTable->AddProperty("SPECULARLOBECONSTANT", Ephoton, SpecularLobe,   num);
    GLOpTable->AddProperty("SPECULARSPIKECONSTANT", Ephoton, SpecularSpike,   num);
    GLOpTable->AddProperty("BACKSCATTERCONSTANT", Ephoton, Backscatter,     num);
    GLOpTable->AddProperty("REFLECTIVITY", Ephoton, Reflectivity_gl, num);
    GLOpTable->AddProperty("EFFICIENCY",   Ephoton, Efficiency_gl,   num);

    G4OpticalSurface* OpGLSurface_polished = new G4OpticalSurface("GLSurface_polished");
    OpGLSurface_polished->SetType(dielectric_metal);
    OpGLSurface_polished->SetFinish(polished);
    //OpGLSurface_polished->SetFinish(ground);
    OpGLSurface_polished->SetModel(unified); 
    //OpGLSurface_polished->SetSigmaAlpha(sigma_alpha);
    //OpGLSurface_polished->SetSigmaAlpha(sigma_alpha_polished);
    OpGLSurface_polished->SetMaterialPropertiesTable(GLOpTable);
    
#ifndef NOREFLECTOR
    new G4LogicalBorderSurface("ESRSurfaceFront", PhyCsI1,     PhyESRFront, OpESRSurface_polished);
    //new G4LogicalBorderSurface("ESRSurfaceFront", PhyESRFront, PhyCsI1,     OpESRSurface_polished_back);
    new G4LogicalBorderSurface(
            "ESRSurfaceFront", PhyESRFront, PhyESRFrontTopLayer, OpESRSurfaceTop_polished);
            
    new G4LogicalBorderSurface(
            "ESRSurfaceBot2TopLayer",   PhyESRBottom2, PhyESRBottom2TopLayer, OpESRSurfaceTop_polished);
    new G4LogicalBorderSurface(
            "ESRSurfaceRight2TopLayer", PhyESRRight2,  PhyESRRight2TopLayer,  OpESRSurfaceTop_polished);
    new G4LogicalBorderSurface(
            "ESRSurfaceTop2TopLayer",   PhyESRTop2,    PhyESRTop2TopLayer,    OpESRSurfaceTop_polished);
    new G4LogicalBorderSurface(
            "ESRSurfaceLeft2TopLayer",  PhyESRLeft2,   PhyESRLeft2TopLayer,   OpESRSurfaceTop_polished);
        
    new G4LogicalBorderSurface(
            "ESRSurfaceBot1TopLayer",   PhyESRBottom1, PhyESRBottom1TopLayer, OpESRSurfaceTop_polished);
    new G4LogicalBorderSurface(
            "ESRSurfaceRight1TopLayer", PhyESRRight1,  PhyESRRight1TopLayer,  OpESRSurfaceTop_polished);
    new G4LogicalBorderSurface(
            "ESRSurfaceTop1TopLayer",   PhyESRTop1,    PhyESRTop1TopLayer,    OpESRSurfaceTop_polished);
    new G4LogicalBorderSurface(
            "ESRSurfaceLeft1TopLayer",  PhyESRLeft1,   PhyESRLeft1TopLayer,   OpESRSurfaceTop_polished);

    if (matType < 10)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot1",   PhyCsI1, PhyESRBottom1, OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight1", PhyCsI1, PhyESRRight1,  OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop1",   PhyCsI1, PhyESRTop1,    OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceLeft1",  PhyCsI1, PhyESRLeft1,   OpESRSurface_polished);
        
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceBot1Back",   PhyESRBottom1, PhyCsI1, OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceRight1Back", PhyESRRight1,  PhyCsI1, OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceTop1Back",   PhyESRTop1,    PhyCsI1, OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceLeft1Back",  PhyESRLeft1,   PhyCsI1, OpESRSurface_polished_back);
    }
    else
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",   PhyCsI1, PhyESRBottom2, OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2", PhyCsI1, PhyESRRight2,  OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",   PhyCsI1, PhyESRTop2,    OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",  PhyCsI1, PhyESRLeft2,   OpESRSurface_polished);
        
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceBot2Back",   PhyESRBottom2, PhyCsI1, OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceRight2Back", PhyESRRight2,  PhyCsI1, OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceTop2Back",   PhyESRTop2,    PhyCsI1, OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "ESRSurfaceLeft2Back",  PhyESRLeft2,   PhyCsI1, OpESRSurface_polished_back);
    }

    /* 1 = no treatment
       2 = top treated
       3 = left treated
       4 = top bottom treated
       5 = top left treated 
       6 = left right treated
       7 = top bottom left treated
       8 = top left right treated
       9 = 4 surfaces treated
       10 = right treated from the far side, matFrac=0 -> 100%, matFrac=1 -> 75%, ... matFrac=3 -> 25%
       11 = right top from the far side
       12 = right top bottom from the far side*/

    if (matType == 1)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_polished);

        logESRBottom2 ->SetVisAttributes(Gr1Solid); 
        logESRRight2 ->SetVisAttributes(Gr1Solid);
        logESRTop2 ->SetVisAttributes(Gr1Solid);
        logESRLeft2 ->SetVisAttributes(Gr1Solid);
    }
    else if (matType == 2)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_polished);
        
        //new G4LogicalBorderSurface("ESRSurfaceTop2Back", PhyESRTop2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(Gr1Solid); 
        logESRRight2 ->SetVisAttributes(Gr1Solid);
        logESRTop2 ->SetVisAttributes(RSolid);
        logESRLeft2 ->SetVisAttributes(Gr1Solid);
    }
    else if (matType == 3)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceLeft2Back", PhyESRLeft2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(Gr1Solid); 
        logESRRight2 ->SetVisAttributes(Gr1Solid);
        logESRTop2 ->SetVisAttributes(Gr1Solid);
        logESRLeft2 ->SetVisAttributes(RSolid);
    }
    else if (matType == 4)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_polished);
        
        //new G4LogicalBorderSurface("ESRSurfaceBot2Back", PhyESRBottom2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop2Back", PhyESRTop2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(RSolid); 
        logESRRight2 ->SetVisAttributes(Gr1Solid);
        logESRTop2 ->SetVisAttributes(RSolid);
        logESRLeft2 ->SetVisAttributes(Gr1Solid);
    }
    else if (matType == 5)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceTop2Back", PhyESRTop2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceLeft2Back", PhyESRLeft2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(Gr1Solid); 
        logESRRight2 ->SetVisAttributes(Gr1Solid);
        logESRTop2 ->SetVisAttributes(RSolid);
        logESRLeft2 ->SetVisAttributes(RSolid);
    }
    else if (matType == 6)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceRight2Back", PhyESRRight2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceLeft2Back", PhyESRLeft2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(Gr1Solid); 
        logESRRight2 ->SetVisAttributes(RSolid);
        logESRTop2 ->SetVisAttributes(Gr1Solid);
        logESRLeft2 ->SetVisAttributes(RSolid);
    }
    else if (matType == 7)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceBot2Back", PhyESRBottom2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop2Back", PhyESRTop2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceLeft2Back", PhyESRLeft2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(RSolid); 
        logESRRight2 ->SetVisAttributes(Gr1Solid);
        logESRTop2 ->SetVisAttributes(RSolid);
        logESRLeft2 ->SetVisAttributes(RSolid);
    }
    else if (matType == 8)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceRight2Back", PhyESRRight2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop2Back", PhyESRTop2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceLeft2Back", PhyESRLeft2, PhyCsI1, OpESRSurface_ground_back);

        logESRBottom2 ->SetVisAttributes(Gr1Solid); 
        logESRRight2 ->SetVisAttributes(RSolid);
        logESRTop2 ->SetVisAttributes(RSolid);
        logESRLeft2 ->SetVisAttributes(RSolid);
    }
    else if (matType == 9)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot2",PhyCsI1,PhyESRBottom2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceRight2",PhyCsI1,PhyESRRight2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceTop2",PhyCsI1,PhyESRTop2,OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft2",PhyCsI1,PhyESRLeft2,OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceBot2Back", PhyESRBottom2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceRight2Back", PhyESRRight2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop2Back", PhyESRTop2, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceLeft2Back", PhyESRLeft2, PhyCsI1, OpESRSurface_ground_back);

        //logESRBottom2 ->SetVisAttributes(RSolid); 
        //logESRRight2 ->SetVisAttributes(RSolid);
        //logESRTop2 ->SetVisAttributes(RSolid);
        //logESRLeft2 ->SetVisAttributes(RSolid);
        logESRLeft2 ->SetVisAttributes(RWire);
        logESRBottom2->SetVisAttributes(RWire); 
        logESRRight2->SetVisAttributes(RWire);
        logESRTop2->SetVisAttributes(RWire);
        logESRLeft2->SetVisAttributes(RWire);
    }
    //else if (matType == 10)
    //{
    //    new G4LogicalBorderSurface("ESRSurfaceBot1",   PhyCsI1, PhyESRBottom1, OpESRSurface_polished);
    //    new G4LogicalBorderSurface("ESRSurfaceRight1", PhyCsI1, PhyESRRight1,  OpESRSurface_ground);
    //    new G4LogicalBorderSurface("ESRSurfaceTop1",   PhyCsI1, PhyESRTop1,    OpESRSurface_polished);
    //    new G4LogicalBorderSurface("ESRSurfaceLeft1",  PhyCsI1, PhyESRLeft1,   OpESRSurface_polished);
    //    
    //    new G4LogicalBorderSurface("ESRSurfaceRight1Back", PhyESRRight1,  PhyCsI1, OpESRSurface_ground_back);

    //    logESRLeft1->SetVisAttributes(RWire);
    //    logESRBottom1->SetVisAttributes(RWire); 
    //    logESRRight1->SetVisAttributes(RWire);
    //    logESRTop1->SetVisAttributes(RWire);
    //    logESRLeft1->SetVisAttributes(RWire);
    //}
    else if (matType == 10)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot1",   PhyCsI1, PhyESRBottom1, OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceRight1", PhyCsI1, PhyESRRight1,  OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceTop1",   PhyCsI1, PhyESRTop1,    OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft1",  PhyCsI1, PhyESRLeft1,   OpESRSurface_ground);
        
        //new G4LogicalBorderSurface("ESRSurfaceBot1Back",   PhyESRBottom1, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceRight1Back", PhyESRRight1,  PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop1Back",   PhyESRTop1,    PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceLeft1Back",  PhyESRLeft1,   PhyCsI1, OpESRSurface_ground_back);

        logESRLeft1->SetVisAttributes(RWire);
        logESRBottom1->SetVisAttributes(RWire); 
        logESRRight1->SetVisAttributes(RWire);
        logESRTop1->SetVisAttributes(RWire);
        logESRLeft1->SetVisAttributes(RWire);
    }
    else if (matType == 11)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot1",   PhyCsI1, PhyESRBottom1, OpESRSurface_polished);
        new G4LogicalBorderSurface("ESRSurfaceRight1", PhyCsI1, PhyESRRight1,  OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceTop1",   PhyCsI1, PhyESRTop1,    OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft1",  PhyCsI1, PhyESRLeft1,   OpESRSurface_polished);
        
        //new G4LogicalBorderSurface("ESRSurfaceRight1Back", PhyESRRight1,  PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop1Back",   PhyESRTop1,    PhyCsI1, OpESRSurface_ground_back);

        logESRLeft1->SetVisAttributes(RWire);
        logESRBottom1->SetVisAttributes(RWire); 
        logESRRight1->SetVisAttributes(RWire);
        logESRTop1->SetVisAttributes(RWire);
        logESRLeft1->SetVisAttributes(RWire);
    }
    else if (matType == 12)
    {
        new G4LogicalBorderSurface("ESRSurfaceBot1",   PhyCsI1, PhyESRBottom1, OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceRight1", PhyCsI1, PhyESRRight1,  OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceTop1",   PhyCsI1, PhyESRTop1,    OpESRSurface_ground);
        new G4LogicalBorderSurface("ESRSurfaceLeft1",  PhyCsI1, PhyESRLeft1,   OpESRSurface_polished);
        
        //new G4LogicalBorderSurface("ESRSurfaceBot1Back",   PhyESRBottom1, PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceRight1Back", PhyESRRight1,  PhyCsI1, OpESRSurface_ground_back);
        //new G4LogicalBorderSurface("ESRSurfaceTop1Back",   PhyESRTop1,    PhyCsI1, OpESRSurface_ground_back);

        logESRLeft1->SetVisAttributes(RWire);
        logESRBottom1->SetVisAttributes(RWire); 
        logESRRight1->SetVisAttributes(RWire);
        logESRTop1->SetVisAttributes(RWire);
        logESRLeft1->SetVisAttributes(RWire);
    }
#endif /*NOREFLECTOR*/

    /// ******************** PMT surface******************
    G4OpticalSurface* OpPmtSurface = new G4OpticalSurface("PmtSurface");
    OpPmtSurface->SetType(dielectric_metal);
    OpPmtSurface->SetFinish(polished);
    OpPmtSurface->SetModel(glisur);
    G4double Reflectivity_pmt[num] = {0.0};
    G4double Efficiency_pmt[num]   = {1.0};
    G4double RefractiveIndex_pmt[num] = {1.48};


    G4MaterialPropertiesTable *PmtOpTable = new G4MaterialPropertiesTable();	
    PmtOpTable->AddProperty("REFLECTIVITY", Ephoton, Reflectivity_pmt, num);
    PmtOpTable->AddProperty("EFFICIENCY",   Ephoton, Efficiency_pmt,   num);
    PmtOpTable->AddProperty("RINDEX", Ephoton, RefractiveIndex_pmt, num);
    OpPmtSurface->SetMaterialPropertiesTable(PmtOpTable);
    
    //APD surface
    G4OpticalSurface *opApdSurface = new G4OpticalSurface("opApdSurface");
    opApdSurface->SetType(dielectric_metal);
    opApdSurface->SetFinish(polished);
    //opApdSurface->SetModel(glisur);
    opApdSurface->SetModel(unified);
    
#ifndef NOREFLECTOR
    G4double reflectivityApd[num] = {0.0};
    //double reflectivityApd[num] = {0.35872};
    //double refractiveIndexApd[num] = {1.58};
    G4double refractiveIndexApd[num] = {3.9854};
    G4double efficiencyApd[nEntriesCsI]   = {0.848, 0.855, 0.859, 0.861, 0.861,
            0.861, 0.859, 0.856, 0.854, 0.851, 0.847, 0.843, 0.840, 0.835, 0.830,
            0.823, 0.817, 0.810, 0.801, 0.791, 0.783, 0.773, 0.761, 0.747, 0.732,
            0.721, 0.707, 0.693, 0.681, 0.664, 0.646, 0.622, 0.594, 0.563, 0.546,
            0.533, 0.524, 0.515, 0.509, 0.503}; // from Hamamatsu s8664 series apd 1012 data sheet
    for (G4int i = 0; i < nEntriesCsI; i++)
    {
        efficiencyApd[i] = 1.0;
    }
#else
    G4double reflectivityApd[num] = {0.0};
    G4double refractiveIndexApd[num] = {1.0};
    G4double efficiencyApd[nEntriesCsI];
    for (G4int i = 0; i < nEntriesCsI; i++)
        efficiencyApd[i] = 1.0;
#endif /*NOREFLECTOR*/

    G4MaterialPropertiesTable *apdOpTable = new G4MaterialPropertiesTable();
    apdOpTable->AddProperty("REFLECTIVITY", Ephoton, reflectivityApd, num);
    apdOpTable->AddProperty("EFFICIENCY", PhotonEnergySpectrum, efficiencyApd, nEntriesCsI);
    apdOpTable->AddProperty("RINDEX", Ephoton, refractiveIndexApd, num);

    opApdSurface->SetMaterialPropertiesTable(apdOpTable);

    G4double EpoxyThickness = 0.62*mm;
    G4double EpoxyWidth = 12.1*mm;
    G4double EpoxyHeight = 25.3*mm;
    G4double APDThickness = 0.30*mm; // only an estimate; not important
    G4double APDSide = 10.00*mm; // square side length;
    G4double APDGap = 1.54*mm; //
    G4double CeramicWidth = 14.14*mm;
    G4double CeramicHeight = 27.28*mm;
    G4double CeramicThickness = 2.04*mm;
    G4double siliconeRubberThickness = 5.0e-3*mm;
    //double siliconeRubberThickness = 10.0*mm;

    if (detType == 1)
    {
        //---------------------------------PMT SYSTEM---------------------------

        // GLASS
        G4Box* SldPmtgl = new G4Box("glass",outputCsI[4],outputCsI[3],GlassThickness/2.0);
        G4LogicalVolume* logPmtgl = new G4LogicalVolume(SldPmtgl,Glass,"PMTgl",0,0,0);
        new G4PVPlacement(
                0,G4ThreeVector(-outputCsI[4],outputCsI[3],-GlassThickness/2.0),
                logPmtgl,"PMTgl",logWorld,false,0);

#ifndef NOREFLECTOR
        G4Box* grt = new G4Box("grt",outputCsI[4],ESRThickness/2.0,GlassThickness/2.0);
        G4LogicalVolume* loggrt = new G4LogicalVolume(grt,Mylar,"grt",0,0,0);
        G4VPhysicalVolume* Phygrt = new G4PVPlacement(
                0,G4ThreeVector(-outputCsI[4],outputCsI[3]*2.0+ESRThickness,-GlassThickness/2.0),
                loggrt,"PMTgrt",logWorld,false,0);

        G4Box* grb = new G4Box("grb",outputCsI[4],ESRThickness/2.0,GlassThickness/2.0);
        G4LogicalVolume* loggrb = new G4LogicalVolume(grb,Mylar,"grb",0,0,0);
        G4VPhysicalVolume* Phygrb = new G4PVPlacement(
                0,G4ThreeVector(-outputCsI[4],-ESRThickness,-GlassThickness/2.0),
                loggrb,"PMTgrb",logWorld,false,0);

        G4Box* grl = new G4Box("grl",ESRThickness/2.0,outputCsI[3],GlassThickness/2.0);
        G4LogicalVolume* loggrl = new G4LogicalVolume(grl,Mylar,"grl",0,0,0);
        G4VPhysicalVolume* Phygrl = new G4PVPlacement(
                0,G4ThreeVector(-outputCsI[4]*2.0-ESRThickness,outputCsI[3],-GlassThickness/2.0),
                loggrl,"PMTgrl",logWorld,false,0);

        G4Box* grr = new G4Box("grr",ESRThickness/2.0,outputCsI[3],GlassThickness/2.0);
        G4LogicalVolume* loggrr = new G4LogicalVolume(grr,Mylar,"grr",0,0,0);
        G4VPhysicalVolume* Phygrr = new G4PVPlacement(
                0,G4ThreeVector(ESRThickness,outputCsI[3],-GlassThickness/2.0),
                loggrl,"PMTgrl",logWorld,false,0);

        loggrb ->SetVisAttributes(BSolid);
        loggrr ->SetVisAttributes(BSolid);
        loggrl ->SetVisAttributes(BSolid);
        loggrt ->SetVisAttributes(BSolid);
#endif /*NOREFLECTOR*/

#ifndef NEW_GEOMETRY      // PMT
        G4Box* SldPmt = new G4Box("PMT",outputCsI[4],outputCsI[3],PMTThickness/2.0);
        G4LogicalVolume* logPmt = new G4LogicalVolume(SldPmt,Air,"PMT",0,0,0);
        new G4PVPlacement(
                0,G4ThreeVector(-outputCsI[4],outputCsI[3],-GlassThickness-PMTThickness/2.0),
                logPmt,"PMT",logWorld,false,0);

        new G4LogicalSkinSurface("PmtSurface",logPmt, OpPmtSurface);

#endif /*NEW_GEOMETRY*/

#ifndef NOREFLECTOR
        new G4LogicalBorderSurface("Surfacegrt",PhyCsI1,Phygrt,OpGLSurface_polished);
        new G4LogicalBorderSurface("Surfacegrr",PhyCsI1,Phygrr,OpGLSurface_polished);
        new G4LogicalBorderSurface("Surfacegrb",PhyCsI1,Phygrb,OpGLSurface_polished);
        new G4LogicalBorderSurface("Surfacegrl",PhyCsI1,Phygrl,OpGLSurface_polished);

        logPmtgl ->SetVisAttributes(BSolid);
#endif /*NOREFLECTOR*/
#ifndef NEW_GEOMETRY
        logPmt ->SetVisAttributes(Gr5Solid);
#endif /*NEW_GEOMETRY*/
        /// --------------Sensitive Detector-------------------------------------------------

        //G4SDManager* SDman = G4SDManager::GetSDMpointer();// is G4manager class for sensitive detectors
        if(!pmt_SD){//check if pmt_SD does not exists otherwise create it
            pmt_SD = new LcPMTSD("PMT1");
            SDman->AddNewDetector(pmt_SD); //now we've created the SD so it exists(no doubt)
        }
#ifndef NEW_GEOMETRY
       logPmt->SetSensitiveDetector(pmt_SD);
#endif /*NEW_GEOMETRY*/
    }
    else if (detType == 3)
    {
        //Silicone rubber
        G4Box* sldSiliconeRubber = new G4Box(
                "sldSiliconeRubber", CeramicWidth/2.0, CeramicHeight/2.0, siliconeRubberThickness/2.0);
        G4LogicalVolume* logSiliconeRubber = new G4LogicalVolume(
                sldSiliconeRubber, siliconeRubber, "logSiliconeRubber", 0, 0, 0);

#ifndef NOREFLECTOR
        G4VPhysicalVolume* phySiliconeRubber = new G4PVPlacement(
                0, G4ThreeVector(-outputCsI[4], outputCsI[3], -siliconeRubberThickness/2.0),
                logSiliconeRubber, "phySiliconeRubber", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        //epoxy
        //G4Box* sldEpoxy = new G4Box("sldEpoxy", CeramicWidth/2.0, CeramicHeight/2.0, GlassThickness/2.0);
        G4Box* sldEpoxy = new G4Box(
                "sldEpoxy", CeramicWidth/2.0, CeramicHeight/2.0, EpoxyThickness/2.0);
        G4LogicalVolume* logEpoxy = new G4LogicalVolume(sldEpoxy, Glass, "logEpoxy", 0, 0, 0);
        //G4VPhysicalVolume* phyEpoxy = new G4PVPlacement(
        //        0, G4ThreeVector(-outputCsI[4], outputCsI[3], -GlassThickness/2.0),
        //        logEpoxy, "phyEpoxy", logWorld, false, 0);

#ifndef NOREFLECTOR
        G4VPhysicalVolume* phyEpoxy = new G4PVPlacement(
                0, G4ThreeVector(
                    -outputCsI[4], outputCsI[3], -siliconeRubberThickness-EpoxyThickness/2.0),
                logEpoxy, "phyEpoxy", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        //back esr
        G4Box* sldEsrBackBulk = new G4Box(
                "sldEsrBackBulk", 
                outputCsI[4]+ESRThickness, 
                outputCsI[3]+ESRThickness, 
                ESRThickness/2.0);
        //G4Box* sldThickEpoxy = new G4Box(
        //        "sldThickEpoxy", 
        //        CeramicWidth/2.0 - 1.0e-3*mm, 
        //        CeramicHeight/2.0 - 1.0e-3*mm, 
        //        100.0*GlassThickness/2.0);
        G4Box* sldThickEpoxy = new G4Box(
                "sldThickEpoxy", 
                CeramicWidth/2.0, 
                CeramicHeight/2.0, 
                10.0*cm/2.0);
        G4SubtractionSolid* sldEsrBack = new G4SubtractionSolid(
                "sldEsrBack", sldEsrBackBulk, sldThickEpoxy);
        G4LogicalVolume* logEsrBack = new G4LogicalVolume(sldEsrBack, Mylar, "logEsrBack", 0, 0, 0);

#ifndef NOREFLECTOR
        G4VPhysicalVolume* phyEsrBack = new G4PVPlacement(
                0, G4ThreeVector(
                    -outputCsI[4], 
                    outputCsI[3], 
                    -ESRThickness/2.0),
                logEsrBack, "phyEsrBack", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        G4LogicalVolume* logEsrBackTopLayer = new G4LogicalVolume(
                sldEsrBack, Mylar, "logEsrBackTopLayer", 0, 0, 0);

#ifndef NOREFLECTOR
        G4VPhysicalVolume* phyEsrBackTopLayer = new G4PVPlacement(
                0, G4ThreeVector(
                    -outputCsI[4], 
                    outputCsI[3], 
                    -ESRThickness*1.5),
                logEsrBackTopLayer, "phyEsrBackTopLayer", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        // ESR plug for the photons leaking through phySiliconeRubber
        //G4Box* sldCeramicBulk = new G4Box(
        //        "sldCeramicBulk", CeramicWidth/2.0, CeramicHeight/2.0, CeramicThickness/2.0);
        G4Box* sldLeakPlugBulk = new G4Box(
                "sldLeakPlugBulk", 1.1*CeramicWidth/2.0, 1.1*CeramicHeight/2.0, CeramicThickness/2.0);
        G4SubtractionSolid* sldLeakPlug = new G4SubtractionSolid(
                "sldLeakPlug", sldLeakPlugBulk, sldThickEpoxy);
        G4LogicalVolume* logLeakPlug = new G4LogicalVolume(sldLeakPlug, Mylar, "logLeakPlug", 0, 0, 0);

#ifndef NOREFLECTOR
        G4VPhysicalVolume* phyLeakPlug = new G4PVPlacement(
                0, G4ThreeVector(-outputCsI[4], outputCsI[3], -ESRThickness-CeramicThickness/2.0),
                //0, G4ThreeVector(-outputCsI[4], outputCsI[3], -CeramicThickness/2.0),
                logLeakPlug, "phyLeakPlug", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        //apd
        //G4Box* sldApd = new G4Box("sldApd", CeramicWidth/2.0, CeramicHeight/2.0, PMTThickness/2.0);
#ifndef NOREFLECTOR
        G4Box* sldApd = new G4Box("sldApd", APDSide/2.0, APDSide/2.0, APDThickness/2.0);
#else
        G4double thicknessScale = 100.0;
        G4Box* sldApd = new G4Box("sldApd", APDSide/2.0, APDSide/2.0, thicknessScale*APDThickness/2.0);
#endif /*NOREFLECTOR*/
        //G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, Air, "logApd", 0, 0, 0);
#ifndef NOREFLECTOR
        //G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, silicon, "logApd", 0, 0, 0);
        G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, Glass, "logApd", 0, 0, 0);
#elif defined(WORLD_VACUUM)
        G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, galVacuum, "logApd", 0, 0, 0);
#else
        G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, Air, "logApd", 0, 0, 0);
#endif /*NOREFLECTOR*/
        //G4VPhysicalVolume* phyApd = new G4PVPlacement(
        //        0, G4ThreeVector(-outputCsI[4], outputCsI[3], -GlassThickness-PMTThickness/2.0),
        //        logApd, "phyApd", logWorld, false, 0);
#ifndef NOREFLECTOR
        G4ThreeVector posApd1 = G4ThreeVector(
                -outputCsI[4], 
                outputCsI[3]-APDSide/2.0-APDGap/2.0, 
                -siliconeRubberThickness-EpoxyThickness-APDThickness/2.0);
#else
        G4double csiApdGap = 3.0*cm;
        G4ThreeVector posApd1 = G4ThreeVector(
                -outputCsI[4], 
                outputCsI[3]-APDSide/2.0-APDGap/2.0, 
                -siliconeRubberThickness-EpoxyThickness-thicknessScale*APDThickness/2.0 - csiApdGap);
#endif /*NOREFLECTOR*/

#ifndef NEW_GEOMETRY
        G4ThreeVector posApd2 = G4ThreeVector(
                -outputCsI[4],
                outputCsI[3]+APDSide/2.0+APDGap/2.0,
                -siliconeRubberThickness-EpoxyThickness-APDThickness/2.0);

        G4VPhysicalVolume* phyApd1 = new G4PVPlacement(
                        0, posApd1, logApd, "phyApd1", logWorld, false, 0);
#endif /*NEW_GEOMETRY*/
#ifndef NOREFLECTOR
        G4VPhysicalVolume* phyApd2 = new G4PVPlacement(
                        0, posApd2, logApd, "phyApd2", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        //ceramic transition vectors
        G4ThreeVector transEpoxy(0, 0, CeramicThickness/2.0-EpoxyThickness/2.0);
        G4ThreeVector transApd1(
                0, 
                APDSide/2.0+APDGap/2.0, 
                CeramicThickness/2.0-EpoxyThickness-APDThickness/2.0);
        G4ThreeVector transApd2(
                0, 
                -APDSide/2.0-APDGap/2.0, 
                CeramicThickness/2.0-EpoxyThickness-APDThickness/2.0);

        //ceramic geometry
        G4Box* sldCeramicBulk = new G4Box(
                "sldCeramicBulk", CeramicWidth/2.0, CeramicHeight/2.0, CeramicThickness/2.0);
        G4SubtractionSolid* sldCeramicEpoxyOut = new G4SubtractionSolid(
                "sldCeramicEpoxyOut", sldCeramicBulk, sldEpoxy, 0, transEpoxy);
        G4SubtractionSolid* sldCeramic1ApdOut = new G4SubtractionSolid(
                "sldCeramic1ApdOut", sldCeramicEpoxyOut, sldApd, 0, transApd1);
        G4SubtractionSolid* sldCeramic2ApdOut = new G4SubtractionSolid(
                "sldCeramic2ApdOut", sldCeramic1ApdOut, sldApd, 0, transApd2);
        G4LogicalVolume* logCeramic2ApdOut = new G4LogicalVolume(
                sldCeramic2ApdOut, Ceramic, "logCeramic2ApdOut", 0, 0, 0);

#ifndef NOREFLECTOR
        G4VPhysicalVolume* phyCeramic2ApdOut = new G4PVPlacement(
                0, 
                G4ThreeVector(
                    -outputCsI[4], 
                    outputCsI[3], 
                    -siliconeRubberThickness-CeramicThickness/2.0),
                logCeramic2ApdOut, "phyCeramic2ApdOut", logWorld, false, 0);
#endif /*NOREFLECTOR*/

        new G4LogicalSkinSurface("apdSurface", logApd, OpPmtSurface);
        new G4LogicalSkinSurface("apdSurface", logApd, opApdSurface);

#ifndef NOREFLECTOR
        new G4LogicalBorderSurface("esrBackSurface",     PhyCsI1,    phyEsrBack, OpESRSurface_polished);
        //G4VPhysicalVolume* phySiliconeRubber = new G4PVPlacement(
        //        0, G4ThreeVector(-outputCsI[4], outputCsI[3], -siliconeRubberThickness/2.0),
        //        logSiliconeRubber, "phySiliconeRubber", logWorld, false, 0);
        new G4LogicalBorderSurface(
                "esrBackSRubberSurface", phySiliconeRubber, phyEsrBack, OpESRSurface_polished);
        //G4VPhysicalVolume* phyEpoxy = new G4PVPlacement(
        //        0, G4ThreeVector(
        //            -outputCsI[4], outputCsI[3], -siliconeRubberThickness-EpoxyThickness/2.0),
        //        logEpoxy, "phyEpoxy", logWorld, false, 0);
        new G4LogicalBorderSurface(
                "esrBackEpoxySurface", phyEpoxy, phyEsrBack, OpESRSurface_polished);
        //new G4LogicalSkinSurface("esrBackSurface", logEsrBack, OpESRSurface_polished);
        
        //new G4LogicalBorderSurface("esrBackSurfaceBack", phyEsrBack, PhyCsI1,    OpESRSurface_polished_back);
        new G4LogicalBorderSurface(
                "esrBackSurfaceTopLayer", phyEsrBack, phyEsrBackTopLayer, OpESRSurfaceTop_polished);
        //new G4LogicalBorderSurface("epoxySurface", PhyCsI1, phyEpoxy, OpGLSurface_polished);
        //new G4LogicalSkinSurface("logLeakPlugSurface", logLeakPlug, OpESRSurface_polished);
        new G4LogicalBorderSurface(
                "leakPlugEpoxySurface", phyEpoxy, phyLeakPlug, OpESRSurface_polished);
        
        logEsrBack->SetVisAttributes(GWire);
        phyEsrBack->CheckOverlaps();
        //phyLeakPlug->CheckOverlaps();
        logCeramic2ApdOut->SetVisAttributes(BWire);
        logEpoxy->SetVisAttributes(RBWire);
#endif /*NOREFLECTOR*/
        logApd->SetVisAttributes(YSolid);

#ifndef NEW_GEOMETRY
        G4cout
            << ">>>> APD1 position: " << posApd1
            << "\n>>>> APD2 position: " << posApd2
            << G4endl;
#endif /*NEW_GEOMETRY*/
#ifndef NOREFLECTOR
        phyEpoxy->CheckOverlaps();
        phyApd2->CheckOverlaps();
        phyCeramic2ApdOut->CheckOverlaps();
#endif /*NOREFLECTOR*/
        //phyApd1->CheckOverlaps();

        //G4SDManager* SDman = G4SDManager::GetSDMpointer();// is G4manager class for sensitive detectors
#ifndef NEW_GEOMETRY
        if (!pmt_SD)
        {
            pmt_SD = new LcPMTSD("PMT1");
            SDman->AddNewDetector(pmt_SD);
//#endif /*NEW_GEOMETRY*/
#ifdef NOREFLECTOR
 #ifdef TRACES
            if (!csiAbs_SD)
            {
                csiAbs_SD = new LcPMTSD("PMT1000");
                SDman->AddNewDetector(csiAbs_SD);
            }


#endif /*TRACES*/
#endif /*NOREFLECTOR*/
        }
#endif /*NEW_GEOMETRY*/
#ifdef PHOTON_COUNTER
        if (!photonCounterSD)
        {
            photonCounterSD = new LcPhotonCounterSD("photonCounterSD");
            SDman->AddNewDetector(photonCounterSD);
        }
#endif /*PHOTON_COUNTER*/
//#ifndef NEW_GEOMETRY
        logApd->SetSensitiveDetector(pmt_SD);
//#endif /*NEW_GEOMETRY*/
#ifdef PHOTON_COUNTER
        logPhotonCounter->SetSensitiveDetector(photonCounterSD);
#endif /*PHOTON_COUNTER*/
#ifdef NOREFLECTOR
 #ifdef TRACES
        logCsI1->SetSensitiveDetector(csiAbs_SD);
 #endif /*TRACES*/
#endif /*NOREFLECTOR*/

    }
    else if (detType == 7)
    {
        //G4Box* SldPmt = new G4Box("PMT",outputCsI[4],outputCsI[3],PMTThickness/2.0);
        //Silicone rubber
        //G4Box* sldSiliconeRubber = new G4Box(
        //        "sldSiliconeRubber", CeramicWidth/2.0, CeramicHeight/2.0, siliconeRubberThickness/2.0);
        G4Box* sldSiliconeRubber = new G4Box(
                "sldSiliconeRubber", outputCsI[4], outputCsI[3], siliconeRubberThickness/2.0);
        G4LogicalVolume* logSiliconeRubber = new G4LogicalVolume(
                sldSiliconeRubber, siliconeRubber, "logSiliconeRubber", 0, 0, 0);

#ifndef NEW_GEOMETRY
        G4VPhysicalVolume* phySiliconeRubber = new G4PVPlacement(
                0, G4ThreeVector(-outputCsI[4], outputCsI[3], -siliconeRubberThickness/2.0),
                logSiliconeRubber, "phySiliconeRubber", logWorld, false, 0);
#endif /*NEW_GEOMETRY*/

        //epoxy
        //G4Box* sldEpoxy = new G4Box("sldEpoxy", CeramicWidth/2.0, CeramicHeight/2.0, GlassThickness/2.0);
        //G4Box* sldEpoxy = new G4Box(
        //        "sldEpoxy", CeramicWidth/2.0, CeramicHeight/2.0, EpoxyThickness/2.0);
        G4Box* sldEpoxy = new G4Box(
                "sldEpoxy", outputCsI[4], outputCsI[3], EpoxyThickness/2.0);
        G4LogicalVolume* logEpoxy = new G4LogicalVolume(sldEpoxy, Glass, "logEpoxy", 0, 0, 0);
        //G4VPhysicalVolume* phyEpoxy = new G4PVPlacement(
        //        0, G4ThreeVector(-outputCsI[4], outputCsI[3], -GlassThickness/2.0),
        //        logEpoxy, "phyEpoxy", logWorld, false, 0);

#ifndef NEW_GEOMETRY
        G4VPhysicalVolume* phyEpoxy = new G4PVPlacement(
                0, G4ThreeVector(
                    -outputCsI[4], outputCsI[3], -siliconeRubberThickness-EpoxyThickness/2.0),
                logEpoxy, "phyEpoxy", logWorld, false, 0);
#endif /*NEW_GEOMETRY*/

        //back esr
        //G4Box* sldEsrBackBulk = new G4Box(
        //        "sldEsrBackBulk", 
        //        outputCsI[4]+ESRThickness, 
        //        outputCsI[3]+ESRThickness, 
        //        ESRThickness/2.0);
        //G4Box* sldThickEpoxy = new G4Box(
        //        "sldThickEpoxy", CeramicWidth/2.0, CeramicHeight/2.0, 100.0*GlassThickness/2.0);
        //G4SubtractionSolid* sldEsrBack = new G4SubtractionSolid(
        //        "sldEsrBack", sldEsrBackBulk, sldThickEpoxy);
        //G4LogicalVolume* logEsrBack = new G4LogicalVolume(sldEsrBack, Mylar, "logEsrBack", 0, 0, 0);

        //G4VPhysicalVolume* phyEsrBack = new G4PVPlacement(
        //        0, G4ThreeVector(
        //            -outputCsI[4], 
        //            outputCsI[3], 
        //            -ESRThickness/2.0),
        //        logEsrBack, "phyEsrBack", logWorld, false, 0);

        //G4LogicalVolume* logEsrBackTopLayer = new G4LogicalVolume(
        //        sldEsrBack, Mylar, "logEsrBackTopLayer", 0, 0, 0);

        //G4VPhysicalVolume* phyEsrBackTopLayer = new G4PVPlacement(
        //        0, G4ThreeVector(
        //            -outputCsI[4], 
        //            outputCsI[3], 
        //            -ESRThickness*1.5),
        //        logEsrBackTopLayer, "phyEsrBackTopLayer", logWorld, false, 0);

        //apd
        //G4Box* sldApd = new G4Box("sldApd", CeramicWidth/2.0, CeramicHeight/2.0, PMTThickness/2.0);
        //G4Box* sldApd = new G4Box("sldApd", APDSide/2.0, APDSide/2.0, APDThickness/2.0);
        G4Box* sldApd = new G4Box("sldApd", outputCsI[4], outputCsI[3]/2.0, APDThickness/2.0);
        //G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, Air, "logApd", 0, 0, 0);
        G4LogicalVolume* logApd = new G4LogicalVolume(sldApd, silicon, "logApd", 0, 0, 0);
        //G4VPhysicalVolume* phyApd = new G4PVPlacement(
        //        0, G4ThreeVector(-outputCsI[4], outputCsI[3], -GlassThickness-PMTThickness/2.0),
        //        logApd, "phyApd", logWorld, false, 0);
        //G4ThreeVector posApd1 = G4ThreeVector(
        //        -outputCsI[4], 
        //        outputCsI[3]-APDSide/2.0-APDGap/2.0, 
        //        -siliconeRubberThickness-EpoxyThickness-APDThickness/2.0);
        //G4ThreeVector posApd2 = G4ThreeVector(
        //        -outputCsI[4], 
        //        outputCsI[3]+APDSide/2.0+APDGap/2.0, 
        //        -siliconeRubberThickness-EpoxyThickness-APDThickness/2.0);
        G4ThreeVector posApd1 = G4ThreeVector(
                -outputCsI[4], 
                outputCsI[3]-outputCsI[3]/2.0, 
                -siliconeRubberThickness-EpoxyThickness-APDThickness/2.0);
        G4ThreeVector posApd2 = G4ThreeVector(
                -outputCsI[4], 
                outputCsI[3]+outputCsI[3]/2.0+APDGap/2.0, 
                -siliconeRubberThickness-EpoxyThickness-APDThickness/2.0);

#ifndef NEW_GEOMETRY
        G4VPhysicalVolume* phyApd1 = new G4PVPlacement(
        		0, posApd1, logApd, "phyApd1", logWorld, false, 0);
        G4VPhysicalVolume* phyApd2 = new G4PVPlacement(
        		0, posApd2, logApd, "phyApd1", logWorld, false, 0);
#endif /*NEW_GEOMETRY*/
        //ceramic transition vectors
        G4ThreeVector transEpoxy(0, 0, CeramicThickness/2.0-EpoxyThickness/2.0);
        //G4ThreeVector transApd1(
        //        0, 
        //        APDSide/2.0+APDGap/2.0, 
        //        CeramicThickness/2.0-EpoxyThickness-APDThickness/2.0);
        //G4ThreeVector transApd2(
        //        0, 
        //        -APDSide/2.0-APDGap/2.0, 
        //        CeramicThickness/2.0-EpoxyThickness-APDThickness/2.0);
        G4ThreeVector transApd1(
                0, 
                outputCsI[3]/2.0, 
                CeramicThickness/2.0-EpoxyThickness-APDThickness/2.0);
        G4ThreeVector transApd2(
                0, 
                -outputCsI[3]/2.0, 
                CeramicThickness/2.0-EpoxyThickness-APDThickness/2.0);

        //ceramic geometry
        //G4Box* sldCeramicBulk = new G4Box(
        //        "sldCeramicBulk", CeramicWidth/2.0, CeramicHeight/2.0, CeramicThickness/2.0);
        G4Box* sldCeramicBulk = new G4Box(
                "sldCeramicBulk", outputCsI[4], outputCsI[3], CeramicThickness/2.0);
        G4SubtractionSolid* sldCeramicEpoxyOut = new G4SubtractionSolid(
                "sldCeramicEpoxyOut", sldCeramicBulk, sldEpoxy, 0, transEpoxy);
        G4SubtractionSolid* sldCeramic1ApdOut = new G4SubtractionSolid(
                "sldCeramic1ApdOut", sldCeramicEpoxyOut, sldApd, 0, transApd1);
        G4SubtractionSolid* sldCeramic2ApdOut = new G4SubtractionSolid(
                "sldCeramic2ApdOut", sldCeramic1ApdOut, sldApd, 0, transApd2);
        G4LogicalVolume* logCeramic2ApdOut = new G4LogicalVolume(
                sldCeramic2ApdOut, Ceramic, "logCeramic2ApdOut", 0, 0, 0);
#ifndef NEW_GEOMETRY

        G4VPhysicalVolume* phyCeramic2ApdOut = new G4PVPlacement(
                0, 
                G4ThreeVector(
                    -outputCsI[4], 
                    outputCsI[3], 
                    -siliconeRubberThickness-CeramicThickness/2.0),
                logCeramic2ApdOut, "phyCeramic2ApdOut", logWorld, false, 0);
#endif /*NEW_GEOMETRY*/

        //new G4LogicalSkinSurface("apdSurface", logApd, OpPmtSurface);
        new G4LogicalSkinSurface("apdSurface", logApd, opApdSurface);

        //new G4LogicalBorderSurface("esrBackSurface",     PhyCsI1,    phyEsrBack, OpESRSurface_polished);
        ////new G4LogicalBorderSurface("esrBackSurfaceBack", phyEsrBack, PhyCsI1,    OpESRSurface_polished_back);
        //new G4LogicalBorderSurface(
        //        "esrBackSurfaceTopLayer", phyEsrBack, phyEsrBackTopLayer, OpESRSurfaceTop_polished);
        ////new G4LogicalBorderSurface("epoxySurface", PhyCsI1, phyEpoxy, OpGLSurface_polished);

        logCeramic2ApdOut->SetVisAttributes(BWire);
        logEpoxy->SetVisAttributes(RBWire);
        //logEsrBack->SetVisAttributes(GWire);
        logApd->SetVisAttributes(YSolid);

        G4cout
            << ">>>> APD1 position: " << posApd1
            << "\n>>>> APD2 position: " << posApd2
            << G4endl;

#ifndef NEW_GEOMETRY
        phyEpoxy->CheckOverlaps();

        //phyEsrBack->CheckOverlaps();
        phyApd1->CheckOverlaps();
        phyApd2->CheckOverlaps();
        phyCeramic2ApdOut->CheckOverlaps();

#endif /*NEW_GEOMETRY*/

#ifndef NEW_GEOMETRY

        G4SDManager* SDman = G4SDManager::GetSDMpointer();// is G4manager class for sensitive detectors
#endif /*NEW_GEOMETRY*/
        if (!pmt_SD)
        {
            pmt_SD = new LcPMTSD("PMT1");
            SDman->AddNewDetector(pmt_SD);
        }
        logApd->SetSensitiveDetector(pmt_SD);

    }

#ifndef NEW_GEOMETRY
    G4SDManager* SDman = G4SDManager::GetSDMpointer();// is G4manager class for sensitive detectors

    if(!CsI_SD){//check if CsI_SD does not exists otherwise create it
          CsI_SD = new LcCsISD("CsI1");
          SDman->AddNewDetector(CsI_SD);//now we've created the SD so it exists(no doubt)
          logCsI1->SetSensitiveDetector(CsI_SD);
    }
#endif /*NEW_GEOMETRY*/
    logWorld->SetVisAttributes(BWire);

    //--------------------------------------------------------------------------

    //logCsI1 ->SetVisAttributes(Gr1Solid);
    logCsI1 ->SetVisAttributes(BWire);

#ifndef NOREFLECTOR
    //logESRFront ->SetVisAttributes(Gr1Solid);
    //logESRRight1 ->SetVisAttributes(Gr1Solid);
    //logESRBottom1 ->SetVisAttributes(Gr1Solid);
    //logESRLeft1 ->SetVisAttributes(Gr1Solid);
    //logESRTop1 ->SetVisAttributes(Gr1Solid);
    logESRFront->SetVisAttributes(GWire);
    logESRRight1->SetVisAttributes(GWire);
    logESRBottom1->SetVisAttributes(GWire);
    logESRLeft1->SetVisAttributes(GWire);
    logESRTop1->SetVisAttributes(GWire);
#endif /*NOREFLECTOR*/
#ifndef NEW_GEOMETRY
    PhyCsI1->CheckOverlaps();
#endif /*NEW_GEOMETRY*/
#ifndef NOREFLECTOR
    //PhyPmtgl->CheckOverlaps();
    //PhyPmt->CheckOverlaps();
    PhyESRFront->CheckOverlaps();
    //PhyESRRight1->CheckOverlaps();
    //PhyESRRight2->CheckOverlaps();
    //PhyESRBottom1->CheckOverlaps();
    //PhyESRBottom2->CheckOverlaps();
    PhyESRLeft1->CheckOverlaps();
    PhyESRLeft2->CheckOverlaps();
    //PhyESRTop1->CheckOverlaps();
    //PhyESRTop2->CheckOverlaps();
#endif /*NOREFLECTOR*/
#ifdef PHOTON_COUNTER
    phyPhotonCounter->CheckOverlaps();
#endif /*PHOTON_COUNTER*/

    return PhyWorld;
}

//-------------------------------------------------------------

void trapezeCsI(G4double *data, G4double *output)
{
    G4double Xmax = data[1]-((data[1]-data[0])/(data[5]-data[4]))*data[5];
    G4double Ymax = data[3]-((data[3]-data[2])/(data[5]-data[4]))*data[5];
    //vector linking the center of both rectangular surfaces
    G4double AB = sqrt(pow(((Xmax-data[1])/2.0),2.0)+pow(((Ymax-data[3])/2.0),2.0)+pow(data[5],2.0)); 
    G4double ABprim = sqrt(pow(((Xmax-data[1])/2.0),2.0)+pow(((Ymax-data[3])/2.0),2.0));

    G4double thetarad = acos(data[5]/AB) *rad;
    G4double theta = thetarad*180.0/M_PI*deg;

    G4double phirad = M_PI_2 + acos((Ymax-data[3])/(2.0*ABprim)) *rad;
    G4double phi = phirad*180.0/M_PI*deg;

    G4double xAB = (Xmax-data[1])/2.0;
    G4double yAB = (Ymax-data[3])/2.0;

    G4double xcenter = data[1]/2.0+xAB/2.0;
    G4double ycenter = data[3]/2.0+yAB/2.0;
    G4double zcenter = data[5]/2.0;

    output[0] = data[5]/2.0;
    output[1] = theta;
    output[2] = phi;
    output[3] = Ymax/2.0;
    output[4] = Xmax/2.0;
    output[5] = data[3]/2.0;
    output[6] = data[1]/2.0;
    output[7] = xcenter;
    output[8] = ycenter;
    output[9] = zcenter;
}

//------------------------------------------------------------------

void trapezeLeft(G4double *dataCsI, G4double *output, G4double ESRThickness, G4double cut)
{
    G4double outputCsI[10] = {0.0};
    trapezeCsI(dataCsI, outputCsI);// = outputCsI

    G4double length = sqrt(pow((outputCsI[4]*2.0-dataCsI[1]),2.0)+pow(dataCsI[5],2.0)); 
    G4double slopex = (dataCsI[1]-dataCsI[0])/(dataCsI[5]-dataCsI[4]);
    G4double slopey = (dataCsI[3]-dataCsI[2])/(dataCsI[5]-dataCsI[4]);

    G4double dataL1[6] = {
        ESRThickness, ESRThickness, dataCsI[3]-slopey*cut, dataCsI[3], 0.0, cut*length/dataCsI[5]
    };
    G4double outputl[10] = {0.0};
    trapezeCsI(dataL1, outputl);
    G4int i;
    for (i=0; i <= 9; i++){
        output[i] = outputl[i];
    }
    output[10] = -acos(dataCsI[5]/length);
    output[11] = -(dataCsI[1]-slopex*cut/2.0);
    output[12] = -(dataCsI[1]-slopex*(cut+(dataCsI[5]-cut)/2.0));
    output[13] = length-2.0*output[0];
}

//-------------------------------------------------------------

void trapezeTop(G4double *dataCsI, G4double *output, G4double ESRThickness, G4double cut)
{
    G4double outputCsI[10] = {0.0};
    trapezeCsI(dataCsI, outputCsI);// = outputCsI

    G4double length = sqrt(pow((outputCsI[3]*2.0-outputCsI[5]*2.0),2.0)+pow(dataCsI[5],2.0)); 
    G4double slopex = (dataCsI[1]-dataCsI[0])/(dataCsI[5]-dataCsI[4]);
    G4double slopey = (dataCsI[3]-dataCsI[2])/(dataCsI[5]-dataCsI[4]);

    G4double dataL1[6] = {
        dataCsI[1]-slopex*cut, dataCsI[1], ESRThickness, ESRThickness, 0.0, cut*length/dataCsI[5]
    };
    G4double outputt[10] = {0.0};
    trapezeCsI(dataL1, outputt);
    G4int i;
    for (i=0; i <= 9; i++){
        output[i] = outputt[i];
    }
    output[10] = -acos(dataCsI[5]/length);
    output[11] = 2.0*outputCsI[5]-slopey*(cut/2.0);
    output[12] = 2.0*outputCsI[3]+slopey*(outputCsI[0]-cut/2.0);
    output[13] = length-2.0*output[0];
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
