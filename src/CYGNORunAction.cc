#include "CYGNORunAction.hh"
#include "CYGNORunActionMessenger.hh"
#include "CYGNOAnalysis.hh"
#include "CYGNODetectorConstruction.hh"

#include "CYGNORun.hh"

CYGNORunAction::CYGNORunAction(CYGNODetectorConstruction* myDC)
  : G4UserRunAction(), fDetector(myDC) 
{

  fMessenger = new CYGNORunActionMessenger(this);
  fNumberOfMuonsDetector = 0;
  FileName = "out";
  fOutFileCut = 1;
  fRegisterOn = 1;
  fHitsInfo = 1;


}

CYGNORunAction::~CYGNORunAction()
{
  delete G4AnalysisManager::Instance();
  CYGNOAnalysis* analysis = CYGNOAnalysis::getInstance();
  delete analysis;//??? Is it the place where to do it???
  //delete fMessenger;
}

void CYGNORunAction::BeginOfRunAction(const G4Run* aRun)
{
	G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

	fNumberOfMuonsDetector = 0;

	//inform the runManager to save random number seed
	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);
	
	// Get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


	CYGNOAnalysis* analysis = CYGNOAnalysis::getInstance();
	analysis->SetOutFileCut(fOutFileCut);
	analysis->SetRegisterOn(fRegisterOn);
	analysis->SetHitsInfo(fHitsInfo);
	analysis->InitRun(FileName,fDetector);
	// Open an output file
	// The default file name is set in RunAction::RunAction(),
	// it can be overwritten in a macro
	analysisManager->OpenFile();
}

void CYGNORunAction::EndOfRunAction(const G4Run* run)
{ 
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

}

G4Run* CYGNORunAction::GenerateRun() {
    return new CYGNORun;
}

