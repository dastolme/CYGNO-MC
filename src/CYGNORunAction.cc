#include "CYGNORunAction.hh"
#include "CYGNOEventAction.hh"
#include "CYGNORunActionMessenger.hh"
#include "CYGNODetectorConstruction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"


CYGNORunAction::CYGNORunAction(CYGNOEventAction* myEA, CYGNODetectorConstruction* myDC)
  : G4UserRunAction(), fDetector(myDC), fEventAction(myEA)
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
  man->CreateNtuple("nTuple", "Hits Info");
  man->CreateNtupleIColumn("eventnumber");
  man->CreateNtupleIColumn("numhits");
  man->CreateNtupleDColumn("ekin_particle");
  man->CreateNtupleIColumn("particle_type");
  man->CreateNtupleDColumn("energyDep");
  man->CreateNtupleDColumn("energyDep_NR");
  man->CreateNtupleIColumn("pdgID_hits", fEventAction -> Get_pdgID_hits());
  man->CreateNtupleDColumn("tracklen_hits", fEventAction -> Get_tracklen_hits());
  man->CreateNtupleDColumn("px_particle", fEventAction -> Get_px_particle());
  man->CreateNtupleDColumn("py_particle", fEventAction -> Get_py_particle());
  man->CreateNtupleDColumn("pz_particle", fEventAction -> Get_pz_particle());
  man->CreateNtupleDColumn("energyDep_hits", fEventAction -> Get_energyDep_hits());
  man->CreateNtupleDColumn("x_hits", fEventAction -> Get_x_hits());
  man->CreateNtupleDColumn("y_hits", fEventAction -> Get_y_hits());
  man->CreateNtupleDColumn("z_hits", fEventAction -> Get_z_hits());
  man->FinishNtuple();

  // Open an output file
  man->OpenFile(FileName);

  return;

}

