#include "CYGNODetectorConstruction.hh"
#include "CYGNOPhysicsList.hh"
#include "CYGNOPrimaryGeneratorAction.hh"
#include "CYGNOActionInitialization.hh"
#include "G4EmParameters.hh"
#include "G4PhysListFactory.hh"
//#ifdef G4MULTITHREADED
//#include "G4MTRunManager.hh"
//#else
#include "G4RunManager.hh"
//#endif

#include "G4Timer.hh"
#include "Randomize.hh"

#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
//#include "Shielding.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc,char** argv)
{

  //detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = nullptr;
  if (argc == 1) ui = new G4UIExecutive(argc,argv);

  //Instantiate the G4Timer object, to monitor the CPU time spent for 
  //the entire execution
  G4Timer* theTimer = new G4Timer();
  //Start the benchmark
  theTimer->Start();

   //
  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // User Verbose output class
  //
  //  G4VSteppingVerbose* verbosity = new G4VSteppingVerbose;
  //  G4VSteppingVerbose::SetInstance(verbosity);
  
  // Run manager
  
  //#ifdef G4MULTITHREADED
  //G4MTRunManager* runManager = new G4MTRunManager;
  //runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
  //#else
  G4RunManager * runManager = new G4RunManager;
  //#endif

  // User Initialization classes (mandatory)
  //
  CYGNODetectorConstruction* detector = new CYGNODetectorConstruction;
  runManager->SetUserInitialization(detector);

// #ifdef G4VIS_USE
  // visualization manager
  //
//   G4VisManager* visManager = new G4VisExecutive;
//   visManager->Initialize();
// #endif

  // PHYSICS
  // CYGNO physics list is the Shielding physics list with the addition of emlists option4 and of fluo,PIXE,Auger. In a second moment we can add the optical photons and simulate the scintillation
  // For the study of systematics, change Shielding to QGSP_BERT_HP and QGSP_BIC_HP, and the option4 to livermore and penelope.
  CYGNOPhysicsList* physics = new CYGNOPhysicsList;
  runManager->SetUserInitialization(physics);
   
  // User Action classes
  runManager->SetUserInitialization(new CYGNOActionInitialization(detector));

  // Create your custom primary generator action with the binary file name
  // CYGNOPrimaryGeneratorAction* customGenerator = new CYGNOPrimaryGeneratorAction(detector);
  // Set the filename for the binary file
  // customGenerator->SetFilename("output_binary.dat");
  // runManager->SetUserAction(customGenerator);

  // Initialize G4 kernel
  //
  runManager->Initialize();
      
  // Get the pointer to the User Interface manager
  //
  G4VisManager* visManager = nullptr;
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  

  if (ui)   // interactive mode : define visualization and UI terminal
    {
     visManager = new G4VisExecutive;
     visManager->Initialize();
     ui->SessionStart();
     delete ui;

     G4UIsession * session = 0;
     session = new G4UIterminal(); 
     UImanager->ApplyCommand("/control/execute vis.mac"); 
     session->SessionStart();
     delete session;

     delete visManager;
    }
    
  else           // batch mode
    {    
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UImanager->ApplyCommand(command+fileName);
    }

//Stop the benchmark here
theTimer->Stop();
G4cout << "The simulation took: " << theTimer->GetRealElapsed() << " s to run (real time)" << G4endl;

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
  //delete analysis;
  //delete runManager;
  //  delete verbosity;

  return 0;
}
