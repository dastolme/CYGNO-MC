#pragma once

// STL //
#include <string>

class CYGNODetectorConstructionMessenger;
// GEANT4 //
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4UserLimits;

// CADMESH //
#include "CADMesh.hh"

// USER //
class CYGNOSensitiveDetector;

#include "G4ThreeVector.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include <vector>


class CYGNODetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    CYGNODetectorConstruction();
    ~CYGNODetectorConstruction();

    G4VPhysicalVolume* Construct() override;
    void SaveMassAndDensity();
    void UpdateGeometry();
    void UpdateGeometryPath(G4String newpath);
    void ConstructSDandField() override;

    void SetExternalRockThickness(G4double rockthick) {rockThicknessOuter = rockthick;}
    void SetProductionRockThickness(G4double rockthick) {productionLayerThickness = rockthick;}
    void SetInternalRockThickness(G4double rockthick) {rockThicknessInner = rockthick;}

    void SetCYGNOLab(G4String lab) {CYGNOLab = lab;}
    G4String GetCYGNOLab() {return CYGNOLab;}

    void SetCYGNOShielding(G4String shield) {CYGNOShielding = shield;}
    G4String GetCYGNOShielding() {return CYGNOShielding;}

    void SetGeomPath(G4String path) {CYGNOGeomPath = path;}

    void SetShieldThick0(G4double thick) {thick0 = thick;}
    void SetShieldThick1(G4double thick) {thick1 = thick;}
    void SetShieldThick2(G4double thick) {thick2 = thick;}
    void SetShieldThick3(G4double thick) {thick3 = thick;}    
    void SetShield0Material(G4String shm) {Mat0 = shm;}
    void SetShield1Material(G4String shm) {Mat1 = shm;}
    void SetShield2Material(G4String shm) {Mat2 = shm;}
    void SetShield3Material(G4String shm) {Mat3 = shm;}
    void SetInsideVolumeRadius(G4double r) {InsideVolume_OR = r;}
    void SetInsideVolumeHeight(G4double h) {InsideVolume_Z = h;}

    G4double GetShieldThick0() {return thick0;}
    G4double GetShieldThick1() {return thick1;}
    G4double GetShieldThick2() {return thick2;}
    G4double GetShieldThick3() {return thick3;}    
  private:
    
    CYGNODetectorConstructionMessenger* fMessenger;

    //G4VSolid * world_solid;
    //G4LogicalVolume* world_logical;
    //G4VPhysicalVolume* world_physical;
    G4UserLimits* fStepLimit;

    G4double InsideVolume_OR;
    G4double InsideVolume_Z;
    G4double rockThicknessOuter;
    G4double rockThicknessInner;
    G4double productionLayerThickness;
   
    G4String CYGNOGeomPath; 
    G4String CYGNOLab;
    G4String CYGNOShielding;
    G4double thick0;
    G4double thick1;
    G4double thick2;
    G4double thick3;
    G4String Mat0;
    G4String Mat1;
    G4String Mat2;
    G4String Mat3;


    //CADMesh
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_LIMEDetectorBody;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_LIMEinternalStructure;
    //CADMesh * mesh_camera;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_LIMEendPMT;
    //CADMesh * mesh_turns_support;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_FieldRings;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_GEMstretchers;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_GEMsupportStructure;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_GEMfoils;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_SupportBenchLime;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_Cathode; 
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_LIMEResistors;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_CopperShielding;
    std::shared_ptr<CADMesh::TessellatedMesh> mesh_WaterShielding;
    
    //Building blocks: logic volumes, sizes and positions
    G4ThreeVector  tr_Tot;
    G4LogicalVolume* Rock_log;
    G4ThreeVector size_Rock;
    G4ThreeVector tr_Rock;
    G4RotationMatrix rot_Rock;
    G4RotationMatrix absrot_Rock;
    G4LogicalVolume* Laboratory_log;
    G4ThreeVector size_Laboratory;
    G4ThreeVector tr_Laboratory;
    G4RotationMatrix rot_Laboratory;
    G4LogicalVolume* Shielding_log;
    G4ThreeVector size_Shielding;
    G4ThreeVector tr_Shielding;
    G4RotationMatrix rot_Shielding;
    G4RotationMatrix absrot_Shielding;
    G4LogicalVolume* InsideVolume_log;
    G4ThreeVector size_InsideVolume;
    G4ThreeVector tr_InsideVolume;
    G4RotationMatrix rot_InsideVolume;
    G4ThreeVector size_cad;
    G4ThreeVector tr_cad;
    G4RotationMatrix rot_cad;
    G4RotationMatrix absrot_cad;
    G4ThreeVector tr_CYGNO_gas_1;
    G4ThreeVector tr_CYGNO_gas_2;
    G4RotationMatrix rot_CYGNO_gas;
    G4RotationMatrix absrot_CYGNO_gas;
    G4ThreeVector tr_cad_internal;
    
    G4RotationMatrix rot_cad_shield;
    G4ThreeVector tr_cad_shield;
    
    
    //Solids and meshes
    G4VSolid * cad_LIMEDetectorBody_solid;
    G4VSolid * cad_LIMEinternalStructure_solid;
    //G4VSolid * cad_cameras_all_solid;
    //G4VSolid * cad_window_solid;
    G4VSolid * cad_LIMEendPMT_solid;
    //G4VSolid * cad_turns_support_solid;
    G4VSolid * cad_GEMstretchers_solid;
    G4VSolid * cad_GEMsupportStructure_solid;
    G4VSolid * cad_GEMfoils_solid;
    G4VSolid * cad_SupportBenchLime_solid;
    G4VSolid * cad_Cathode_solid;
    G4VSolid * cad_FieldRings_solid;
    G4VSolid * cad_LIMEResistors_solid;
    G4VSolid * cad_CopperShielding_solid;
    G4VSolid * cad_WaterShielding_solid;
   
    
    // Logical volumes
    G4LogicalVolume* WorldVolume_log;
    G4LogicalVolume* Shield0_log; 
    G4LogicalVolume* Shield1_log; 
    G4LogicalVolume* Shield2_log; 
    G4LogicalVolume* Shield3_log; 
    G4LogicalVolume* AirBox_log;

    G4LogicalVolume * cad_LIMEDetectorBody_logical;
    G4LogicalVolume * cad_LIMEinternalStructure_logical;
    //G4LogicalVolume * cad_cameras_all_logical;
    //G4LogicalVolume * cad_window_logical;
    G4LogicalVolume * TPC_log;
    G4LogicalVolume * CYGNO_log;
    G4LogicalVolume * cad_LIMEendPMT_logical;
    //G4LogicalVolume * cad_turns_support_logical;
    G4LogicalVolume * cad_GEMstretchers_logical;
    G4LogicalVolume * cad_GEMsupportStructure_logical;
    G4LogicalVolume * cad_GEMfoils_logical;
    G4LogicalVolume * cad_SupportBenchLime_logical;
    G4LogicalVolume * cad_Cathode_logical;
    G4LogicalVolume * cad_FieldRings_logical;
    G4LogicalVolume * cad_LIMEResistors_logical; 
    G4LogicalVolume * cad_CopperShielding_logical; 
    G4LogicalVolume * cad_WaterShielding_logical; 
    G4LogicalVolume * camera_log; 
    G4LogicalVolume * camera_lens_log; 
    G4LogicalVolume * camera_shield_log;
    G4LogicalVolume * window_log;
    G4LogicalVolume * ambe_capsule_log;
    G4LogicalVolume * ambe_source_log;
    G4LogicalVolume * ambe_shield_log;
    G4LogicalVolume * PC_wallLNGS_log;
    G4LogicalVolume * Al_wallLNGS_log;
    G4LogicalVolume * PU_wallLNGS_log;
    G4LogicalVolume * Al_ext_wallLNGS_log;
    G4LogicalVolume * CR_PC_wallLNGS_log;
    G4LogicalVolume * CR_Al_wallLNGS_log;
    G4LogicalVolume * CR_PU_wallLNGS_log;
    G4LogicalVolume * CR_Al_ext_wallLNGS_log;
    G4LogicalVolume * LIME_base_log;
    G4LogicalVolume * DAMA_container_log;
    G4LogicalVolume * TIR_gallery_log;
    G4LogicalVolume * Control_Room_log;
    G4LogicalVolume * Rock_gallery_log;
    
    // Physical volumes
    G4VPhysicalVolume* WorldVolume_phys;
    G4VPhysicalVolume* productionRockThinTube_phys;
    G4VPhysicalVolume* externalRock_phys;
    G4VPhysicalVolume* InnerAirSphere_phys;
    G4VPhysicalVolume* Shield0_phys;
    G4VPhysicalVolume* Shield1_phys;
    G4VPhysicalVolume* Shield2_phys;
    G4VPhysicalVolume* Shield3_phys;
    G4VPhysicalVolume* AirBox_phys;

    G4VPhysicalVolume * cad_LIMEDetectorBody_physical;
    G4VPhysicalVolume * cad_LIMEinternalStructure_physical;
    //G4VPhysicalVolume * cad_cameras_all_physical;
    //G4VPhysicalVolume * cad_window_physical;
    G4VPhysicalVolume * TPC_phys;
    G4VPhysicalVolume * CYGNO_phys;
    G4VPhysicalVolume * cad_LIMEendPMT_physical;
    //G4VPhysicalVolume * cad_turns_support_physical;
    G4VPhysicalVolume * cad_GEMstretchers_physical;
    G4VPhysicalVolume * cad_GEMsupportStructure_physical;
    G4VPhysicalVolume * cad_GEMfoils_physical;
    G4VPhysicalVolume * cad_SupportBenchLime_physical;
    G4VPhysicalVolume * cad_Cathode_physical;
    G4VPhysicalVolume * cad_FieldRings_physical;
    G4VPhysicalVolume* cad_LIMEResistors_physical;
    G4VPhysicalVolume* cad_CopperShielding_physical;
    G4VPhysicalVolume* cad_WaterShielding_physical;
    G4VPhysicalVolume* camera_phys; 
    G4VPhysicalVolume* camera_lens_phys; 
    G4VPhysicalVolume* camera_shield_phys;
    G4VPhysicalVolume* window_phys;
    G4VPhysicalVolume* ambe_capsule_phys;
    G4VPhysicalVolume* ambe_source_phys;
    G4VPhysicalVolume* ambe_shield_phys;
    G4VPhysicalVolume* ambe_pb_shield_phys;
    
    G4VPhysicalVolume * PC_wallLNGS_phys;
    G4VPhysicalVolume * Al_wallLNGS_phys;
    G4VPhysicalVolume * PU_wallLNGS_phys;
    G4VPhysicalVolume * Al_ext_wallLNGS_phys;
    G4VPhysicalVolume * CR_PC_wallLNGS_phys;
    G4VPhysicalVolume * CR_Al_wallLNGS_phys;
    G4VPhysicalVolume * CR_PU_wallLNGS_phys;
    G4VPhysicalVolume * CR_Al_ext_wallLNGS_phys;
    G4VPhysicalVolume * LIME_base_phys;
    G4VPhysicalVolume * DAMA_container_phys; 
    G4VPhysicalVolume * TIR_gallery_phys; 
    G4VPhysicalVolume * Control_Room_phys;
    G4VPhysicalVolume * Rock_gallery_phys;     


};
