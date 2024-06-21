#include "CYGNOEventAction.hh"
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
#include "G4AnalysisManager.hh"

CYGNOEventAction::CYGNOEventAction(CYGNODetectorConstruction* myDC)
  : G4UserEventAction(), fDetector(myDC)
{
  // hits collections
  CYGNOID = -1;
}

CYGNOEventAction::~CYGNOEventAction()
{
}

void CYGNOEventAction::BeginOfEventAction(const G4Event* evt)
{

  //New event, add the user information object
  G4EventManager::
    GetEventManager()->SetUserInformation(new CYGNOUserEventInformation);

  if ( CYGNOID == -1) {
    G4SDManager * SDman = G4SDManager::GetSDMpointer();
    CYGNOID = SDman->GetCollectionID("CYGNOCollection");
  } 


}

void CYGNOEventAction::EndOfEventAction(const G4Event* evt)
{
  G4int event_id = evt->GetEventID();

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
  G4double totEnergy         = 0.;
  G4double totEnergyNR       = 0.;
  G4double hitEnergy         = 0.;

  v_pdgID_hits.clear();
  v_tracklen_hits.clear();
  v_px_particle.clear();
  v_py_particle.clear();
  v_pz_particle.clear();
  v_energyDep_hits.clear();
  v_x_hits.clear();
  v_y_hits.clear();
  v_z_hits.clear();

  if(CYGNOHC) {
    CYGNO_hits = CYGNOHC->entries();
        
    if (CYGNO_hits > 0) {
      
      man->FillNtupleIColumn(2,0,event_id);
      man->FillNtupleIColumn(2,1,CYGNO_hits);
      man->FillNtupleDColumn(2,2,(*CYGNOHC)[0]->GetKineticEne());
      man->FillNtupleIColumn(2,3,(*CYGNOHC)[0]->GetParticleID());
      G4cout << "firstParticleE =" << (*CYGNOHC)[0]->GetKineticEne() << "keV" << G4endl;
      
      for (G4int i=0; i<CYGNO_hits; i++) {
        
        hitEnergy         = (*CYGNOHC)[i]->GetEdep();
        totEnergy        += hitEnergy;
        
        v_pdgID_hits.push_back((*CYGNOHC)[i]->GetParticleID());
        v_tracklen_hits.push_back((*CYGNOHC)[i]->GetLength());
        v_px_particle.push_back((*CYGNOHC)[i]->GetMom().x());
        v_py_particle.push_back((*CYGNOHC)[i]->GetMom().y());
        v_pz_particle.push_back((*CYGNOHC)[i]->GetMom().z());
        v_energyDep_hits.push_back(hitEnergy);
        v_x_hits.push_back((*CYGNOHC)[i]->GetPos().x());
        v_y_hits.push_back((*CYGNOHC)[i]->GetPos().y());
        v_z_hits.push_back((*CYGNOHC)[i]->GetPos().z());

        if ((int)(*CYGNOHC)[i]->GetParticleID()>1000000000){
          totEnergyNR += hitEnergy;
        }
      }
      man->FillNtupleDColumn(2,4,totEnergy);
      man->FillNtupleDColumn(2,5,totEnergyNR);
      man->AddNtupleRow(2);
    }
  }
}