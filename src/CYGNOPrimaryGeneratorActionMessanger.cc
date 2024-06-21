#include "CYGNOPrimaryGeneratorActionMessanger.hh"

CYGNOPrimaryGeneratorActionMessenger::CYGNOPrimaryGeneratorActionMessenger(CYGNOPrimaryGeneratorAction* pga)
    : G4UImessenger(), fPrimaryGeneratorAction(pga)
{
    fDirectory = new G4UIdirectory("/CYGNO/primaryGenerator/");
    fDirectory->SetGuidance("Primary generator action control commands");

    fFillNtupleCmd = new G4UIcmdWithABool("/CYGNO/primaryGenerator/fillNtuple", this);
    fFillNtupleCmd->SetGuidance("Control whether to fill the ntuple");
    fFillNtupleCmd->SetParameterName("fillNtuple", true);
    fFillNtupleCmd->SetDefaultValue(true);
}

CYGNOPrimaryGeneratorActionMessenger::~CYGNOPrimaryGeneratorActionMessenger()
{
    delete fFillNtupleCmd;
    delete fDirectory;
}

void CYGNOPrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fFillNtupleCmd) {
        fPrimaryGeneratorAction->SetFillNtuple(fFillNtupleCmd->GetNewBoolValue(newValue));
    }
}
