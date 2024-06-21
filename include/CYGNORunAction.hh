#ifndef CYGNORunAction_h
#define CYGNORunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include <fstream>
#include <vector>

class G4Run;
class CYGNORunActionMessenger;
class CYGNODetectorConstruction;
class CYGNOEventAction;

class CYGNORunAction : public G4UserRunAction
{
  public:
  CYGNORunAction(CYGNOEventAction*, CYGNODetectorConstruction*);
  virtual ~CYGNORunAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  
  void AddNumberOfMuonsDetector(){fNumberOfMuonsDetector++;};
  void SetOutFile(G4String fname) {FileName = fname;};
  void SetOutFileCut(G4int cut) {fOutFileCut = cut;};
  void SetRegisterOn(G4int regOn) {fRegisterOn = regOn;};
  void SetHitsInfo(G4int cut) {fHitsInfo = cut;};

  // Additional methods
  // void SetVolumeInfoFileName(const G4String& fileName);
  // void OpenVolumeInfoFile();
  // void CloseVolumeInfoFile();
  // void WriteVolumeInfoToFile(const G4String& volumeName, double mass, double density);

  private:

  void Book();
   
  CYGNODetectorConstruction* fDetector = nullptr;
  CYGNOEventAction* fEventAction = nullptr;
  int fNumberOfMuonsDetector;  
  G4String FileName;  
  G4int fOutFileCut;
  G4int fRegisterOn;
  G4int fHitsInfo;

  //messanger
  CYGNORunActionMessenger* fMessenger; 
};

#endif
