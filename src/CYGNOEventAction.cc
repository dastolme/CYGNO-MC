#include "CYGNOEventAction.hh"
#include "CYGNORunAction.hh"
#include "CYGNOEventActionMessenger.hh"
#include "CYGNOAnalysis.hh"
#include "CYGNOHit.hh"
#include "CYGNODetectorConstruction.hh"
#include "CYGNOUserEventInformation.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"

#include "G4UnitsTable.hh"

CYGNOEventAction::CYGNOEventAction(CYGNORunAction* run, CYGNODetectorConstruction* myDC)
  : G4UserEventAction(), runAct(run), fDetector(myDC)
{
  fMessenger = new CYGNOEventActionMessenger(this);
  fDetectorHit = false;
  repfreq = 1000;

  // hits collections
  CYGNOID = -1;

  energy_pri=0;

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
  
  fDetectorHit = false;

  //thread-local run action
  if (!runAct) 
    runAct = 
      dynamic_cast<const CYGNORunAction*>
      (G4RunManager::GetRunManager()->GetUserRunAction());
  
  if (!genAction)
    genAction = dynamic_cast<const CYGNOPrimaryGeneratorAction*>
      (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  // grab energy of primary
  energy_pri = genAction->GetEnergyPrimary();

  // grab x, y, z positions of the primary vertex
  G4ThreeVector primaryVertexPosition = genAction->GetPrimaryVertexPosition();
  x_position_pri = primaryVertexPosition.getX();
  y_position_pri = primaryVertexPosition.getY();
  z_position_pri = primaryVertexPosition.getZ();

  event_id = evt->GetEventID();

  if ( CYGNOID == -1) {
    G4SDManager * SDman = G4SDManager::GetSDMpointer();
    CYGNOID = SDman->GetCollectionID("CYGNOCollection");
  } 


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
    }
  
  if (event_id%repfreq == 0) 
    {
      G4cout << ">>> Event " << evt->GetEventID() << G4endl;
      //      G4cout << "    " << n_trajectories << " trajectories stored in this event." << G4endl;
    }


  // check that hits collection has been defined
  if(CYGNOID<0) return;

  G4AnalysisManager* man = G4AnalysisManager::Instance();
  if (!man) {
      G4cerr << "Error: Analysis manager not available!" << G4endl;
      return;
  }

  // address hits collections
  CYGNOHitsCollection* CYGNOHC = NULL;
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(HCE) {
    CYGNOHC = (CYGNOHitsCollection*)(HCE->GetHC(CYGNOID));
  }

  // event summary
  totEnergy         = 0.;
  totEnergyNR       = 0.;
  hitEnergy         = 0.;
  firstParticleE    = 0.;
  firstParticleID   = 0.;
  particleID        = 0.;

  if(CYGNOHC) {
    CYGNO_hits = CYGNOHC->entries();
        
    if (CYGNO_hits > 0) {
      for (G4int i=0; i<CYGNO_hits; i++) {
        if(i==0) {
          firstParticleID   = (*CYGNOHC)[0]->GetParticleID();
          firstParticleE    = (*CYGNOHC)[0]->GetKineticEne();
          G4cout << "firstParticleE =" << firstParticleE << "keV" << G4endl;
        }
        hitEnergy         = (*CYGNOHC)[i]->GetEdep();
        totEnergy        += hitEnergy;
        
        particleID        = (*CYGNOHC)[i]->GetParticleID();
        particleEnergy    = (*CYGNOHC)[i]->GetKineticEne();

        if ((int)(*CYGNOHC)[i]->GetParticleID()>1000000000){
          totEnergyNR += (*CYGNOHC)[i]->GetEdep();
        }
      }
      fillNtuple();
    }
  }
}

void CYGNOEventAction::fillNtuple() 
{
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  //Fill ntuple #2
  man->FillNtupleDColumn(2,0,event_id);
  man->FillNtupleDColumn(2,1,firstParticleE);
  man->FillNtupleDColumn(2,2,totEnergy);
  man->FillNtupleDColumn(2,3,totEnergyNR);
  man->FillNtupleDColumn(2,4,firstParticleID);
  man->AddNtupleRow(2);

}