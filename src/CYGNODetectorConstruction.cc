#include <iostream>
#include <string.h>
#include <fstream> 

// GEANT4 //
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
//#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4Sphere.hh"
#include "G4EllipticalTube.hh"
#include "G4Ellipsoid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4GeometryManager.hh"
#include "G4RunManager.hh"

#include "G4UserLimits.hh"

#include "G4RotationMatrix.hh"

// USER //
#include "CYGNODetectorConstruction.hh"
#include "CYGNODetectorConstructionMessenger.hh"
#include "CYGNODetectorLNGS.hh"
#include "CYGNODetectorMaterial.hh"
#include "CYGNODetectorProperty.hh"
#include "CYGNOSensitiveDetector.hh"
#include "CYGNOVolumes.hh"
#include "CADMesh.hh"
//#include "CYGNOBiasMultiParticleChangeCrossSection.hh"

CYGNODetectorConstruction::CYGNODetectorConstruction() :
   CYGNOGeomPath("../geometry_ASCII/lime_new"),
   rockThicknessOuter(-999*m),
   rockThicknessInner(-999*m),
   //rockThicknessInner(4.*m),
   //productionLayerThickness(2.5*m),
   productionLayerThickness(-999*m),
   CYGNOLab("NoCave"),
   //CYGNOLab("LNGS"),
   //CYGNOLab("MuonLNGS"),
   CYGNOShielding("LIMEShield"),
   //CYGNOShielding("AmBe"),
   //CYGNOShielding("FullShield"),
  //  CYGNOShielding("NoShield"),
   thick0(0.90*m), thick1(0.40*m), thick2(0.20*m), thick3(0.05*m), 
   Mat0("Water"), Mat1("PE"), Mat2("Pb"), Mat3("Cu")
{
	
     fMessenger = new CYGNODetectorConstructionMessenger(this);
}

CYGNODetectorConstruction::~CYGNODetectorConstruction()
{
	delete fMessenger;

}

G4VPhysicalVolume* CYGNODetectorConstruction::Construct()
{
   
  //UpdateGeometryPath(CYGNOGeomPath); 

  G4NistManager * nist_manager = G4NistManager::Instance();
  
  //-----------------------------
  // construction of materials
  //-----------------------------
  G4cout << "Constructing materials..." << G4endl;

  //G4Material * air = nist_manager->FindOrBuildMaterial("G4_AIR");
  //G4Material * water = nist_manager->FindOrBuildMaterial("G4_WATER");
  CYGNODetectorMaterial* CYGNOMaterials = CYGNODetectorMaterial::GetInstance();
  G4cout << "... done" << G4endl;

  ////-----------------------------
  //// construction of general properties
  ////-----------------------------
  //G4cout << "Constructing general properties...";
  //CYGNODetectorProperty* CYGNOProperties = CYGNODetectorProperty::GetInstance();
  //G4cout << "... done" << G4endl;

  //**********************************************************************
  //   DEFINITION OF THE GEOMETRY
  //**********************************************************************
      
  //INITIALIZING TRANSLATION VECTORS TO 0:
  Rock_log = 0;
  size_Rock = G4ThreeVector();
  tr_Rock = G4ThreeVector();
  rot_Rock = G4RotationMatrix();
  absrot_Rock = G4RotationMatrix();
  Laboratory_log = 0;
  size_Laboratory = G4ThreeVector();
  tr_Laboratory = G4ThreeVector();
  rot_Laboratory = G4RotationMatrix();
  Shielding_log = 0;
  size_Shielding = G4ThreeVector();
  tr_Shielding = G4ThreeVector();
  rot_Shielding = G4RotationMatrix();
  absrot_Shielding = G4RotationMatrix();
  InsideVolume_log = 0;
  size_InsideVolume = G4ThreeVector();
  tr_InsideVolume = G4ThreeVector();
  rot_InsideVolume = G4RotationMatrix();

  //Name of the volumes
  G4String name_solid="";
  G4String name_log="";
  G4String name_phys="";

  G4ThreeVector tr;
  G4RotationMatrix rot;

  //**********************************************************************
  // WORLD ***************************************
  //**********************************************************************
  G4double world_x = 100.0*m;
  G4double world_y = 100.0*m;
  G4double world_z = 400.0*m;
      
  name_phys="WorldVolume";
  name_log=name_phys+"_log";
  name_solid=name_phys+"_solid";
  G4Box* WorldVolume_box = new G4Box(name_solid,0.5*world_x,0.5*world_y,0.5*world_z);
  WorldVolume_log = new G4LogicalVolume(WorldVolume_box,CYGNOMaterials->Material("Vacuum"),name_log,0,0,0);
  //THE WORLD CANNOT BE TRANSLATED, therefore the first volume inside it (Rock_log) is the volume that must be translated in order to adjust the origin. Rock_log cannot be as large as the world to avoid that this volume is moved out of the world
  WorldVolume_phys = new G4PVPlacement(0,G4ThreeVector(),WorldVolume_log,name_phys,0,false,0,true);//The world volume cannot be translated
        
  //These variables are used to set the thin tube for depth studies in the externalRock_log
  G4double rockdist_z;
  G4double rockdepth_z;
        
  //**********************************************************************
  // LABORATORY ***************************************
  //**********************************************************************
  G4cout << "Constructing laboratory..." << G4endl;
  G4bool isThinTubeCompatible=false;
  // ---------------------------------- LNGS
  if (CYGNOLab == "LNGS")
  {
    /////////////////////////////////////////////////////////
    // In this configuration the translations are wrong, WIP
    // /////////////////////////////////////////////////////
    G4cout << "========== WARNING =============" << G4endl;
    G4cout << "Translation vectors in this configuration ar wrong, WIP, use 'NoCave'" << G4endl;
    G4cout << "================================" << G4endl;
    CYGNODetectorLNGS* LNGS = CYGNODetectorLNGS::GetInstance();
    if (rockThicknessOuter != -999*m)
          LNGS->SetExternalRockThickness(rockThicknessOuter);
    if (productionLayerThickness != -999*m)
          LNGS->SetProductionRockThickness(productionLayerThickness);
    if (rockThicknessInner != -999*m)
          LNGS->SetInternalRockThickness(rockThicknessInner);	  
      
    LNGS->SetDetectorMaterial(CYGNOMaterials);
    LNGS->ConstructRock();
    Rock_log=LNGS->GetRock();
    size_Rock=LNGS->GetRockSizeXYZ();
    absrot_Rock=LNGS->GetRockAbsRotation();
    Laboratory_log=LNGS->GetLaboratory();
    size_Laboratory=LNGS->GetLaboratorySizeXYZ();
    tr_Laboratory=LNGS->GetLaboratoryTranslation();
    rot_Laboratory=LNGS->GetLaboratoryRotation();
      
    ////for thin tube
    //if(LNGS->GetProductionRockThickness()==0.*cm && LNGS->GetInternalRockThickness()==0.*cm)
    //  isThinTubeCompatible=true;
    //rockdist_z=LNGS->rockdist_z;
    //rockdepth_z=LNGS->rockdepth_z;
  }
  // ---------------------------------- NoCave
  else if (CYGNOLab == "NoCave") 
  {
    //**********************************************************************
    // Double Air sphere surrounding the whole detector and shielding
    //**********************************************************************        
    G4double airInnerRadius = 25.0*m;
    G4double airThickness = 20.0*m;
    if (productionLayerThickness != -999*m)
    airThickness = productionLayerThickness;
    if (rockThicknessInner != -999*m)
    airInnerRadius=rockThicknessInner;

    //Air permeates the are around the detector
    name_phys="OuterAirSphere";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Sphere* OuterAirSphere = new G4Sphere(name_solid, 0., airInnerRadius+airThickness, 0*degree, 360*degree, 0*degree,180*degree);
    G4LogicalVolume* OuterAirSphere_log = new G4LogicalVolume(OuterAirSphere,CYGNOMaterials->Material("Air"),name_log);
    Rock_log=OuterAirSphere_log;
    size_Rock=G4ThreeVector(airInnerRadius+airThickness,airInnerRadius+airThickness,airInnerRadius+airThickness);
    absrot_Rock = G4RotationMatrix();
    
    name_phys="InnerAirSphere";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Sphere* InnerAirSphere = new G4Sphere(name_solid,  0., airInnerRadius,  0*degree, 360*degree, 0*degree,180*degree);
    G4LogicalVolume* InnerAirSphere_log = new G4LogicalVolume(InnerAirSphere, CYGNOMaterials->Material("Air"), name_log);
    Laboratory_log=InnerAirSphere_log;
    size_Laboratory=G4ThreeVector(airInnerRadius,airInnerRadius,airInnerRadius);
    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_Laboratory+=(rot_Laboratory*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_Laboratory*=rot; //equivalent to rot_Laboratory=rot_Laboratory*rot
    InnerAirSphere_phys = new G4PVPlacement(G4Transform3D(rot,tr),InnerAirSphere_log,name_phys,OuterAirSphere_log,false,0,true);        
  }
  else if (CYGNOLab == "MuonLNGS")
  {
    //**********************************************************************
    // Air parallelepiped inside a parallelepiped made of rock
    //**********************************************************************        
    G4double airInnerRadius = 3.0*m; //3.0*m for Muon simulation
    G4double airThickness = 5*cm; //5.0*m for Muon simulation
    if (productionLayerThickness != -999*m)
    airThickness = productionLayerThickness;
    if (rockThicknessInner != -999*m)
    airInnerRadius=rockThicknessInner;

    //Air permeates the are around the detector
    name_phys="OuterAirSphere";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    //G4Sphere* OuterAirSphere = new G4Sphere(name_solid, 0., airInnerRadius+airThickness, 0*degree, 360*degree, 0*degree,180*degree); 
    
    //LNGS rock:
    G4Box* OuterAirSphere = new G4Box(name_solid,(airInnerRadius+airThickness)/2.,(airInnerRadius+airThickness)/2.,(airInnerRadius+airThickness)/2.);
    G4LogicalVolume* OuterAirSphere_log = new G4LogicalVolume(OuterAirSphere,CYGNOMaterials->Material("LNGSRock"),name_log); //LNGS rock
    Rock_log=OuterAirSphere_log;
    size_Rock=G4ThreeVector(airInnerRadius+airThickness,airInnerRadius+airThickness,airInnerRadius+airThickness); //seems it's not used
    absrot_Rock = G4RotationMatrix();
    
    name_phys="InnerAirSphere";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* InnerAirSphere = new G4Box(name_solid,airInnerRadius/2.,airInnerRadius/2.,airInnerRadius/2.);
    G4LogicalVolume* InnerAirSphere_log = new G4LogicalVolume(InnerAirSphere, CYGNOMaterials->Material("Air"), name_log);
    Laboratory_log=InnerAirSphere_log;
    size_Laboratory=G4ThreeVector(airInnerRadius,airInnerRadius,airInnerRadius);
    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_Laboratory+=(rot_Laboratory*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_Laboratory*=rot; //equivalent to rot_Laboratory=rot_Laboratory*rot
    InnerAirSphere_phys = new G4PVPlacement(G4Transform3D(rot,tr),InnerAirSphere_log,name_phys,OuterAirSphere_log,false,0,true);
  }
  else
  {
    G4cout << "ERROR: Something went wrong with the definition of the variable CYGNOLab" << G4endl;
    //throw std::exception();
    exit(1);		
  }


  G4cout << "Laboratory done." << G4endl;

  //**********************************************************************
  // SHIELDING **********************************************************
  
  //some variables:
  G4double x_ambe_shield;
  G4double y_ambe_shield;
  G4double z_ambe_shield;
  G4double z_ambe_pb_shield;
  G4RotationMatrix* rotambeshield;
  G4ThreeVector trambeshield;
  G4double z_ambe_capsule;
  G4RotationMatrix *rot_room_LNGS;
  G4ThreeVector tr_room_LNGS;
        
  if (CYGNOShielding == "FullShield") 
  {
    G4cout<<"Shielding Construction Started"<<G4endl;
    
    if (thick3==-999*m || thick2==-999*m || thick1==-999*m || thick0==-999*m || Mat0=="" || Mat1=="" || Mat2=="" || Mat3=="") 
    {
      G4cout << "ERROR: Please specify the thickennses and the materials of the shielding layers if you want to use the shielding design: FullShield" << G4endl;
      //throw std::exception();
      exit(1);
    }

    // ----------------------------------- Inner room dimensions
    G4double AirBox_x;
    G4double AirBox_y;
    G4double AirBox_z;
    G4Box* AirBox;
    AirBox_x = 1.2*m;  //1.2*m; //cygno 2.65*m; lime 2.0*m; inner cu shield 1.2*m; inner water shield 1.8*m;
    AirBox_y = 0.7*m;  //0.7*m; //cygno 1.45*m; lime 0.8*m; inner cu shield 0.7*m; inner water shield 1.0*m;
    AirBox_z = 0.6*m;  //0.6*m; //cygno 1.45*m; lime 0.8*m; inner cu shield 0.6*m; inner water shield 1.0*m;
    tr_InsideVolume = G4ThreeVector(0.,0.,0.);
    rot_InsideVolume = G4RotationMatrix();		
    size_InsideVolume = G4ThreeVector(AirBox_x/2.,
                          AirBox_y/2.,
                          AirBox_z/2.);		
    size_Shielding = G4ThreeVector(AirBox_x/2. + thick3 + thick2 + thick1 + thick0,
                          AirBox_y/2. + thick3 + thick2 + thick1 + thick0,
                          AirBox_z/2. + thick3 + thick2 + thick1 + thick0);
    absrot_Shielding = G4RotationMatrix();

    // ----------------------------------- Shield 0
    G4double Shield0_x = AirBox_x + 2.*thick3 + 2.*thick2 + 2.*thick1 + 2.*thick0 ;
    G4double Shield0_y = AirBox_y + 2.*thick3 + 2.*thick2 + 2.*thick1 + 2.*thick0 ;
    G4double Shield0_z = AirBox_z + 2.*thick3 + 2.*thick2 + 2.*thick1 + 2.*thick0 ;        
    G4Material* Shield0Mat = CYGNOMaterials->Material(Mat0);
    name_phys="Shield0";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield0 = new G4Box(name_solid,0.5*Shield0_x,0.5*Shield0_y,0.5*Shield0_z);
    Shield0_log = new G4LogicalVolume(Shield0,Shield0Mat,name_log);
    Shielding_log = Shield0_log;
    Shield0_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat0));

    // ----------------------------------- Shield 1
    G4double Shield1_x = AirBox_x + 2.*thick3 + 2.*thick2 + 2.*thick1 ;
    G4double Shield1_y = AirBox_y + 2.*thick3 + 2.*thick2 + 2.*thick1 ;
    G4double Shield1_z = AirBox_z + 2.*thick3 + 2.*thick2 + 2.*thick1 ;
    G4Material* Shield1Mat = CYGNOMaterials->Material(Mat1);
    name_phys="Shield1";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield1 = new G4Box(name_solid,0.5*Shield1_x,0.5*Shield1_y,0.5*Shield1_z);
    Shield1_log = new G4LogicalVolume(Shield1,Shield1Mat,name_log);
    Shield1_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat1));
      
    // ----------------------------------- Shield 2        
    G4double Shield2_x = AirBox_x + 2.*thick3 + 2.*thick2 ;
    G4double Shield2_y = AirBox_y + 2.*thick3 + 2.*thick2 ;
    G4double Shield2_z = AirBox_z + 2.*thick3 + 2.*thick2 ;
    G4Material* Shield2Mat = CYGNOMaterials->Material(Mat2);
    name_phys="Shield2";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield2 = new G4Box(name_solid,0.5*Shield2_x,0.5*Shield2_y,0.5*Shield2_z);
    Shield2_log = new G4LogicalVolume(Shield2,Shield2Mat,name_log);
    Shield2_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat2));
      
    // ----------------------------------- Shield 3        
    G4double Shield3_x = AirBox_x + 2.*thick3 ;
    G4double Shield3_y = AirBox_y + 2.*thick3 ;
    G4double Shield3_z = AirBox_z + 2.*thick3 ;
    G4Material* Shield3Mat = CYGNOMaterials->Material(Mat3);
    name_phys="Shield3";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield3 = new G4Box(name_solid,0.5*Shield3_x,0.5*Shield3_y,0.5*Shield3_z);
    Shield3_log = new G4LogicalVolume(Shield3,Shield3Mat,name_log);
    Shield3_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat3));
    
    // ----------------------------------- Airbox
    name_phys="AirBox";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    AirBox = new G4Box(name_solid,0.5*AirBox_x,0.5*AirBox_y,0.5*AirBox_z);
    AirBox_log = new G4LogicalVolume(AirBox,CYGNOMaterials->Material("Air"),name_log);
    AirBox_log->SetVisAttributes(CYGNOMaterials->VisAttributes("Air"));
    InsideVolume_log = AirBox_log;
  
  }
  // ---------------------------------- No shielding
  else if (CYGNOShielding == "NoShield") 
  {
    G4double AirBox_x;
    G4double AirBox_y;
    G4double AirBox_z;
    G4Box* AirBox;
    AirBox_x = 5.*m;
    AirBox_y = 3.*m;
    AirBox_z = 3.*m;       
    name_phys="AirBox";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    AirBox = new G4Box(name_solid,0.5*AirBox_x,0.5*AirBox_y,0.5*AirBox_z);
    AirBox_log = new G4LogicalVolume(AirBox,CYGNOMaterials->Material("Air"),name_log,0,0,0);

    Shielding_log=AirBox_log;
    size_Shielding=G4ThreeVector(AirBox_x/2.,AirBox_y/2.,AirBox_z/2.);
    absrot_Shielding = G4RotationMatrix();
    size_InsideVolume=G4ThreeVector(AirBox_x/2.,AirBox_y/2.,AirBox_z/2.);
    tr_InsideVolume=G4ThreeVector(0.,0.,0.);
    rot_InsideVolume=G4RotationMatrix();
  }
  else if (CYGNOShielding == "LIMEShield") 
  {
    // ----------------------------------- Inner room dimensions
    G4double AirBox_x;
    G4double AirBox_y;
    G4double AirBox_z;
    G4Box* AirBox;
    AirBox_x = 1.1*m; //default: 1.1m,0.6m,0.55m; 
    AirBox_y = 0.6*m;
    AirBox_z = 0.55*m;

    tr_InsideVolume = G4ThreeVector(0.,0.,0.);
    rot_InsideVolume = G4RotationMatrix();		
    absrot_Shielding = G4RotationMatrix();

    // ----------------------------------- Shield 0
    G4double Shield0_x = 2.84*m ;
    G4double Shield0_y = 2.04*m ;
    G4double Shield0_z = 2.14*m ;        
    G4Material* Shield0Mat = CYGNOMaterials->Material(Mat0);
    name_phys="Shield0";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield0 = new G4Box(name_solid,0.5*Shield0_x,0.5*Shield0_y,0.5*Shield0_z);
    Shield0_log = new G4LogicalVolume(Shield0,Shield0Mat,name_log);
    Shielding_log = Shield0_log;
    Shield0_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat0));

    // ----------------------------------- Shield 1
    G4double Shield1_x = 2.82*m ;
    G4double Shield1_y = 2.02*m ;
    G4double Shield1_z = 2.12*m ;
    G4Material* Shield1Mat = CYGNOMaterials->Material(Mat1);
    name_phys="Shield1";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield1 = new G4Box(name_solid,0.5*Shield1_x,0.5*Shield1_y,0.5*Shield1_z);
    Shield1_log = new G4LogicalVolume(Shield1,Shield1Mat,name_log);
    Shield1_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat1));
    
    // ----------------------------------- Shield 2        
    G4double Shield2_x = 2.8*m; //default configuration: 2.8m, 2.0m, 2.1m;
    G4double Shield2_y = 2.0*m;
    G4double Shield2_z = 2.1*m;
    G4Material* Shield2Mat = CYGNOMaterials->Material(Mat2);
    name_phys="Shield2";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield2 = new G4Box(name_solid,0.5*Shield2_x,0.5*Shield2_y,0.5*Shield2_z);
    Shield2_log = new G4LogicalVolume(Shield2,Shield2Mat,name_log);
    Shield2_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat2));
    
    // ----------------------------------- Shield 3        
    G4double Shield3_x =  1.7*m; //per incapsulare la camera 1.7m,0.95m,0.95m; per l'AmBe (il setup comprensivo dello shielding lo definisco figlio dello shield3): 2.78m,1.98m,2.08m; 
    G4double Shield3_y =  0.95*m;
    G4double Shield3_z =  0.95*m;
    G4Material* Shield3Mat = CYGNOMaterials->Material(Mat3);
    name_phys="Shield3";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield3 = new G4Box(name_solid,0.5*Shield3_x,0.5*Shield3_y,0.5*Shield3_z);
    Shield3_log = new G4LogicalVolume(Shield3,Shield3Mat,name_log);
    Shield3_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat3));
    
    // ----------------------------------- Airbox
    name_phys="AirBox";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    AirBox = new G4Box(name_solid,0.5*AirBox_x,0.5*AirBox_y,0.5*AirBox_z);
    AirBox_log = new G4LogicalVolume(AirBox,CYGNOMaterials->Material("Air"),name_log);
    AirBox_log->SetVisAttributes(CYGNOMaterials->VisAttributes("Air"));
    InsideVolume_log = AirBox_log;

  }
  else if (CYGNOShielding == "AmBe")
  {
    // ----------------------------------- Inner room dimensions
    G4double AirBox_x;
    G4double AirBox_y;
    G4double AirBox_z;
    G4Box* AirBox;
    AirBox_x = 1.1*m; //default: 1.1m,0.6m,0.55m; 
    AirBox_y = 0.6*m;
    AirBox_z = 0.55*m;

    tr_InsideVolume = G4ThreeVector(0.,0.,0.);
    rot_InsideVolume = G4RotationMatrix();		
    absrot_Shielding = G4RotationMatrix();

    // ----------------------------------- Shield 0
    G4double Shield0_x = 4.321*m ;//for old AmBe 3.84, 3.04, 3.14
    G4double Shield0_y = 2.126*m ;
    G4double Shield0_z = 2.316*m ;        
    G4Material* Shield0Mat = CYGNOMaterials->Material(Mat0);
    name_phys="Shield0";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield0 = new G4Box(name_solid,0.5*Shield0_x,0.5*Shield0_y,0.5*Shield0_z);
    Shield0_log = new G4LogicalVolume(Shield0,Shield0Mat,name_log);
    Shielding_log = Shield0_log;
    Shield0_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat0));

    // ----------------------------------- Shield 1
    G4double Shield1_x = 4.316*m ;//for old AmBe 3.82, 3.02, 3.12
    G4double Shield1_y = 2.121*m ;
    G4double Shield1_z = 2.311*m ;
    G4Material* Shield1Mat = CYGNOMaterials->Material(Mat1);
    name_phys="Shield1";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield1 = new G4Box(name_solid,0.5*Shield1_x,0.5*Shield1_y,0.5*Shield1_z);
    Shield1_log = new G4LogicalVolume(Shield1,Shield1Mat,name_log);
    Shield1_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat1));
      
    // ----------------------------------- Shield 2        
    G4double Shield2_x = 4.311*m; //default configuration: 2.8m, 2.0m, 2.1m; for old AmBe 3.8, 3.0, 3.1
    G4double Shield2_y = 2.116*m;
    G4double Shield2_z = 2.306*m;
    G4Material* Shield2Mat = CYGNOMaterials->Material(Mat2);
    name_phys="Shield2";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield2 = new G4Box(name_solid,0.5*Shield2_x,0.5*Shield2_y,0.5*Shield2_z);
    Shield2_log = new G4LogicalVolume(Shield2,Shield2Mat,name_log);
    Shield2_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat2));
      
    // ----------------------------------- Shield 3        
    G4double Shield3_x =  4.24*m; //per incapsulare la camera 1.7m,0.95m,0.95m; per l'AmBe (il setup comprensivo dello shielding lo definisco figlio dello shield3): 2.78m,1.98m,2.08m; 
    G4double Shield3_y =  2.045*m;
    G4double Shield3_z =  2.235*m;
    G4Material* Shield3Mat = CYGNOMaterials->Material(Mat3);
    name_phys="Shield3";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* Shield3 = new G4Box(name_solid,0.5*Shield3_x,0.5*Shield3_y,0.5*Shield3_z);
    Shield3_log = new G4LogicalVolume(Shield3,Shield3Mat,name_log);
    Shield3_log->SetVisAttributes(CYGNOMaterials->VisAttributes(Mat3));
      
    // ----------------------------------- Airbox
    name_phys="AirBox";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    AirBox = new G4Box(name_solid,0.5*AirBox_x,0.5*AirBox_y,0.5*AirBox_z);
    AirBox_log = new G4LogicalVolume(AirBox,CYGNOMaterials->Material("Air"),name_log);
    AirBox_log->SetVisAttributes(CYGNOMaterials->VisAttributes("Air"));
    InsideVolume_log = AirBox_log;
    
    //AmBe simulation
    
    //AmBe capsule and source
    G4double diam_ambe_capsule = 6.4*mm;
    z_ambe_capsule = 17.6*mm;
    G4Tubs* ambe_capsule = new G4Tubs("ambe_capsule_solid",0.,0.5*diam_ambe_capsule,0.5*z_ambe_capsule,0.*deg,360.*deg);
    
    G4double diam_ambe_source = 4.*mm;
    G4double z_ambe_source = 4.*mm;
    G4Tubs* ambe_source = new G4Tubs("ambe_source_solid",0.,0.5*diam_ambe_source,0.5*z_ambe_source,0.*deg,360.*deg);
    
    ambe_capsule_log = new G4LogicalVolume(ambe_capsule,CYGNOMaterials->Material("Steel"),"ambe_capsule_log",0,0,0);
    ambe_source_log = new G4LogicalVolume(ambe_source,CYGNOMaterials->Material("AmBe"),"ambe_source_log",0,0,0);
    
    
    //AmBe lead shielding
    G4double x_ambe_pb_shield = 20*cm;
    G4double y_ambe_pb_shield = 10*cm;
    z_ambe_pb_shield = 10*cm;    //10cm per due panetti, 5cm per un panetto
    G4Box* Pb_shield = new G4Box("Pb_shield",0.5*x_ambe_pb_shield,0.5*y_ambe_pb_shield,0.5*z_ambe_pb_shield);
    ambe_pb_shield_log = new G4LogicalVolume(Pb_shield,CYGNOMaterials->Material("Pb"),"ambe_pb_shield_log",0,0,0);
    
    
    //AmBe PE shielding
    x_ambe_shield = 100*cm; //original: 100,50,50; second config: 100,60,60
    y_ambe_shield = 50*cm;
    z_ambe_shield = 50*cm;
    //x_ambe_shield = 100*cm; 
    //y_ambe_shield = 60*cm;
    //z_ambe_shield = 60*cm;
    G4Box* PE_shield = new G4Box("PE_shield",0.5*x_ambe_shield,0.5*y_ambe_shield,0.5*z_ambe_shield);
    G4Box* Pb_hole = new G4Box("Pb_hole",0.5*x_ambe_pb_shield,0.5*y_ambe_pb_shield,0.5*z_ambe_pb_shield);
    G4Tubs* ambe_hole = new G4Tubs("ambe_hole",0.,0.5*diam_ambe_capsule+2.*mm,0.5*z_ambe_capsule+2.*mm,0.*deg,360.*deg);
    rotambeshield = new G4RotationMatrix();

    //decomment for the original PE shield
    G4Box* PE_back = new G4Box("PE_box",50*cm, 5*cm, 5*cm);
    trambeshield = G4ThreeVector(0.,0.,-0.5*z_ambe_shield-5.*cm);
    G4UnionSolid* ambe_shield_back = new G4UnionSolid("ambe_shield_back",PE_shield,PE_back,rotambeshield,trambeshield); //solid box + the block on the back
    //
    
    trambeshield = G4ThreeVector(0.,0.,0.5*z_ambe_shield-0.5*z_ambe_pb_shield);
    G4SubtractionSolid* ambe_shield0 = new G4SubtractionSolid("ambe_shield0",ambe_shield_back,Pb_hole,rotambeshield,trambeshield); //de-comment and comment next line for the original PE shield
    //G4SubtractionSolid* ambe_shield0 = new G4SubtractionSolid("ambe_shield0",PE_shield,Pb_hole,rotambeshield,trambeshield); //
    trambeshield = G4ThreeVector(0.,0.,0.5*z_ambe_shield-z_ambe_pb_shield-0.5*z_ambe_capsule-2*mm);
    G4SubtractionSolid* ambe_shield = new G4SubtractionSolid("ambe_shield",ambe_shield0,ambe_hole,rotambeshield,trambeshield);
    
    ambe_shield_log = new G4LogicalVolume(ambe_shield,CYGNOMaterials->Material("PE"),"ambe_shield_log",0,0,0);
    
    //Plastic room
    //h=2050mm lungo y, lungo z 2240mm, 4245 lungo x. 1cm policarbonato, 1mm Al, 5cm poliuretano, 1mm Al
    G4double x_roomLNGS = 4245*mm;
    G4double y_roomLNGS = 2050*mm;
    G4double z_roomLNGS = 2240*mm;
    G4double PC_wall_thickness = 10*mm; //polycarbonate layer of the wall
    G4double PU_wall_thickness = 50*mm; //polyurethane foam layer of the wall
    G4double Al_wall_thickness = 1.*mm; //aluminium layer of the wall
    
    G4Box* roomLNGS = new G4Box("roomLNGS", 0.5*(x_roomLNGS), 0.5*(y_roomLNGS), 0.5*(z_roomLNGS)); //inner wall
    G4Box* PC_roomLNGS = new G4Box("Polycarbonate_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness));
    G4Box* Al_roomLNGS = new G4Box("Al_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness));
    G4Box* PU_roomLNGS = new G4Box("Polyurethane_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness));
    G4Box* Al_ext_roomLNGS = new G4Box("Polyurethane_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness));    
    tr_room_LNGS = G4ThreeVector(0.,0.,0.);
    rot_room_LNGS = new G4RotationMatrix();
    G4SubtractionSolid* PC_wallLNGS = new G4SubtractionSolid("PC_wallLNGS",PC_roomLNGS,roomLNGS,rot_room_LNGS,tr_room_LNGS);
    G4SubtractionSolid* Al_wallLNGS = new G4SubtractionSolid("Al_wallLNGS",Al_roomLNGS,PC_roomLNGS,rot_room_LNGS,tr_room_LNGS);
    G4SubtractionSolid* PU_wallLNGS = new G4SubtractionSolid("PU_wallLNGS",PU_roomLNGS,Al_roomLNGS,rot_room_LNGS,tr_room_LNGS);
    G4SubtractionSolid* Al_ext_wallLNGS = new G4SubtractionSolid("Al_ext_wallLNGS",Al_ext_roomLNGS,PU_roomLNGS,rot_room_LNGS,tr_room_LNGS);
    
    //tr_room_LNGS = G4ThreeVector(131.*mm,(0.5*y_roomLNGS-757.*mm),-100.*mm);
    
    PC_wallLNGS_log = new G4LogicalVolume(PC_wallLNGS,CYGNOMaterials->Material("PC"),"PC_wallLNGS_log",0,0,0);
    Al_wallLNGS_log = new G4LogicalVolume(Al_wallLNGS,CYGNOMaterials->Material("Al"),"Al_wallLNGS_log",0,0,0);
    PU_wallLNGS_log = new G4LogicalVolume(PU_wallLNGS,CYGNOMaterials->Material("PU_foam"),"PU_wallLNGS_log",0,0,0);
    Al_ext_wallLNGS_log = new G4LogicalVolume(Al_ext_wallLNGS,CYGNOMaterials->Material("Al"),"Al_ext_wallLNGS_log",0,0,0);
  
    //LIME PE base	
    G4double x_LIME_base = 1920*mm;
    G4double y_LIME_base = 400*mm;
    G4double z_LIME_base = 840*mm;
    G4Box* LIME_base = new G4Box("LIME_base", 0.5*x_LIME_base, 0.5*y_LIME_base, 0.5*z_LIME_base);
    LIME_base_log = new G4LogicalVolume(LIME_base,CYGNOMaterials->Material("PE"),"LIME_base_log",0,0,0);
  
    //Volumes for control room, DAMA and the tunnel
  
    //control room
    G4Box* CR_roomLNGS = new G4Box("roomLNGS", 0.5*(x_roomLNGS), 0.5*(y_roomLNGS), 0.5*(z_roomLNGS)); //inner wall
    G4Box* CR_PC_roomLNGS = new G4Box("Polycarbonate_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness));
    G4Box* CR_Al_roomLNGS = new G4Box("Al_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness));
    G4Box* CR_PU_roomLNGS = new G4Box("Polyurethane_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness));
    G4Box* CR_Al_ext_roomLNGS = new G4Box("Polyurethane_roomLNGS",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness));    
    tr_room_LNGS = G4ThreeVector(0.,0.,0.);
    rot_room_LNGS = new G4RotationMatrix();
    G4SubtractionSolid* CR_PC_wallLNGS = new G4SubtractionSolid("CR_PC_wallLNGS",CR_PC_roomLNGS,CR_roomLNGS,rot_room_LNGS,tr_room_LNGS);
    G4SubtractionSolid* CR_Al_wallLNGS = new G4SubtractionSolid("CR_Al_wallLNGS",CR_Al_roomLNGS,CR_PC_roomLNGS,rot_room_LNGS,tr_room_LNGS);
    G4SubtractionSolid* CR_PU_wallLNGS = new G4SubtractionSolid("CR_PU_wallLNGS",CR_PU_roomLNGS,CR_Al_roomLNGS,rot_room_LNGS,tr_room_LNGS);
    G4SubtractionSolid* CR_Al_ext_wallLNGS = new G4SubtractionSolid("CR_Al_ext_wallLNGS",CR_Al_ext_roomLNGS,CR_PU_roomLNGS,rot_room_LNGS,tr_room_LNGS);
  
    CR_PC_wallLNGS_log = new G4LogicalVolume(CR_PC_wallLNGS,CYGNOMaterials->Material("PC"),"CR_PC_wallLNGS_log",0,0,0);
    CR_Al_wallLNGS_log = new G4LogicalVolume(CR_Al_wallLNGS,CYGNOMaterials->Material("Al"),"CR_Al_wallLNGS_log",0,0,0);
    CR_PU_wallLNGS_log = new G4LogicalVolume(CR_PU_wallLNGS,CYGNOMaterials->Material("PU_foam"),"CR_PU_wallLNGS_log",0,0,0);
    CR_Al_ext_wallLNGS_log = new G4LogicalVolume(CR_Al_ext_wallLNGS,CYGNOMaterials->Material("Al"),"CR_Al_ext_wallLNGS_log",0,0,0);	
  
    //inner virtual volume
    G4Box* ControlRoom = new G4Box("Control_Room", 0.5*(x_roomLNGS-10*mm), 0.5*(y_roomLNGS-10*mm), 0.5*(z_roomLNGS-10*mm));
    Control_Room_log = new G4LogicalVolume(ControlRoom,CYGNOMaterials->Material("Air"),"ControlRoom_log",0,0,0);	
  
    //DAMA and TIR gallery virtual box
    G4Box* DAMA_container = new G4Box("DAMA_container",0.5*(4.*m+x_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(z_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness));    
    DAMA_container_log = new G4LogicalVolume(DAMA_container,CYGNOMaterials->Material("Air"),"DAMA_container_log",0,0,0);
    G4Box* TIR_gallery = new G4Box("TIR_gallery",0.5*(x_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), 0.5*(y_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness), (z_roomLNGS+PC_wall_thickness+Al_wall_thickness+PU_wall_thickness+Al_wall_thickness));    
    TIR_gallery_log = new G4LogicalVolume(TIR_gallery,CYGNOMaterials->Material("Air"),"TIR_gallery_log",0,0,0);
  
  
    //Rock tunnel
    G4double width = 11500.*mm;
    G4double length = 25000.*mm;
    G4double height = 6500.*mm;
    G4Box* tunnel_in = new G4Box("tunnel_in",0.5*length,0.5*height,0.5*width);
    G4Box* tunnel_out = new G4Box("tunnel_out",0.5*(length+2.*m),0.5*height,0.5*(width+2.*m));
    G4SubtractionSolid* tunnel = new G4SubtractionSolid("tunnel",tunnel_out,tunnel_in,rot_room_LNGS,G4ThreeVector(0.,1.*m,0.));
  
    G4double rmin = 9265.*mm;
    G4double rmax = 10265.*mm;
    G4double dz = 27000.*mm;
    G4double phiMin = -45*deg;//38.36
    G4double phiMax = 2.*45*deg;

    G4Tubs* dome = new G4Tubs("dome", rmin, rmax, dz/2., phiMin, phiMax);
    G4ThreeVector tr_dome(0.*m,-4.2*m,0.*m);
    G4RotationMatrix* rot_dome = new G4RotationMatrix();
    rot_dome->rotateX(90.*deg);
    rot_dome->rotateY(90.*deg);
  
    G4UnionSolid* Rock_gallery = new G4UnionSolid("Rock_gallery",tunnel,dome,rot_dome,tr_dome);

    Rock_gallery_log = new G4LogicalVolume(Rock_gallery,CYGNOMaterials->Material("LNGSRock"),"Rock_gallery_log",0,0,0);

    tr_room_LNGS = G4ThreeVector(131.*mm,(0.5*y_roomLNGS-765.*mm),-100.*mm);
    
  }
  else
  {
    G4cout << "ERROR: Something went wrong with the definition of the variable CYGNOShielding" << G4endl;
    //throw std::exception();
    exit(1);		
  }
     
  /// Cameras
  G4double x_camera_body = 125*mm;
  G4double y_camera_body = 85*mm;
  G4double z_camera_body = 85*mm;
  G4Box* camera_body = new G4Box("camera_body_solid",0.5*x_camera_body,0.5*y_camera_body,0.5*z_camera_body);

  G4double z_camera_lens = 44.9*mm;
  G4double diam_camera_lens = 48*mm;
  G4Tubs* camera_lens = new G4Tubs("camera_lens_solid",0.,0.5*diam_camera_lens,0.5*z_camera_lens,0.*deg,360.*deg);
  
  //keep camera and lens separated
  camera_log = new G4LogicalVolume(camera_body,CYGNOMaterials->Material("Camera"),"camera_log",0,0,0) ;
  camera_lens_log = new G4LogicalVolume(camera_lens,CYGNOMaterials->Material("Camera"),"camera_lens_log",0,0,0) ;

  G4double tolerance = 1*mm;
    

  //**********************************************************************
  // ********* CYGNO volumes form CADMesh *****************************
  //**********************************************************************
  
  char namestl[70];
  
  snprintf(namestl, sizeof(namestl), "%s/LIMEbody-ShortCone.stl", CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;   
  ifstream infile(CYGNOGeomPath.c_str());
  if (infile.good())
    mesh_LIMEDetectorBody = CADMesh::TessellatedMesh::FromSTL(namestl);     
  
  snprintf(namestl, sizeof(namestl), "%s/LIMEinternalStructure.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;
  if (infile.good())
    mesh_LIMEinternalStructure = CADMesh::TessellatedMesh::FromSTL(namestl);
  
  snprintf(namestl, sizeof(namestl), "%s/LIMEendPMT.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;
  if (infile.good())
    mesh_LIMEendPMT = CADMesh::TessellatedMesh::FromSTL(namestl);  
  
  snprintf(namestl, sizeof(namestl), "%s/FieldRings_new.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;
  if (infile.good())
    mesh_FieldRings = CADMesh::TessellatedMesh::FromSTL(namestl);
  
  snprintf(namestl, sizeof(namestl), "%s/GEMstretchers.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;
  if (infile.good())
    mesh_GEMstretchers = CADMesh::TessellatedMesh::FromSTL(namestl); 
  
  snprintf(namestl, sizeof(namestl), "%s/GEMsupportStructure.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;     
  if (infile.good())
    mesh_GEMsupportStructure = CADMesh::TessellatedMesh::FromSTL(namestl);

  snprintf(namestl, sizeof(namestl), "%s/GEMfoils.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;
  if (infile.good())
    mesh_GEMfoils = CADMesh::TessellatedMesh::FromSTL(namestl); 

  snprintf(namestl, sizeof(namestl), "%s/Cathode_new.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl;
  if (infile.good())
    mesh_Cathode = CADMesh::TessellatedMesh::FromSTL(namestl); 
  
  snprintf(namestl, sizeof(namestl), "%s/LIME_Resistors.stl",CYGNOGeomPath.c_str());
  G4cout << namestl << G4endl; 
  if (infile.good())
    mesh_LIMEResistors = CADMesh::TessellatedMesh::FromSTL(namestl);
  
  snprintf(namestl, sizeof(namestl), "%s/CopperBox100mm.stl",CYGNOGeomPath.c_str()); //change to CopperShielding60mm or CopperShielding100mm
  G4cout << namestl << G4endl; 
  if (infile.good())
    mesh_CopperShielding = CADMesh::TessellatedMesh::FromSTL(namestl);

  // snprintf(namestl, sizeof(namestl), "%s/WaterShielding.stl",CYGNOGeomPath.c_str()); 
  // G4cout << namestl << G4endl; 
  // if (infile.good())
  //     mesh_WaterShielding = new CADMesh(namestl);
        
  if (infile.good())
  {
    // mesh_LIMEDetectorBody->SetScale(1);
    // mesh_LIMEinternalStructure->SetScale(1);
    // //mesh_camera->SetScale(mm);
    // //mesh_window->SetScale(mm);
    // mesh_LIMEendPMT->SetScale(1);
    // mesh_FieldRings->SetScale(1);
    // mesh_GEMstretchers->SetScale(1);
    // mesh_GEMsupportStructure->SetScale(1);
    // mesh_GEMfoils->SetScale(1);
    // //mesh_SupportBenchLime->SetScale(mm);
    // mesh_Cathode->SetScale(1);
    // mesh_LIMEResistors->SetScale(1);
    // mesh_CopperShielding->SetScale(1);
    // mesh_WaterShielding->SetScale(1);
  
    //LIME Detector Body
    cad_LIMEDetectorBody_solid = mesh_LIMEDetectorBody->GetSolid();
    cad_LIMEDetectorBody_logical = new G4LogicalVolume(cad_LIMEDetectorBody_solid, CYGNOMaterials->Material("Perspex"), "cad_LIMEDetectorBody_logical");
    cad_LIMEDetectorBody_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));


    //LIME Internal Structure
    cad_LIMEinternalStructure_solid = mesh_LIMEinternalStructure->GetSolid();
    cad_LIMEinternalStructure_logical = new G4LogicalVolume(cad_LIMEinternalStructure_solid, CYGNOMaterials->Material("Perspex"), "cad_LIMEinternalStructure_logical", 0, 0, 0);
    //cad_LIMEinternalStructure_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_LIMEinternalStructure_logical->GetMaterial()->GetName()));
    cad_LIMEinternalStructure_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));
  }
  
  //TPC gas
  G4double TPC_x = 635.*mm;//640.*mm;
  G4double TPC_y = 495.*mm; //500
  G4double TPC_z = 450.*mm; //470.*mm;
    
  name_phys="TPC";
  name_log=name_phys+"_log";
  name_solid=name_phys+"_solid";
  G4Box* TPC_box = new G4Box(name_solid,0.5*TPC_x,0.5*TPC_y,0.5*TPC_z);
  TPC_log = new G4LogicalVolume(TPC_box,CYGNOMaterials->Material("CYGNO_gas"),name_log,0,0,0);
  
  //CYGNO fiducial gas
  G4double CYGNO_x = 480.*mm;//480 mm
  G4double CYGNO_y = 320.*mm;
  G4double CYGNO_z = 320.*mm;//300.*mm; 320 used for AmBe to digitize
    
  name_phys="CYGNO";
  name_log=name_phys+"_log";
  name_solid=name_phys+"_solid";
  G4Box* CYGNO_box = new G4Box(name_solid,0.5*CYGNO_x,0.5*CYGNO_y,0.5*CYGNO_z);
  CYGNO_log = new G4LogicalVolume(CYGNO_box,CYGNOMaterials->Material("CYGNO_gas"),name_log,0,0,0);
  
  G4double maxStep = 1*um;
  fStepLimit = new G4UserLimits(maxStep);
  CYGNO_log->SetUserLimits(fStepLimit); 

  //CYGNO_log->SetVisAttributes(CYGNOMaterials->VisAttributes(CYGNO_log->GetMaterial()->GetName()));
  CYGNO_log->SetVisAttributes(CYGNOMaterials->VisAttributes("CYGNO_gas"));

  if (infile.good())
  {
    //LIMEendPMT
    cad_LIMEendPMT_solid = mesh_LIMEendPMT->GetSolid();
    cad_LIMEendPMT_logical = new G4LogicalVolume(cad_LIMEendPMT_solid, CYGNOMaterials->Material("Perspex"), "cad_LIMEendPMT_logical", 0, 0, 0);
    //cad_LIMEendPMT_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_LIMEendPMT_logical->GetMaterial()->GetName()));
    cad_LIMEendPMT_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));

    //turns support
    //cad_turns_support_solid = mesh_turns_support->GetSolid();
    //cad_turns_support_logical = new G4LogicalVolume(cad_turns_support_solid, CYGNOMaterials->Material("Perspex"), "cad_turns_support_logical", 0, 0, 0);
    //cad_turns_support_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_turns_support_logical->GetMaterial()->GetName()));
    //cad_turns_support_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));

    //field cage  
    cad_FieldRings_solid = mesh_FieldRings->GetSolid();
    cad_FieldRings_logical = new G4LogicalVolume(cad_FieldRings_solid, CYGNOMaterials->Material("Cu"), "cad_FieldRings_logical", 0, 0, 0);
    //cad_FieldRings_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_FieldRings_logical->GetMaterial()->GetName()));
    cad_FieldRings_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));

    //GEM stretchers
    cad_GEMstretchers_solid = mesh_GEMstretchers->GetSolid();
    cad_GEMstretchers_logical = new G4LogicalVolume(cad_GEMstretchers_solid, CYGNOMaterials->Material("Perspex"), "cad_GEMstretchers_logical", 0, 0, 0);
    //cad_GEMstretchers_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_GEMstretchers_logical->GetMaterial()->GetName()));
    cad_GEMstretchers_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));
    
    //GEM structure support
    cad_GEMsupportStructure_solid = mesh_GEMsupportStructure->GetSolid();
    cad_GEMsupportStructure_logical = new G4LogicalVolume(cad_GEMsupportStructure_solid, CYGNOMaterials->Material("Perspex"), "cad_GEMsupportStructure_logical", 0, 0, 0);
    cad_GEMsupportStructure_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));


    //GEMfoils
    cad_GEMfoils_solid = mesh_GEMfoils->GetSolid();
    cad_GEMfoils_logical = new G4LogicalVolume(cad_GEMfoils_solid, CYGNOMaterials->Material("GEM"), "cad_GEMfoils_logical", 0, 0, 0); //GEM material is an effective material of kapton + copper 
    cad_GEMfoils_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_GEMfoils_logical->GetMaterial()->GetName()));
      
    //Support Bench LIME
    //  cad_SupportBenchLime_solid = mesh_SupportBenchLime->GetSolid();
    //  cad_SupportBenchLime_logical = new G4LogicalVolume(cad_SupportBenchLime_solid, CYGNOMaterials->Material("Cu"), "cad_SupportBenchLime_logical", 0, 0, 0);
    //  cad_SupportBenchLime_logical->SetVisAttributes(CYGNOMaterials->VisAttributes(cad_SupportBenchLime_logical->GetMaterial()->GetName()));
    //  cad_SupportBenchLime_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));
      
    //cathode
    cad_Cathode_solid = mesh_Cathode->GetSolid();
    cad_Cathode_logical = new G4LogicalVolume(cad_Cathode_solid, CYGNOMaterials->Material("Cu"), "cad_Cathode_logical", 0, 0, 0);
    cad_Cathode_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));
      
    //resistors
    cad_LIMEResistors_solid = mesh_LIMEResistors->GetSolid();
    cad_LIMEResistors_logical = new G4LogicalVolume(cad_LIMEResistors_solid, CYGNOMaterials->Material("Ceramic"), "cad_LIMEResistors_logical", 0, 0, 0);
    cad_LIMEResistors_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Ceramic"));
      
    //copper shielding
    cad_CopperShielding_solid = mesh_CopperShielding->GetSolid();
    cad_CopperShielding_logical = new G4LogicalVolume(cad_CopperShielding_solid, CYGNOMaterials->Material("Cu"), "cad_CopperShielding_logical", 0, 0, 0);
    cad_CopperShielding_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));

    //water shielding
    /*cad_WaterShielding_solid = mesh_WaterShielding->GetSolid();
    cad_WaterShielding_logical = new G4LogicalVolume(cad_WaterShielding_solid, CYGNOMaterials->Material("Water"), "cad_WaterShielding_logical", 0, 0, 0);
    cad_WaterShielding_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Water"));
    */    
  }

  //FIXME
  //G4ThreeVector tr_fix_shield3;
  //tr_fix_shield3 = G4ThreeVector(-10*cm,0,0);
  //tr_fix_shield3 = G4ThreeVector(0,0,0);
  G4ThreeVector tr_airbox;
  tr_airbox = G4ThreeVector(8*cm,5*cm,0);
  G4ThreeVector tr_tpc;
  tr_tpc=G4ThreeVector(-(TPC_x/2.-CYGNO_x/2.-60.*mm),1.5*cm,0.);
  //tr_tpc = G4ThreeVector(0.,0.,0.);
  G4ThreeVector tr_shield3;
  tr_shield3 = G4ThreeVector(20*cm,10*cm,0);
  G4ThreeVector tr_shield_ext;
  tr_shield_ext = G4ThreeVector(20*cm,10*cm,0);
  //tr_shield_ext = G4ThreeVector(0,0,0);

  //tr_cad=G4ThreeVector(-3554*mm,-3845*mm,230.*mm);
  tr_cad=G4ThreeVector(-35.5*cm,-25.8*cm,-26*cm);
  
  G4double ztr_cam = 785.*mm + z_camera_lens+0.5*x_camera_body +6*cm; //893.4mm
  G4ThreeVector trcam0(ztr_cam,-20.*mm,0.);
  G4RotationMatrix* rotcam0 = new G4RotationMatrix();
  
  G4ThreeVector trlens0(ztr_cam-0.5*x_camera_body-0.5*z_camera_lens,-20.*mm,0.);
  G4RotationMatrix* rotlens0 = new G4RotationMatrix();
  rotlens0->rotateY(90*deg);

  G4RotationMatrix* rotAmBe;
  G4ThreeVector trAmBesource;
  G4ThreeVector trAmBeshield;
  G4ThreeVector trAmBePbshield;
  G4ThreeVector trAmBecapsule;
  G4ThreeVector trLIMEbase;
  G4ThreeVector trDAMA;
  G4ThreeVector trTIRgallery;
  G4ThreeVector trControlRoom;
  G4ThreeVector trRockGallery;
    
  if (CYGNOShielding == "AmBe")
  { 
    //with Lead shield
    trAmBeshield = G4ThreeVector(0.,0.,-670.*mm); //4cm of copper: 0,0,650 for bigger PE shield, 0,0,600 for the original 100x50x50
    trAmBePbshield = G4ThreeVector(0.,0.,0.5*z_ambe_shield-0.5*z_ambe_pb_shield);
    trAmBePbshield+=trAmBeshield;
    trAmBecapsule = G4ThreeVector(0.,0.,0.5*z_ambe_shield-0.5*z_ambe_capsule-z_ambe_pb_shield);
    //trAmBecapsule+=trAmBeshield;
    //trAmBeshield += tr_tpc;
    //trAmBePbshield += tr_tpc;
    rotAmBe = new G4RotationMatrix();
    trAmBesource = G4ThreeVector(0.,0.,0.5*z_ambe_capsule-3.2*mm);
    trLIMEbase = G4ThreeVector(200.*mm,-200.*mm-300.*mm-60*mm,0.);
    
    trDAMA = G4ThreeVector(-2.*m,0.,-9000.*mm);
    trTIRgallery = G4ThreeVector(0.,0.,-3700*mm);
    trControlRoom = G4ThreeVector(0.,2200.*mm,0.);
    trRockGallery = G4ThreeVector(6.*m,1.*m,-4.5*m);
    
    //without lead shield
    /* trAmBeshield = G4ThreeVector(0.,0.,-600.*mm);
    trAmBecapsule = G4ThreeVector(0.,0.,0.5*z_ambe_shield-0.5*z_ambe_capsule);
    trAmBeshield += tr_tpc;
    rotAmBe = new G4RotationMatrix();
    trAmBesource = G4ThreeVector(0.,0.,0.5*z_ambe_capsule-3.2*mm);*/
  }
    
  //FIXME
  tr_cad = tr_cad+tr_tpc;
  trcam0 = trcam0+tr_tpc;
  trlens0 = trlens0+tr_tpc;


  if (CYGNOLab == "LNGS")
  {
    tr+=G4ThreeVector(0.,-1*size_Laboratory.y()+size_Shielding.y(),size_Laboratory.z()-10*m);
    tr_cad+=G4ThreeVector(0.,1.0*m-1*size_Laboratory.y()+size_Shielding.y(),size_Laboratory.z()-10*m);	  
      
    rot = G4RotationMatrix();// rotation of daughter volume
    tr_Shielding+=(rot_Shielding*tr);
  }
  else if (CYGNOLab == "NoCave" || CYGNOLab == "MuonLNGS") 
  {
    tr=G4ThreeVector(0.,0.,0.);
    tr_cad+=G4ThreeVector(0.,0.,0.);
    rot = G4RotationMatrix();
    tr_Shielding+=(rot_Shielding*tr);
  }

  if (CYGNOShielding != "LIMEShield" && CYGNOShielding!= "AmBe") 
  {
    Shield0_phys = new G4PVPlacement(G4Transform3D(rot,tr),Shielding_log,"Shield0",Laboratory_log,false,0,true);
  }
  if (CYGNOShielding == "NoShield")
  {
    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    AirBox_log = Shielding_log;
  }
    
  else if (CYGNOShielding == "FullShield") 
  {   
    // ----------------------------------- Volume placements

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield1_phys = new G4PVPlacement(G4Transform3D(rot,tr),Shield1_log,"Shield1",Shielding_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield2_phys = new G4PVPlacement(G4Transform3D(rot,tr),Shield2_log,"Shield2",Shield1_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield3_phys = new G4PVPlacement(G4Transform3D(rot,tr),Shield3_log,"Shield3",Shield2_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    AirBox_phys = new G4PVPlacement(G4Transform3D(rot,tr), AirBox_log, "AirBox", Shield3_log, false, 0,true); 
  }
    
  else if (CYGNOShielding == "LIMEShield") 
  {
    Shield0_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield_ext),Shielding_log,"Shield0",Laboratory_log,false,0,true);
      
    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield1_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield_ext-tr_shield_ext),Shield1_log,"Shield1",Shielding_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield2_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield_ext-tr_shield_ext),Shield2_log,"Shield2",Shield1_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield3_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield3-tr_shield_ext),Shield3_log,"Shield3",Shield2_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    AirBox_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_airbox-tr_shield3 ), AirBox_log, "AirBox", Shield3_log, false, 0,true); 
  }
  else if (CYGNOShielding == "AmBe")
  {
    Shield0_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield_ext),Shielding_log,"Shield0",Laboratory_log,false,0,true);
      
    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield1_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield_ext-tr_shield_ext),Shield1_log,"Shield1",Shielding_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield2_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield_ext-tr_shield_ext),Shield2_log,"Shield2",Shield1_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    Shield3_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_shield3-tr_shield_ext),Shield3_log,"Shield3",Shield2_log,false,0,true);

    tr = G4ThreeVector(0.,0.,0.);//translation in mother frame
    tr_InsideVolume+=(rot_InsideVolume*tr);
    rot = G4RotationMatrix();// rotation of daughter volume
    rot_InsideVolume*=rot; //equivalent to rot_InsideVolume=rot_InsideVolume*rot
    //AirBox_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_airbox-tr_shield3-tr_room_LNGS), AirBox_log, "AirBox", Shield3_log, false, 0,true); 
    AirBox_phys = new G4PVPlacement(G4Transform3D(rot,tr+tr_airbox-tr_room_LNGS), AirBox_log, "AirBox", Shield3_log, false, 0,true); 	
	}
  
  G4ThreeVector  size;
    
  if (infile.good())
  {
    //cad_WaterShielding_physical = new G4PVPlacement(G4Transform3D(rot_cad_shield,tr_cad-tr_shield_ext), 
    //cad_WaterShielding_logical,"cad_WaterShielding_physical", Shield2_log, false, 0, true); 

    if (CYGNOShielding!="AmBe")
    {
    cad_CopperShielding_physical = new G4PVPlacement(G4Transform3D(rot_cad_shield,tr_cad-tr_shield3), 
            cad_CopperShielding_logical,"cad_CopperShielding_physical", Shield3_log, false, 0, true); 
    }
	  else
    {
      cad_CopperShielding_physical = new G4PVPlacement(G4Transform3D(rot_cad_shield,tr_cad-tr_room_LNGS), 
        	                          cad_CopperShielding_logical,"cad_CopperShielding_physical", Shield3_log, false, 0, true); 
    }

    cad_LIMEDetectorBody_physical = new G4PVPlacement(G4Transform3D(rot_cad_shield,tr_cad-tr_airbox), 
                                    cad_LIMEDetectorBody_logical,"cad_LIMEDetectorBody_physical", AirBox_log, false, 0, true);
    cad_LIMEendPMT_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_airbox), 
                              cad_LIMEendPMT_logical,"cad_LIMEendPMT_physical", AirBox_log, false, 0, true);
  }
  tr=G4ThreeVector(0.,0.,0.);
  //FIXME
  TPC_phys = new G4PVPlacement(G4Transform3D(rot,tr_tpc-tr_airbox),
            TPC_log,"TPC_gas", AirBox_log, false, 0, true);
  
  tr_CYGNO_gas_1= -1.*tr_tpc;
  //tr_CYGNO_gas_1=G4ThreeVector(TPC_x/2.-CYGNO_x/2.-50.*mm,-20.*mm,0.);
  CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_1),
          CYGNO_log,"CYGNO_gas", TPC_log, false, 0, true);
        
  tr=G4ThreeVector(0.,0.,0.);
  rot = G4RotationMatrix();
  if (infile.good())
  {
    cad_LIMEinternalStructure_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                                        cad_LIMEinternalStructure_logical,"cad_LIMEinternalStructure_physical", TPC_log, false, 0, true);
    cad_FieldRings_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                              cad_FieldRings_logical,"cad_FieldRings_physical", TPC_log, false, 0, true);
    cad_GEMstretchers_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                                cad_GEMstretchers_logical,"cad_GEMstretchers_physical", TPC_log, false, 0, true);
    cad_GEMsupportStructure_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                                      cad_GEMsupportStructure_logical,"cad_GEMsupportStructure_physical", TPC_log, false, 0, true);
    cad_GEMfoils_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                            cad_GEMfoils_logical,"cad_GEMfoils_physical", TPC_log, false, 0, true);
    cad_Cathode_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                          cad_Cathode_logical,"cad_Cathode_physical", TPC_log, false, 0, true);
    cad_LIMEResistors_physical = new G4PVPlacement(G4Transform3D(rot,tr_cad-tr_tpc), 
                                cad_LIMEResistors_logical,"cad_LIMEResistors_physical", TPC_log, false, 0, true); 
  }  
    

  if (CYGNOShielding == "AmBe")
  {
    ambe_shield_phys = new G4PVPlacement(rotAmBe,trAmBeshield+tr_shield3-tr_shield_ext-tr_room_LNGS, ambe_shield_log, "ambe_shield", Shield3_log,false,0,true);
    ambe_pb_shield_phys = new G4PVPlacement(rotAmBe,trAmBePbshield+tr_shield3-tr_shield_ext-tr_room_LNGS, ambe_pb_shield_log, "ambe_pb_shield", Shield3_log,false,0,true);
    ambe_capsule_phys = new G4PVPlacement(rotAmBe,trAmBecapsule+trAmBeshield+tr_shield3-tr_shield_ext-tr_room_LNGS, ambe_capsule_log,"ambe_capsule",Shield3_log, false, 0, true);
    ambe_source_phys = new G4PVPlacement(rotAmBe,trAmBesource, ambe_source_log,"ambe_source",ambe_capsule_log, false, 0, true);
    PC_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield3-tr_shield_ext,PC_wallLNGS_log,"PC_wallLNGS",Shield2_log,false,0,true);
    Al_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield3-tr_shield_ext,Al_wallLNGS_log,"Al_wallLNGS",Shield2_log,false,0,true);
    PU_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield3-tr_shield_ext,PU_wallLNGS_log,"PU_wallLNGS",Shield2_log,false,0,true);
    Al_ext_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield3-tr_shield_ext,Al_ext_wallLNGS_log,"Al_ext_wallLNGS",Shield2_log,false,0,true);
    LIME_base_phys = new G4PVPlacement(rot_room_LNGS,trLIMEbase+tr_shield3-tr_shield_ext-tr_room_LNGS,LIME_base_log,"LIME_base",Shield3_log,false,0,true);
    camera_phys = new G4PVPlacement(rotcam0,trcam0-tr_shield_ext+tr_shield3-tr_room_LNGS,camera_log,"camera",Shield3_log, false, 0, true);
    camera_lens_phys = new G4PVPlacement(rotlens0,trlens0+tr_shield3-tr_shield_ext-tr_room_LNGS,camera_lens_log,"camera_lens",Shield3_log, false, 0, true);
    
    CR_PC_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trControlRoom,CR_PC_wallLNGS_log,"CR_PC_wallLNGS",Laboratory_log,false,0,true);
    CR_Al_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trControlRoom,CR_Al_wallLNGS_log,"CR_Al_wallLNGS",Laboratory_log,false,0,true);
    CR_PU_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trControlRoom,CR_PU_wallLNGS_log,"CR_PU_wallLNGS",Laboratory_log,false,0,true);
    Al_ext_wallLNGS_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trControlRoom,CR_Al_ext_wallLNGS_log,"CR_Al_ext_wallLNGS",Laboratory_log,false,0,true);
    DAMA_container_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trDAMA,DAMA_container_log,"DAMA_container",Laboratory_log,false,0,true);
    TIR_gallery_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trTIRgallery,TIR_gallery_log,"TIR_gallery",Laboratory_log,false,0,true);
    Control_Room_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trControlRoom,Control_Room_log,"Control_Room",Laboratory_log,false,0,true);
    Rock_gallery_phys = new G4PVPlacement(rot_room_LNGS,tr+tr_shield_ext+trRockGallery,Rock_gallery_log,"Rock_gallery",Laboratory_log,false,0,true);
  }
  else
  {
		camera_phys = new G4PVPlacement(rotcam0,trcam0-tr_shield_ext,camera_log,"camera",Shield3_log, false, 0, true);
    camera_lens_phys = new G4PVPlacement(rotlens0,trlens0-tr_shield_ext,camera_lens_log,"camera_lens",Shield3_log, false, 0, true);
	}

  //
  //**********************************************************************
  // GLOBAL TRANSLATIONS ***************************************
  G4cout<<"Placement of Laboratory in the World started"<<G4endl;
  
  tr_Rock=-1*(tr_Laboratory+rot_Laboratory*(tr+rot_cad*(tr_InsideVolume+rot_InsideVolume*(tr_Shielding))));//The shift of Rock_log in the world volume to make the origin be the center of the detector

  G4RotationMatrix rot_check = absrot_Rock*(rot_Laboratory*(rot_cad*(rot_InsideVolume)));  
  name_phys="externalRock";
  name_log=name_phys+"_log";
  name_solid=name_phys+"_solid";
  externalRock_phys = new G4PVPlacement(G4Transform3D(absrot_Rock,tr_Rock),Rock_log,name_phys,WorldVolume_log,false,0,true);

  G4cout << "The main CYGNO volume is translated w.r.t the center of the rock volume of:\t x="<< -1*tr_Rock.x()/cm << " cm\t y=" << -1*tr_Rock.y()/cm << " cm\t z=" << -1*tr_Rock.z()/cm << " cm"<< G4endl;

  G4cout << "The Rock volume has been translated to put the main CYGNO volume in the center of the coordinate system"<< G4endl;
  G4cout<<"Placement of Laboratory in the World ended"<<G4endl;

  //======= Save volumes mass and density ======
  
  G4cout<<"Saving masses and densities of the volumes"<<G4endl;
  SaveMassAndDensity();


  //===========
  return WorldVolume_phys;
}

void CYGNODetectorConstruction::SaveMassAndDensity()
{
  CYGNODetectorProperty* CYGNOProperties = CYGNODetectorProperty::GetInstance();

  G4cout << "Saving masses and densities of the volumes of the CYGNODetectorConstruction class"<< G4endl;
  //CYGNOProperties->AddVolumeNameMassAndDensity(Rock_log);
  //CYGNOProperties->AddVolumeNameMassAndDensity(Laboratory_log);
  if (CYGNOShielding=="FullShield"){
      CYGNOProperties->AddVolumeNameMassAndDensity(Shielding_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Shield1_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Shield2_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Shield3_log);
  }
  CYGNOProperties->AddVolumeNameMassAndDensity(AirBox_log);
  ifstream infile(CYGNOGeomPath.c_str());
  if (infile.good()) {
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_LIMEDetectorBody_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_LIMEinternalStructure_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(camera_log);
    CYGNOProperties->AddVolumeNameMassAndDensity(camera_lens_log);
    if (CYGNOShielding == "AmBe"){
      CYGNOProperties->AddVolumeNameMassAndDensity(ambe_capsule_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(ambe_source_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(ambe_shield_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(ambe_pb_shield_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Al_wallLNGS_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(PC_wallLNGS_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(PU_wallLNGS_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Al_ext_wallLNGS_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(LIME_base_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(DAMA_container_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(TIR_gallery_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Control_Room_log);
      CYGNOProperties->AddVolumeNameMassAndDensity(Rock_gallery_log);
    }
    CYGNOProperties->AddVolumeNameMassAndDensity(TPC_log);
    CYGNOProperties->AddVolumeNameMassAndDensity(CYGNO_log);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_LIMEendPMT_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_GEMstretchers_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_GEMsupportStructure_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_GEMfoils_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_Cathode_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_FieldRings_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_LIMEResistors_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_CopperShielding_logical);
  //  CYGNOProperties->AddVolumeNameMassAndDensity(cad_WaterShielding_logical);
  }

//  if ( productionRockThinTube_phys )
//	{
//	  CYGNOProperties->AddPhysVolumeNameMassAndDensity(productionRockThinTube_phys);
//	}
  G4cout << "All volume masses and densities saved"<< G4endl;
}

void CYGNODetectorConstruction::UpdateGeometry()
{

  G4cout << "Updating the Geometry"<< G4endl;
  CYGNODetectorProperty* CYGNOProperties = CYGNODetectorProperty::GetInstance();
  CYGNOProperties->Refresh();

  //Removing sensitive detectors
  CYGNO_log->SetSensitiveDetector(0);
  CYGNO_log=0;

  //Deleting all the solids, logical and physical objects
  //G4RunManager::GetRunManager()->ReinitializeGeometry(true);
  //Equivalent to
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore* PhysicalVolumeStore = G4PhysicalVolumeStore::GetInstance();
  PhysicalVolumeStore->Clean();
  G4LogicalVolumeStore* LogicalVolumeStore = G4LogicalVolumeStore::GetInstance();
  LogicalVolumeStore->Clean();
  G4SolidStore* SolidStore = G4SolidStore::GetInstance();
  SolidStore->Clean();
  
  CYGNODetectorMaterial* CYGNOMaterials = CYGNODetectorMaterial::GetInstance();
  CYGNOMaterials->Refresh();


  //The memory for these pointers has been freed by the above Clean() methods
  CYGNODetectorLNGS* CYGNOLNGS = CYGNODetectorLNGS::GetInstance();
  CYGNOLNGS->Refresh();

  Shield0_log=0; 
  Shield1_log=0; 
  Shield2_log=0; 
  Shield3_log=0; 
  AirBox_log=0;

  cad_LIMEDetectorBody_logical=0;
  cad_LIMEinternalStructure_logical=0;
 // cad_cameras_all_logical=0;
  TPC_log=0;
  CYGNO_log=0;
  cad_LIMEendPMT_logical=0;
  cad_GEMsupportStructure_logical=0;
  cad_GEMstretchers_logical=0;
  cad_GEMfoils_logical=0;
//  cad_SupportBenchLime_logical=0;
  cad_Cathode_logical=0;
  cad_FieldRings_logical=0;
  cad_LIMEResistors_logical=0;
  cad_CopperShielding_logical=0;
//  cad_WaterShielding_logical=0;
  camera_log=0;
  camera_lens_log=0;
 // camera_shield_log=0;
  ambe_shield_log=0;
  ambe_pb_shield_log=0;
  ambe_capsule_log=0;
  ambe_capsule_log=0;
  PC_wallLNGS_log=0;
  Al_wallLNGS_log=0;
  PU_wallLNGS_log=0;
  Al_ext_wallLNGS_log=0;
  LIME_base_log=0;
  CR_PC_wallLNGS_log=0;
  CR_Al_wallLNGS_log=0;
  CR_PU_wallLNGS_log=0;
  CR_Al_ext_wallLNGS_log=0;
  Control_Room_log=0;
  TIR_gallery_log=0;
  DAMA_container_log=0;

  InsideVolume_log=0;
  Shielding_log=0;
  Laboratory_log=0;
  Rock_log=0;
  WorldVolume_log=0;
  productionRockThinTube_phys=0;
  //log->ClearDaughters();

  // Delete all the geometry you had defined and build everything from scratch
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}
void CYGNODetectorConstruction::UpdateGeometryPath(G4String newpath)
{
  G4cout << "Updating the Geometry path to "<< newpath << G4endl;
  SetGeomPath(newpath); 

}

void CYGNODetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  //
  // Sensitive detectors
  //
  auto cygnoSD
    = new CYGNOSensitiveDetector("cygnoSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(cygnoSD);
  SetSensitiveDetector("CYGNO_log",cygnoSD);

}