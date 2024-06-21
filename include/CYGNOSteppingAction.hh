#ifndef CYGNOSteppingAction_h
#define CYGNOSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "CYGNODetectorConstruction.hh"
#include "CYGNOEventAction.hh"
#include <fstream>

class CYGNOSteppingAction : public G4UserSteppingAction
{
  public:
	  CYGNOSteppingAction(CYGNODetectorConstruction*);
	  ~CYGNOSteppingAction(){energyOutputFile.close();};

	  void UserSteppingAction(const G4Step*);
  private:

	  CYGNODetectorConstruction* fDetector;
	  std::ofstream energyOutputFile;
};


#endif
