#ifndef CYGNORunAction_h
#define CYGNORunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include <fstream>

class G4Run;
class CYGNORunActionMessenger;
class CYGNODetectorConstruction;

class CYGNORunAction : public G4UserRunAction
{
  public:
  CYGNORunAction(CYGNODetectorConstruction*);
  virtual ~CYGNORunAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  virtual G4Run* GenerateRun();
  
  void AddNumberOfMuonsDetector(){fNumberOfMuonsDetector++;};
  void SetOutFile(G4String fname) {FileName = fname;};
  void SetOutFileCut(G4int cut) {fOutFileCut = cut;};
  void SetRegisterOn(G4int regOn) {fRegisterOn = regOn;};
  void SetHitsInfo(G4int cut) {fHitsInfo = cut;};

  private:
  
  CYGNORunActionMessenger* fMessenger;  
  CYGNODetectorConstruction* fDetector;
  int fNumberOfMuonsDetector;  
  G4String FileName;  
  G4int fOutFileCut;
  G4int fRegisterOn;
  G4int fHitsInfo;
};

#endif
