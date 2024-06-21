#include "CYGNOStackingAction.hh"

#include "CYGNOUserEventInformation.hh"
#include "CYGNOSteppingAction.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"


#include "G4Track.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4TransportationManager.hh"
#include "G4VProcess.hh"
#include "G4Ions.hh"
#include "G4SystemOfUnits.hh"
//#include <CLHEP/Units/SystemOfUnits.h>

//using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CYGNOStackingAction::CYGNOStackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CYGNOStackingAction::~CYGNOStackingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack 
CYGNOStackingAction::ClassifyNewTrack(const G4Track* track)
{
  CYGNOUserEventInformation* eventInformation=
    (CYGNOUserEventInformation*)G4EventManager::GetEventManager()
    ->GetConstCurrentEvent()->GetUserInformation();

  
  //kill secondary neutrino
  if (track->GetDefinition() == G4NeutrinoE::NeutrinoE() && track->GetParentID()>0) return fKill;
  if (track->GetDefinition() == G4AntiNeutrinoE::AntiNeutrinoE() && track->GetParentID()>0) return fKill;

  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

