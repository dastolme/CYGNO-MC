#ifndef CYGNOStackingAction_h
#define CYGNOStackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

class CYGNOStackingAction : public G4UserStackingAction
{
  public:
    CYGNOStackingAction();
    virtual ~CYGNOStackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);        
  private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

