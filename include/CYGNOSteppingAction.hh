#ifndef CYGNOSteppingAction_h
#define CYGNOSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "CYGNODetectorConstruction.hh"
//#include "CYGNOEventAction.hh"
#include "CYGNOAnalysis.hh"
#include <fstream>

class CYGNOSteppingAction : public G4UserSteppingAction
{
  public:
	  CYGNOSteppingAction(CYGNODetectorConstruction*); //, CYGNOEventAction*);
	  ~CYGNOSteppingAction(){energyOutputFile.close();};

	  void UserSteppingAction(const G4Step*);
  private:

	  CYGNODetectorConstruction* fDetector;
	  //CYGNOEventAction* fEventAction;
	  std::ofstream energyOutputFile;
};


#endif
