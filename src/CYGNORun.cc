#include "CYGNORun.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "CYGNOHit.hh"
#include "G4HCofThisEvent.hh"
#include "CYGNOAnalysis.hh"
#include "G4Threading.hh"

CYGNORun::CYGNORun() :
    G4Run(),
    CYGNOID(-1)
{ }


void CYGNORun::RecordEvent(const G4Event* evt)
{
  G4Run::RecordEvent(evt);

  if ( CYGNOID == -1) {
    G4SDManager * SDman = G4SDManager::GetSDMpointer();
    CYGNOID = SDman->GetCollectionID("CYGNOCollection");
  } 
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if (!HCE) {
    G4ExceptionDescription msg;
    msg << "No hits collection of this event found.\n";
    G4Exception("Run::RecordEvent()",
    "Code001", JustWarning, msg);
    return;
  }


CYGNOHitsCollection* CYGNOHC = 0;
if (CYGNOID != -1) CYGNOHC = static_cast<CYGNOHitsCollection*>(HCE->GetHC(CYGNOID));
//this is just in case we want to do something with the event in multiple runs

}

void CYGNORun::Merge(const G4Run* aRun)
{
  const CYGNORun* localRun = static_cast<const CYGNORun*>(aRun);
}
