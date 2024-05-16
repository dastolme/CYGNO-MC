#include "CYGNORunAction.hh"
#include "CYGNORunActionMessenger.hh"
#include "CYGNOAnalysis.hh"
#include "CYGNODetectorConstruction.hh"

#include "CYGNORun.hh"

CYGNORunAction::CYGNORunAction(CYGNODetectorConstruction* myDC)
  : G4UserRunAction(), fDetector(myDC) 
{
  fMessenger = new CYGNORunActionMessenger(this);
}

CYGNORunAction::~CYGNORunAction()
{
  delete fMessenger;
}

void CYGNORunAction::BeginOfRunAction(const G4Run* aRun)
{
	//Master mode or sequential
  if (IsMaster())    
    G4cout << "### Run " << aRun->GetRunID() << " starts (master)." << G4endl;
  else
    G4cout << "### Run " << aRun->GetRunID() << " starts (worker)." << G4endl;
  
  // Book histograms and ntuples
  Book();

}

void CYGNORunAction::EndOfRunAction(const G4Run* run)
{ 
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();
}

G4Run* CYGNORunAction::GenerateRun() {
    return new CYGNORun;
}

void CYGNORunAction::Book() {
  
  // Get/create analysis manager
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->SetDefaultFileType("root");
  
  man->SetNtupleMerging(true);

  man->SetFirstNtupleId(1);


  // ---- primary ntuple ------
  // id==1
  man->CreateNtuple("tree1", "Particle Source Info");
  man->CreateNtupleDColumn("Energy");
  man->CreateNtupleDColumn("xpos_vertex");
  man->CreateNtupleDColumn("ypos_vertex");
  man->CreateNtupleDColumn("zpos_vertex");
  man->FinishNtuple();

  // ---- secondary ntuple ------   
  //id==2
  man->CreateNtuple("tree2", "Hits Info");
  man->CreateNtupleDColumn("Event");
  man->CreateNtupleDColumn("energy_pri");
  man->CreateNtupleDColumn("e_dep");
  man->CreateNtupleDColumn("e_dep_NR");
  man->CreateNtupleDColumn("firstparticleID");
  man->FinishNtuple();

  // ---- tertiary ntuple ------   
  //id==3
  man->CreateNtuple("tree3", "Incoming particle Info");
  man->CreateNtupleSColumn("particle_name");
  man->CreateNtupleDColumn("energy");
  man->FinishNtuple();

  // Open an output file
  man->OpenFile(FileName);

  return;

}

