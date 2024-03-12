#ifndef CYGNOEventAction_h
#define CYGNOEventAction_h 1

#include "G4UserEventAction.hh"
#include "CYGNORunAction.hh"
#include "CYGNOPrimaryGeneratorAction.hh"

#include "G4GenericMessenger.hh"
#include <vector>

class G4Event;
class CYGNOEventActionMessenger;
class CYGNODetectorConstruction;
class G4Track;

class CYGNOEventAction : public G4UserEventAction
{
  public:
  CYGNOEventAction(CYGNORunAction*,CYGNODetectorConstruction*);
   ~CYGNOEventAction();

  public:
  void BeginOfEventAction(const G4Event* evt);
  void EndOfEventAction(const G4Event* evt);

  void SetDetectorHit(bool isHit){fDetectorHit = isHit;};
  void SetRepFreq(G4int rfreq) {repfreq = rfreq;}

  void AddNumberOfMuonsDetector(){fNumberOfMuonsDetector++;};
  void SetOutFile(G4String fname) {FileName = fname;};
  void SetOutFileCut(G4int cut) {fOutFileCut = cut;};
  void SetRegisterOn(G4int regOn) {fRegisterOn = regOn;};
  void SetHitsInfo(G4int cut) {fHitsInfo = cut;};

  private: 
  void fillNtuple();
  
  CYGNOEventActionMessenger* fMessenger;
  CYGNODetectorConstruction* fDetector;

  const CYGNORunAction*    runAct;  //pointer to run action
  const CYGNOPrimaryGeneratorAction* genAction; // pointer to particle generator

  bool fDetectorHit;
  G4int repfreq;

  G4int event_id;

  G4double energy_pri;
  G4double x_position_pri;
  G4double y_position_pri;
  G4double z_position_pri;

  // hits collections
  G4int CYGNOID;
  G4int CYGNO_hits;

  // event summary
  G4double totEnergy;
  G4double totEnergyNR;
  G4double hitEnergy;
  G4double firstParticleE;
  G4double particleEnergy;
  G4double particleID;
  G4double firstParticleID;

  G4bool   ER;
  G4bool   NR;

  int fNumberOfMuonsDetector; 
  G4String FileName;  
  G4int fOutFileCut;
  G4int fRegisterOn;
  G4int fHitsInfo;
};

#endif


