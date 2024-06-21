#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "CYGNOPrimaryGeneratorAction.hh"  

class CYGNOPrimaryGeneratorActionMessenger : public G4UImessenger {
public:
    CYGNOPrimaryGeneratorActionMessenger(CYGNOPrimaryGeneratorAction* pga);
    virtual ~CYGNOPrimaryGeneratorActionMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue);

private:
    CYGNOPrimaryGeneratorAction* fPrimaryGeneratorAction;
    G4UIdirectory* fDirectory;
    G4UIcmdWithABool* fFillNtupleCmd;
};