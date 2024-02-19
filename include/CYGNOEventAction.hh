#ifndef CYGNOEventAction_h
#define CYGNOEventAction_h 1

#include "G4UserEventAction.hh"
#include "CYGNORunAction.hh"

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

  private: 
  
  CYGNOEventActionMessenger* fMessenger;
  CYGNORunAction*  fRunAct;
  CYGNODetectorConstruction* fDetector;
  
  bool fDetectorHit;
  G4int repfreq;
};

#endif


