#include "CYGNOEventAction.hh"
#include "CYGNORunAction.hh"
#include "CYGNOEventActionMessenger.hh"
#include "CYGNOAnalysis.hh"
#include "CYGNODetectorConstruction.hh"
#include "CYGNOUserEventInformation.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"

CYGNOEventAction::CYGNOEventAction(CYGNORunAction* run, CYGNODetectorConstruction* myDC)
  : G4UserEventAction(), fRunAct(run), fDetector(myDC)
{
  fMessenger = new CYGNOEventActionMessenger(this);
  fDetectorHit = false;
  repfreq = 1000;
}

CYGNOEventAction::~CYGNOEventAction()
{
  delete fMessenger;
}

void CYGNOEventAction::BeginOfEventAction(const G4Event* evt)
{
  //New event, add the user information object
  G4EventManager::
    GetEventManager()->SetUserInformation(new CYGNOUserEventInformation);


  CYGNOAnalysis* analysis = CYGNOAnalysis::getInstance();
  analysis->BeginOfEvent(evt,fDetector);
  
  fDetectorHit = false;
}

void CYGNOEventAction::EndOfEventAction(const G4Event* evt)
{
  G4int event_id = evt->GetEventID();
  
  CYGNOUserEventInformation* eventInformation
	      =(CYGNOUserEventInformation*)evt->GetUserInformation();

  // get number of stored trajectories
  //
  G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();
  
  if(fDetectorHit)
    {	  
      fRunAct->AddNumberOfMuonsDetector();
    }
  
  if (event_id%repfreq == 0) 
    {
      G4cout << ">>> Event " << evt->GetEventID() << G4endl;
      //      G4cout << "    " << n_trajectories << " trajectories stored in this event." << G4endl;
    }

}