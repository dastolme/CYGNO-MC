#include "globals.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4PhysicalVolumeStore.hh"

#include "CYGNODetectorConstruction.hh"
#include "CYGNODetectorConstructionMessenger.hh"

//---------------------------------------------------------------------------//

CYGNODetectorConstructionMessenger::CYGNODetectorConstructionMessenger
(CYGNODetectorConstruction *detector):fDetectorPrimary(detector)
{
    fGeomPathDirectory = new G4UIdirectory("/CYGNO/pathtocad/");
    fGeomPathDirectory->SetGuidance("Control commands to set the geometry path to CAD files");
    
    fCYGNOCADPathCmd = new G4UIcmdWithAString("/CYGNO/pathtocad",this);
    fCYGNOCADPathCmd->SetGuidance("Set the path to CYGNO CAD *.stl files.");
    fCYGNOCADPathCmd->SetDefaultValue("../geometry/v2/");
    fCYGNOCADPathCmd->SetParameterName("choice",false);
    fCYGNOCADPathCmd->AvailableForStates(G4State_Init,G4State_Idle);
    
    fLabDirectory = new G4UIdirectory("/CYGNO/lab/");
    fLabDirectory->SetGuidance("Control commands for lab:");

    fCYGNOLabCmd = new G4UIcmdWithAString("/CYGNO/lab/select",this);
    fCYGNOLabCmd->SetGuidance("Select the CYGNO laboratory geometry. Possible choices: LNGS, SUPL or NoCave");
    fCYGNOLabCmd->SetGuidance("LNGS: Set the laboratory geometry of LNGS");
    fCYGNOLabCmd->SetGuidance("NoCave: Remove the laboratory and replaces it with a large box of air");
    fCYGNOLabCmd->SetDefaultValue("LNGS");
    fCYGNOLabCmd->SetCandidates("LNGS NoCave");
    fCYGNOLabCmd->SetParameterName("choice",false);
    fCYGNOLabCmd->AvailableForStates(G4State_Init,G4State_Idle);

    fexternalrockthickCmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/lab/externalrockthick",this);
    fexternalrockthickCmd->SetGuidance("Set thickness of the rock surrounding the laboratory");
    fexternalrockthickCmd->SetGuidance("This command is valid only if CYGNOLab has been selected as LNGS");
    fexternalrockthickCmd->SetDefaultUnit("m");
    fexternalrockthickCmd->SetUnitCandidates("mm cm m");
    fexternalrockthickCmd->SetParameterName("choice",false);
    fexternalrockthickCmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fproductionrockthickCmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/lab/productionlayerthick",this);
    fproductionrockthickCmd->SetGuidance("Set thickness of the rock layer where particles are generated");
    fproductionrockthickCmd->SetGuidance("When CYGNOLab is NoCave, this command set the thickness of the air shell around the shielding");
    fproductionrockthickCmd->SetDefaultUnit("cm");
    fproductionrockthickCmd->SetUnitCandidates("mm cm m");
    fproductionrockthickCmd->SetParameterName("choice",false);
    fproductionrockthickCmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    finternalrockthickCmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/lab/internalrockthick",this);
    finternalrockthickCmd->SetGuidance("Set the distance of the rock layer where particles are generated from the experimental hall wall");
    finternalrockthickCmd->SetGuidance("When CYGNOLab is NoCave, this command set the distance of the air shell from the origin");
    finternalrockthickCmd->SetDefaultUnit("cm");
    finternalrockthickCmd->SetUnitCandidates("mm cm m");
    finternalrockthickCmd->SetParameterName("choice",false);
    finternalrockthickCmd->AvailableForStates(G4State_Init,G4State_Idle);
    
    fShieldDirectory = new G4UIdirectory("/CYGNO/shield/");
    fShieldDirectory->SetGuidance("Control commands for shield:");
    
    fCYGNOShieldingCmd = new G4UIcmdWithAString("/CYGNO/shield/select",this);
    fCYGNOShieldingCmd->SetGuidance("Select the CYGNO shielding geometry. Possible choices: FullShield, NoShield, CYGNO_04");
    fCYGNOShieldingCmd->SetGuidance("FullShield: four concentric boxes of different thicknesses and materials. Shielding thickness and material to be selected by user");
    fCYGNOShieldingCmd->SetGuidance("NoShield:  no shielding.");
    fCYGNOShieldingCmd->SetGuidance("CYGNO_04: Shieling design for CYGNO_04 as in TDR july 2022.");
    fCYGNOShieldingCmd->SetDefaultValue("CYGNO_04");
    fCYGNOShieldingCmd->SetCandidates("FullShield NoShield CYGNO_04");
    fCYGNOShieldingCmd->SetParameterName("choice",false);
    fCYGNOShieldingCmd->AvailableForStates(G4State_Init,G4State_Idle);
    
    fthick0Cmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/shield/thick0",this);
    fthick0Cmd->SetGuidance("Set thickness for shield layer #0; layers are numbered from 0 to 3, #0 being the outermost");
    fthick0Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as Full or South");
    fthick0Cmd->SetDefaultUnit("cm");
    fthick0Cmd->SetUnitCandidates("mm cm m");
    fthick0Cmd->SetParameterName("choice",false);
    fthick0Cmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fthick1Cmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/shield/thick1",this);
    fthick1Cmd->SetGuidance("Set thickness for shield layer #1; layers are numbered from 0 to 3, #0 being the outermost");
    fthick1Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as Full or South");
    fthick1Cmd->SetDefaultUnit("cm");
    fthick1Cmd->SetUnitCandidates("mm cm m");
    fthick1Cmd->SetParameterName("choice",false);
    fthick1Cmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fthick2Cmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/shield/thick2",this);
    fthick2Cmd->SetGuidance("Set thickness for shield layer #2; layers are numbered from 0 to 3, #0 being the outermost");
    fthick2Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as Full or South");
    fthick2Cmd->SetDefaultUnit("cm");
    fthick2Cmd->SetUnitCandidates("mm cm m");
    fthick2Cmd->SetParameterName("choice",false);
    fthick2Cmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fthick3Cmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/shield/thick3",this);
    fthick3Cmd->SetGuidance("Set thickness for shield layer #3; layers are numbered from 0 to 3, #0 being the outermost");
    fthick3Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as Full or South");
    fthick3Cmd->SetDefaultUnit("cm");
    fthick3Cmd->SetUnitCandidates("mm cm m");
    fthick3Cmd->SetParameterName("choice",false);
    fthick3Cmd->AvailableForStates(G4State_Init,G4State_Idle);
    
    fMat0Cmd = new G4UIcmdWithAString("/CYGNO/shield/mat0",this);
    fMat0Cmd->SetGuidance("Set material of the CYGNO shield layer #0. Possible candidates: Pb, PE, Cu, Air, Water, Steel");
    fMat0Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as FullShield");
    fMat0Cmd->SetParameterName("choice",true);
    fMat0Cmd->SetDefaultValue("Air");
    fMat0Cmd->SetCandidates("Pb PE Cu Air Water Steel");
    fMat0Cmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fMat1Cmd = new G4UIcmdWithAString("/CYGNO/shield/mat1",this);
    fMat1Cmd->SetGuidance("Set material of the CYGNO shield layer #1. Possible candidates: Pb, PE, Cu, Air, Water, Steel");
    fMat1Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as FullShield");
    fMat1Cmd->SetParameterName("choice",true);
    fMat1Cmd->SetDefaultValue("Air");
    fMat1Cmd->SetCandidates("Pb PE Cu Air Water Steel");
    fMat1Cmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fMat2Cmd = new G4UIcmdWithAString("/CYGNO/shield/mat2",this);
    fMat2Cmd->SetGuidance("Set material of the CYGNO shield layer #2. Possible candidates: Pb, PE, Cu, Air, Water, Steel");
    fMat2Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as FullShield");
    fMat2Cmd->SetParameterName("choice",true);
    fMat2Cmd->SetDefaultValue("Pb");
    fMat2Cmd->SetCandidates("Pb PE Cu Air Water Steel");
    fMat2Cmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fMat3Cmd = new G4UIcmdWithAString("/CYGNO/shield/mat3",this);
    fMat3Cmd->SetGuidance("Set material of the CYGNO shield layer #3. Possible candidates: Pb, PE, Cu, Air, Water, Steel");
    fMat3Cmd->SetGuidance("This command is valid only if CYGNOShielding has been selected as FullShield");
    fMat3Cmd->SetParameterName("choice",true);
    fMat3Cmd->SetDefaultValue("Cu");
    fMat3Cmd->SetCandidates("Pb PE Cu Air Water Steel");
    fMat3Cmd->AvailableForStates(G4State_Init,G4State_Idle);
    
    fInsideVolumeRadiusCmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/shield/inner_radius",this);
    fInsideVolumeRadiusCmd->SetGuidance("Set the radius of the cavity inside the SUPL shielding");
    fInsideVolumeRadiusCmd->SetDefaultUnit("m");
    fInsideVolumeRadiusCmd->SetUnitCandidates("mm cm m");
    fInsideVolumeRadiusCmd->SetParameterName("choice",false);
    fInsideVolumeRadiusCmd->AvailableForStates(G4State_Init,G4State_Idle);
  
    fInsideVolumeHeightCmd = new G4UIcmdWithADoubleAndUnit("/CYGNO/shield/inner_height",this);
    fInsideVolumeHeightCmd->SetGuidance("Set the height of the cavity inside the SUPL shielding");
    fInsideVolumeHeightCmd->SetDefaultUnit("m");
    fInsideVolumeHeightCmd->SetUnitCandidates("mm cm m");
    fInsideVolumeHeightCmd->SetParameterName("choice",false);
    fInsideVolumeHeightCmd->AvailableForStates(G4State_Init,G4State_Idle);

    fupdateCmd = new G4UIcmdWithoutParameter("/CYGNO/updateGeo", this);
    fupdateCmd->SetGuidance("Update detector geometry after settings");
}

CYGNODetectorConstructionMessenger::~CYGNODetectorConstructionMessenger()
{
    delete fCYGNOCADPathCmd;
    delete fCYGNOLabCmd;
    delete fCYGNOShieldingCmd;

    delete fexternalrockthickCmd;
    delete fproductionrockthickCmd;
    delete finternalrockthickCmd;
    
    delete fthick0Cmd;
    delete fthick1Cmd;
    delete fthick2Cmd;
    delete fthick3Cmd;
    delete fMat0Cmd;
    delete fMat1Cmd;
    delete fMat2Cmd;
    delete fMat3Cmd;
    delete fInsideVolumeRadiusCmd;
    delete fInsideVolumeHeightCmd;
    
   
    delete fupdateCmd;
}

//---------------------------------------------------------------------------//

void CYGNODetectorConstructionMessenger::SetNewValue(G4UIcommand *command,
                                                     G4String newValue)
{
  if (command == fCYGNOCADPathCmd )
    {
	  fDetectorPrimary->SetGeomPath(newValue);
    }
  else if (command == fCYGNOLabCmd )
    {
	  fDetectorPrimary->SetCYGNOLab(newValue);
    }
  else if (command == fexternalrockthickCmd )
    {
	  fDetectorPrimary->SetExternalRockThickness(fexternalrockthickCmd->GetNewDoubleValue(newValue));
    }
  else if (command == fproductionrockthickCmd )
    {
	  fDetectorPrimary->SetProductionRockThickness(fproductionrockthickCmd->GetNewDoubleValue(newValue));
    }
  else if (command == finternalrockthickCmd )
    {
	  fDetectorPrimary->SetInternalRockThickness(finternalrockthickCmd->GetNewDoubleValue(newValue));
    }
  else if (command == fCYGNOShieldingCmd )
    {
	  fDetectorPrimary->SetCYGNOShielding(newValue);
    }
  else if (command == fthick0Cmd )
    {
	  fDetectorPrimary->SetShieldThick0(fthick0Cmd->GetNewDoubleValue(newValue));
    }
  else if (command == fthick1Cmd )
    {
	  fDetectorPrimary->SetShieldThick1(fthick1Cmd->GetNewDoubleValue(newValue));
    }
  else if (command == fthick2Cmd )
    {
	  fDetectorPrimary->SetShieldThick2(fthick2Cmd->GetNewDoubleValue(newValue));
    }
  else if (command == fthick3Cmd )
    {
	  fDetectorPrimary->SetShieldThick3(fthick3Cmd->GetNewDoubleValue(newValue));
    }
  else if (command == fMat0Cmd )
    {
	  fDetectorPrimary->SetShield0Material(newValue);
    }
  else if (command == fMat1Cmd )
    {
	  fDetectorPrimary->SetShield1Material(newValue);
    }
  else if (command == fMat2Cmd )
    {
	  fDetectorPrimary->SetShield2Material(newValue);
    }
  else if (command == fMat3Cmd )
    {
	  fDetectorPrimary->SetShield3Material(newValue);
    }
  else if (command == fInsideVolumeRadiusCmd )
    {
	  fDetectorPrimary->SetInsideVolumeRadius(fInsideVolumeRadiusCmd->GetNewDoubleValue(newValue));
    }
  else if (command == fInsideVolumeHeightCmd )
    {
	  fDetectorPrimary->SetInsideVolumeHeight(fInsideVolumeHeightCmd->GetNewDoubleValue(newValue));
    }
  else if ( command == fupdateCmd )
	{
	  fDetectorPrimary->UpdateGeometry();
	}
}
