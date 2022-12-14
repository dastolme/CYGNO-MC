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

#include "G4RotationMatrix.hh"

// USER //
#include "CYGNODetectorConstruction.hh"
#include "CYGNODetectorConstructionMessenger.hh"
#include "CYGNODetectorLNGS.hh"
#include "CYGNODetectorMaterial.hh"
#include "CYGNODetectorProperty.hh"
#include "CYGNOSensitiveDetector.hh"
#include "CYGNOVolumes.hh"

CYGNODetectorConstruction::CYGNODetectorConstruction() :
   CYGNOGeomPath("../geometry/cygno_04_v2/"),
   rockThicknessOuter(-999*m),
   rockThicknessInner(-999*m),
   CYGNOLab("NoCave"),
   //CYGNOLab("LNGS"),
   CYGNOShielding("CYGNO_04"),
   //CYGNOShielding("NoShield"),
   thick0(0.01*m), thick1(0.01*m), thick2(0.01*m), thick3(0.01*m), 
   Mat0("Air"), Mat1("Air"), Mat2("Air"), Mat3("Air")
{
	
     fMessenger = new CYGNODetectorConstructionMessenger(this);
}

CYGNODetectorConstruction::~CYGNODetectorConstruction()
{
	delete fMessenger;
}

G4VPhysicalVolume* CYGNODetectorConstruction::Construct()
{
   
    UpdateGeometryPath(CYGNOGeomPath); 
    //register the SD
    G4SDManager* SDmanager=G4SDManager::GetSDMpointer();
    
    G4String CYGNOSDname = "CYGNO/CYGNOSD";
    CYGNOSD = new CYGNOSensitiveDetector( CYGNOSDname );
    SDmanager->AddNewDetector( CYGNOSD );
	
	
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
    if (CYGNOLab == "LNGS"){
    
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
	  G4double airInnerRadius = 10.0*m;
	  G4double airThickness = 5.0*m;
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
    else
    {
      G4cout << "ERROR: Something went wrong with the definition of the variable CYGNOLab" << G4endl;
      //throw std::exception();
      exit(1);		
    }


    G4cout << "Laboratory done." << G4endl;
 
    //**********************************************************************
    // SHIELDING **********************************************************
          
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
        
	//FIXME
	//same as TPC gas
	AirBox_x = 7.*m; //2.65*m;
        AirBox_y = 7.*m; //1.45*m;
        AirBox_z = 7.*m; //1.45*m;        
        
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
          AirBox_x = 2.8*m;
          AirBox_y = 1.6*m;
          AirBox_z = 1.6*m;        
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
    else if (CYGNOShielding == "CYGNO_04") 
    {
	  G4double AirBox_x;
	  G4double AirBox_y;
	  G4double AirBox_z;
	  G4Box* AirBox;
          AirBox_x = 7.*m;
          AirBox_y = 7.*m;
          AirBox_z = 7.*m;        
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
    else
    {
      G4cout << "ERROR: Something went wrong with the definition of the variable CYGNOShielding" << G4endl;
      //throw std::exception();
      exit(1);		
    }
     
    //**********************************************************************
    // ********* CYGNO volumes form CADMesh *****************************
    //**********************************************************************

    char namestl[50];
    sprintf(namestl,"%s/water_shield.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    ifstream infile(CYGNOGeomPath.c_str());
    if (infile.good())
    	mesh_water_shield = new CADMesh(namestl);    
    sprintf(namestl,"%s/pe_shield.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_pe_shield = new CADMesh(namestl);    
    sprintf(namestl,"%s/shield_Cu.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_cu_shield = new CADMesh(namestl);    
    sprintf(namestl,"%s/camera_shield.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_camera_shield = new CADMesh(namestl);    
    sprintf(namestl,"%s/cameras.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_cameras = new CADMesh(namestl);    
    sprintf(namestl,"%s/lenses_ext.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_lenses_ext = new CADMesh(namestl);    
    sprintf(namestl,"%s/lenses.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_lenses = new CADMesh(namestl);    
    sprintf(namestl,"%s/pmts.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_pmts = new CADMesh(namestl);    
    sprintf(namestl,"%s/acrylic_box.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
      mesh_acrylic_box = new CADMesh(namestl);
    sprintf(namestl,"%s/cathode.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_cathode = new CADMesh(namestl);
    sprintf(namestl,"%s/cathode_frame.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
    	mesh_cathode_frame = new CADMesh(namestl);
    sprintf(namestl,"%s/fc_sheet.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
      mesh_fc_sheet = new CADMesh(namestl);
    sprintf(namestl,"%s/internal_structure.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
      mesh_internal_structure = new CADMesh(namestl);
    sprintf(namestl,"%s/gems.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;
    if (infile.good())
      mesh_gem= new CADMesh(namestl);  
    sprintf(namestl,"%s/gem_frame.stl",CYGNOGeomPath.c_str());
    G4cout << namestl << G4endl;     
    if (infile.good())
      mesh_gem_frame = new CADMesh(namestl); 

    if (infile.good()) { 

      mesh_water_shield->SetScale(mm);
      mesh_pe_shield->SetScale(mm);
      mesh_cu_shield->SetScale(mm);
      mesh_camera_shield->SetScale(mm);
      mesh_cameras->SetScale(mm);
      mesh_lenses_ext->SetScale(mm);
      mesh_lenses->SetScale(mm);
      mesh_pmts->SetScale(mm);
      mesh_acrylic_box->SetScale(mm);
      mesh_cathode->SetScale(mm);
      mesh_cathode_frame->SetScale(mm);
      mesh_fc_sheet->SetScale(mm);
      mesh_internal_structure->SetScale(mm);
      mesh_gem->SetScale(mm);
      mesh_gem_frame->SetScale(mm);
   
      //water shield
      cad_water_shield_solid = mesh_water_shield->TessellatedMesh();
      cad_water_shield_logical = new G4LogicalVolume(cad_water_shield_solid, CYGNOMaterials->Material("Water"), "cad_water_shield_logical");
      cad_water_shield_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Water"));
      
      //polyethilene shield base
      cad_pe_shield_solid = mesh_pe_shield->TessellatedMesh();
      cad_pe_shield_logical = new G4LogicalVolume(cad_pe_shield_solid, CYGNOMaterials->Material("PE"), "cad_pe_shield_logical");
      cad_pe_shield_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("PE"));

      //copper shield lateral
      cad_cu_shield_solid = mesh_cu_shield->TessellatedMesh();
      cad_cu_shield_logical = new G4LogicalVolume(cad_cu_shield_solid, CYGNOMaterials->Material("Cu"), "cad_cu_shield_logical");
      cad_cu_shield_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));
      
      //copper shield cameras
      cad_camera_shield_solid = mesh_camera_shield->TessellatedMesh();
      cad_camera_shield_logical = new G4LogicalVolume(cad_camera_shield_solid, CYGNOMaterials->Material("Cu"), "cad_camera_shield_logical");
      cad_camera_shield_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));
      
      //cameras
      cad_cameras_solid = mesh_cameras->TessellatedMesh();
      cad_cameras_logical = new G4LogicalVolume(cad_cameras_solid, CYGNOMaterials->Material("Camera"), "cad_cameras_logical");
      cad_cameras_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Camera"));
      
      //lenses external part
      cad_lenses_ext_solid = mesh_lenses_ext->TessellatedMesh();
      cad_lenses_ext_logical = new G4LogicalVolume(cad_lenses_ext_solid, CYGNOMaterials->Material("Camera"), "cad_lenses_ext_logical");
      cad_lenses_ext_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Camera"));
      
      //lenses
      cad_lenses_solid = mesh_lenses->TessellatedMesh();
      cad_lenses_logical = new G4LogicalVolume(cad_lenses_solid, CYGNOMaterials->Material("BSglass"), "cad_lenses_logical");
      cad_lenses_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Camera"));
      
      //PMTs
      cad_pmts_solid = mesh_pmts->TessellatedMesh();
      cad_pmts_logical = new G4LogicalVolume(cad_pmts_solid, CYGNOMaterials->Material("BSglass"), "cad_pmts_logical");
      cad_pmts_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Camera"));
      
      //internal structure
      cad_internal_structure_solid = mesh_internal_structure->TessellatedMesh();
      cad_internal_structure_logical = new G4LogicalVolume(cad_internal_structure_solid, CYGNOMaterials->Material("Perspex"), "cad_internal_structure_logical", 0, 0, 0);
      cad_internal_structure_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));
      
      //acrylic box
      cad_acrylic_box_solid = mesh_acrylic_box->TessellatedMesh();
      cad_acrylic_box_logical = new G4LogicalVolume(cad_acrylic_box_solid, CYGNOMaterials->Material("Perspex"), "cad_acrylic_box_logical", 0, 0, 0);
      cad_acrylic_box_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));

      //field cage resistive sheet
      cad_fc_sheet_solid = mesh_fc_sheet->TessellatedMesh();
      cad_fc_sheet_logical = new G4LogicalVolume(cad_fc_sheet_solid, CYGNOMaterials->Material("Perspex"), "cad_fc_sheet_logical", 0, 0, 0);
      cad_fc_sheet_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));

      //GEM frame
      cad_gem_frame_solid = mesh_gem_frame->TessellatedMesh();
      cad_gem_frame_logical = new G4LogicalVolume(cad_gem_frame_solid, CYGNOMaterials->Material("Perspex"), "cad_gem_frame_logical", 0, 0, 0);
      cad_gem_frame_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));
      

      //GEM foils
      cad_gem_solid = mesh_gem->TessellatedMesh();
      cad_gem_logical = new G4LogicalVolume(cad_gem_solid, CYGNOMaterials->Material("GEM"), "cad_gem_logical", 0, 0, 0); //GEM material is an effective material of kapton + copper 
      
      
      //cathode
      cad_cathode_solid = mesh_cathode->TessellatedMesh();
      cad_cathode_logical = new G4LogicalVolume(cad_cathode_solid, CYGNOMaterials->Material("Cu"), "cad_cathode_logical", 0, 0, 0);
      cad_cathode_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Cu"));

      //cathode frame
      cad_cathode_frame_solid = mesh_cathode_frame->TessellatedMesh();
      cad_cathode_frame_logical = new G4LogicalVolume(cad_cathode_frame_solid, CYGNOMaterials->Material("Perspex"), "cad_cathode_frame_logical", 0, 0, 0);
      cad_cathode_frame_logical->SetVisAttributes(CYGNOMaterials->VisAttributes("Perspex"));
   
    }

    //TPC gas
    G4double TPC_x = 1160.*mm;
    G4double TPC_y = 960*mm;
    G4double TPC_z = 650*mm;
      
    name_phys="TPC";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* TPC_box = new G4Box(name_solid,0.5*TPC_x,0.5*TPC_y,0.5*TPC_z);
    TPC_log = new G4LogicalVolume(TPC_box,CYGNOMaterials->Material("CYGNO_gas"),name_log,0,0,0);
    TPC_log->SetVisAttributes(CYGNOMaterials->VisAttributes("CYGNO_gas"));
    
    //CYGNO fiducial gas
    G4double CYGNO_x = 470.*mm;
    G4double CYGNO_y = 392.*mm;
    G4double CYGNO_z = 242.*mm;
      
    name_phys="CYGNO";
    name_log=name_phys+"_log";
    name_solid=name_phys+"_solid";
    G4Box* CYGNO_box = new G4Box(name_solid,0.5*CYGNO_x,0.5*CYGNO_y,0.5*CYGNO_z);
    CYGNO_log = new G4LogicalVolume(CYGNO_box,CYGNOMaterials->Material("CYGNO_gas"),name_log,0,0,0);
    CYGNO_log->SetVisAttributes(CYGNOMaterials->VisAttributes("CYGNO_gas"));

    if (CYGNOLab == "LNGS"){
	tr+=G4ThreeVector(0.,-1*size_Laboratory.y()+size_Shielding.y(),size_Laboratory.z()-10*m);
	tr_cad+=G4ThreeVector(0.,1.0*m-1*size_Laboratory.y()+size_Shielding.y(),size_Laboratory.z()-10*m);	  
    
	rot = G4RotationMatrix();// rotation of daughter volume
	tr_Shielding+=(rot_Shielding*tr);
    }
    else if (CYGNOLab == "NoCave") {
	tr=G4ThreeVector(0.,0.,0.);
	tr_cad+=G4ThreeVector(0.,0.,0.);
	rot = G4RotationMatrix();
	tr_Shielding+=(rot_Shielding*tr);

    }

    Shield0_phys = new G4PVPlacement(G4Transform3D(rot,tr),Shielding_log,"Shield0",Laboratory_log,false,0,true);
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
    else if (CYGNOShielding == "CYGNO_04") 
    {   
        // ----------------------------------- Volume placements
    	tr_cad_shields=G4ThreeVector(0.,97*cm,0.);
    	rot_cad_shields = G4RotationMatrix();
    	rot_cad_shields.rotateX(-90.*deg);
    	rot_cad_shields.rotateY(-90.*deg);
	if (infile.good()) { 
         	cad_water_shield_physical = new G4PVPlacement(G4Transform3D(rot_cad_shields,tr_cad_shields), 
         	  	    cad_water_shield_logical,"cad_water_shield_physical", AirBox_log, false, 0, true);
         	cad_pe_shield_physical = new G4PVPlacement(G4Transform3D(rot_cad_shields,tr_cad_shields), 
         	  	    cad_pe_shield_logical,"cad_pe_shield_physical", AirBox_log, false, 0, true);
         	cad_cu_shield_physical = new G4PVPlacement(G4Transform3D(rot_cad_shields,tr_cad_shields), 
         	  	    cad_cu_shield_logical,"cad_cu_shield_physical", AirBox_log, false, 0, true);
         	cad_camera_shield_physical = new G4PVPlacement(G4Transform3D(rot_cad_shields,tr_cad_shields), 
         	  	    cad_camera_shield_logical,"cad_camera_shield_physical", AirBox_log, false, 0, true);
	}
    
    }
    G4ThreeVector  size;

    tr_cad=G4ThreeVector(0.,97*cm,0.);
    rot_cad = G4RotationMatrix();
    rot_cad.rotateX(-90.*deg);
    rot_cad.rotateY(-90.*deg);
    rot = G4RotationMatrix();
    
    if (infile.good()) { 
        
        cad_acrylic_box_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_acrylic_box_logical,"cad_acrylic_box_physical", AirBox_log, false, 0, true);
        
       
        //cameras
        cad_cameras_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_cameras_logical,"cad_cameras_physical", AirBox_log, false, 0, true);
        cad_lenses_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_lenses_logical,"cad_lenses_physical", AirBox_log, false, 0, true);
        cad_lenses_ext_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_lenses_ext_logical,"cad_lenses_ext_physical", AirBox_log, false, 0, true);
        cad_pmts_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_pmts_logical,"cad_pmts_physical", AirBox_log, false, 0, true);

        //TPC gas
        tr=G4ThreeVector(0.,0.,0.);
        TPC_phys = new G4PVPlacement(G4Transform3D(rot,tr),
          	    TPC_log,"TPC_gas", AirBox_log, false, 0, true);
        
        // internal parts
        cad_internal_structure_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_internal_structure_logical,"cad_internal_structure_physical", TPC_log, false, 0, true);
        cad_fc_sheet_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_fc_sheet_logical,"cad_fc_sheet_physical", TPC_log, false, 0, true);
        cad_gem_frame_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_gem_frame_logical,"cad_gem_frame_physical", TPC_log, false, 0, true);
        cad_gem_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_gem_logical,"cad_gem_physical", TPC_log, false, 0, true);
        cad_cathode_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_cathode_logical,"cad_cathode_physical", TPC_log, false, 0, true);
        cad_cathode_frame_physical = new G4PVPlacement(G4Transform3D(rot_cad,tr_cad), 
          	    cad_cathode_frame_logical,"cad_cathode_frame_physical", TPC_log, false, 0, true);
    } 
    //CYGNO sensitive regions
    tr_CYGNO_gas_0=G4ThreeVector(480.*mm-CYGNO_x/2.,CYGNO_y/2.,CYGNO_z/2.);
    tr_CYGNO_gas_1=G4ThreeVector(480.*mm-CYGNO_x/2.,CYGNO_y/2.,-1*CYGNO_z/2.);
    tr_CYGNO_gas_2=G4ThreeVector(480.*mm-CYGNO_x/2.,-1.*CYGNO_y/2.,CYGNO_z/2.);
    tr_CYGNO_gas_3=G4ThreeVector(480.*mm-CYGNO_x/2.,-1.*CYGNO_y/2.,-1*CYGNO_z/2);
    tr_CYGNO_gas_4=G4ThreeVector(-480.*mm+CYGNO_x/2.,CYGNO_y/2.,CYGNO_z/2.);
    tr_CYGNO_gas_5=G4ThreeVector(-480.*mm+CYGNO_x/2.,CYGNO_y/2.,-1*CYGNO_z/2.);
    tr_CYGNO_gas_6=G4ThreeVector(-480.*mm+CYGNO_x/2.,-1.*CYGNO_y/2.,CYGNO_z/2.);
    tr_CYGNO_gas_7=G4ThreeVector(-480.*mm+CYGNO_x/2.,-1.*CYGNO_y/2.,-1*CYGNO_z/2);

    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_0),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 0, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_1),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 1, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_2),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 2, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_3),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 3, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_4),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 4, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_5),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 5, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_6),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 6, true);
    CYGNO_phys = new G4PVPlacement(G4Transform3D(rot,tr_CYGNO_gas_7),
      	    CYGNO_log,"CYGNO_gas", TPC_log, false, 7, true);
          
    
    //Cameras
   // camera_phys = new G4PVPlacement(rotcam0,trcam0,camera_log,"camera",AirBox_log, false, 0, true);
   // camera_lens_phys = new G4PVPlacement(rotlens0,trlens0,camera_lens_log,"camera_lens",AirBox_log, false, 0, true);
   // G4double tolerance = 1*mm;
   
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
    
    //======= Sensitive detector ========
    CYGNO_log->SetSensitiveDetector(CYGNOSD);
 
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
  else if (CYGNOShielding=="CYGNO_04"){
  	ifstream infile(CYGNOGeomPath.c_str());
  	if (infile.good()) {
  	  CYGNOProperties->AddVolumeNameMassAndDensity(cad_water_shield_logical);
  	  CYGNOProperties->AddVolumeNameMassAndDensity(cad_pe_shield_logical);
  	  CYGNOProperties->AddVolumeNameMassAndDensity(cad_cu_shield_logical);
  	  CYGNOProperties->AddVolumeNameMassAndDensity(cad_camera_shield_logical);
  	}
  }
  ifstream infile(CYGNOGeomPath.c_str());
  if (infile.good()) {
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_acrylic_box_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_internal_structure_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_cameras_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_lenses_ext_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_lenses_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_pmts_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_gem_frame_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_gem_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_cathode_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_cathode_frame_logical);
    CYGNOProperties->AddVolumeNameMassAndDensity(cad_fc_sheet_logical);
  }
  
    CYGNOProperties->AddVolumeNameMassAndDensity(AirBox_log);
    CYGNOProperties->AddVolumeNameMassAndDensity(TPC_log);
    CYGNOProperties->AddVolumeNameMassAndDensity(CYGNO_log);

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
  TPC_log=0;
  CYGNO_log=0;

  cad_water_shield_logical=0;
  cad_pe_shield_logical=0;
  cad_cu_shield_logical=0;
  cad_camera_shield_logical=0;
  cad_cameras_logical=0;
  cad_lenses_logical=0;
  cad_lenses_ext_logical=0;
  cad_pmts_logical=0;
  cad_internal_structure_logical=0;
  cad_acrylic_box_logical=0;
  cad_gem_frame_logical=0;
  cad_gem_logical=0;
  cad_cathode_logical=0;
  cad_cathode_frame_logical=0;
  cad_fc_sheet_logical=0;


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
