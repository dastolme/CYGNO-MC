//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "globals.hh"
#include "CYGNOPhysicsList.hh"

#include "G4DecayPhysics.hh"//Already included in any physics list
#include "G4RadioactiveDecayPhysics.hh"//Already included in Shielding
#include "G4OpticalPhysics.hh" //this is for scintillation light: eV energies

//EM lists
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmParameters.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmDNAPhysics_option2.hh"
#include "G4EmDNAPhysics_option4.hh"
#include "G4EmDNAPhysics_option6.hh"

//Hadronic lists
#include "G4HadronPhysicsShielding.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4IonPhysics.hh"//Not very precised. There is a better simulation based on QMD
#include "G4IonQMDPhysics.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronElasticPhysicsLEND.hh"
#include "G4hMultipleScattering.hh"
#include "G4ionIonisation.hh"
#include "G4BetheBlochModel.hh"

#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
//#include "MyOpBoundaryProcess.hh"
#include "G4EmSaturation.hh"
#include "G4OpWLS.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicsListHelper.hh"
/*
#include "MyCerenkov.hh"
#include "MyOpWLS.hh"
*/

#include "CYGNOStepMax.hh"
#include "G4UnitsTable.hh"

//Radioactivity
// #include "G4Radioactivation.hh"
// #include "G4RadioactiveDecay.hh"


G4ThreadLocal G4Cerenkov* CYGNOPhysicsList::fCerenkovProcess = 0;
G4ThreadLocal G4Scintillation* CYGNOPhysicsList::fScintillationProcess = 0;
G4ThreadLocal G4OpAbsorption* CYGNOPhysicsList::fAbsorptionProcess = 0;
G4ThreadLocal G4OpMieHG* CYGNOPhysicsList::fMieHGScatteringProcess = 0;
G4ThreadLocal G4OpRayleigh* CYGNOPhysicsList::fRayleighScatteringProcess = 0;
G4ThreadLocal G4OpWLS* CYGNOPhysicsList::fWLSProcess = 0;
G4ThreadLocal G4OpBoundaryProcess* CYGNOPhysicsList::fBoundaryProcess = 0;
//G4ThreadLocal MyOpBoundaryProcess* CYGNOPhysicsList::fMyBoundaryProcess = 0;

//G4ThreadLocal CYGNOStepMax* CYGNOPhysicsList::fStepMaxProcess = 0;
G4ThreadLocal CYGNOStepMax* CYGNOPhysicsList::fStepMaxProcess = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CYGNOPhysicsList::CYGNOPhysicsList(G4int verbose, G4String LEN_model, G4String HadrPhysVariant) 
  : G4VModularPhysicsList(){
  
  //  pMessenger = new CYGNOPhysicsListMessenger(this);

  // Create a modular physics list and register some modules
  
  SetVerboseLevel(verbose);
  G4LossTableManager::Instance()->SetVerbose(verbose);  
  
  
  // Default Decay Physics
  RegisterPhysics(new G4DecayPhysics());
  // Default Radioactive Decay Physics
  RegisterPhysics(new G4RadioactiveDecayPhysics());
  //RegisterPhysics(new CYGNOTritiumPhysics());

  //EM Physics
  //RegisterPhysics(new G4EmStandardPhysics());
  RegisterPhysics(new G4EmStandardPhysics_option4(verbose));//recommended option
  //RegisterPhysics(new G4EmDNAPhysics_option2(verbose));//GEANT4-DNA 
  //RegisterPhysics(new G4EmStandardPhysicsSS(verbose));//single scattering
  //RegisterPhysics(new G4EmPenelopePhysics());
  //RegisterPhysics(new G4EmLivermorePhysics());
  //RegisterPhysics(new G4EmLowEPPhysics());

  //Had Physics
  size_t find = LEN_model.find("LEND__");
  G4String evaluation;
  if ( find != G4String::npos )
  {
      evaluation=LEN_model;
      evaluation.erase(0,find+6);
      LEN_model="LEND";
  }

  // Hadron Elastic scattering
  if ( LEN_model == "HP" ) 
  {
     RegisterPhysics( new G4HadronElasticPhysicsHP(verbose) ); //this is the one used by default
  }
  else if ( LEN_model == "LEND" ) 
  {
     RegisterPhysics( new G4HadronElasticPhysicsLEND(verbose,evaluation) );
  }
  else 
  {
     G4cout << "Shielding Physics List: Warning!" <<G4endl;
     G4cout << "\"" << LEN_model << "\" is not valid for the low energy neutorn model." <<G4endl;
     G4cout << "Neutron HP package will be used." <<G4endl;
     RegisterPhysics( new G4HadronElasticPhysicsHP(verbose) );
  } 

  G4HadronPhysicsShielding* hps;
  // Hadron Physics
  if (HadrPhysVariant == "M") {
    hps = new G4HadronPhysicsShielding("hInelastic Shielding", verbose, 9.5*GeV, 9.9*GeV);
  } else {
    hps = new G4HadronPhysicsShielding("hInelastic Shielding", verbose, 4.0*GeV, 5.0*GeV);
  }
  if ( LEN_model == "HP" ) 
  {
     ;
  }
  else if ( LEN_model == "LEND" ) 
  {
     hps->UseLEND(evaluation); 
  }
  else 
  {
     //G4cout << "Shielding Physics List: Warning." <<G4endl;
     //G4cout << "Name of Low Energy Neutron model " << LEN_model << " is invalid." <<G4endl;
     //G4cout << "Will use neutron HP package." <<G4endl;
  }
  RegisterPhysics( hps );
  //RegisterPhysics(new G4HadronPhysicsFTFP_BERT_HP());
  //RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());

  if ( LEN_model == "HP" ) {
     //Activate prodcuton of fission fragments in neutronHP
     char env_ff[]="G4NEUTRONHP_PRODUCE_FISSION_FRAGMENTS=1";
     putenv(env_ff);
  }

  // Stopping Physics
  //RegisterPhysics( new G4StoppingPhysics(verbose) );

  // Ion Physics
  ////RegisterPhysics( new G4IonPhysics(verbose));//Less accurate
  //RegisterPhysics( new G4IonQMDPhysics(verbose));  
  //RegisterPhysics( new G4IonElasticPhysics(verbose));

  //Low energy ion
  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while( (*particleIterator)() ){
    G4ParticleDefinition* particle = particleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    G4String particleType = particle->GetParticleType();
    G4double charge = particle->GetPDGCharge();

    if(particleName == "alpha"      ||
             particleName == "deuteron"   ||
             particleName == "triton"     ||
             particleName == "He3")
      {
        //multiple scattering
        pmanager->AddProcess(new G4hMultipleScattering,-1,1,1);

        //ionisation
        G4ionIonisation* ionIoni = new G4ionIonisation();
        ionIoni->SetEmModel(new G4BetheBlochModel());
        ionIoni->SetStepFunction(1e-5, 0.1*um);
        pmanager->AddProcess(ionIoni,                   -1, 2, 2);
      }
    else if (particleName == "GenericIon")
      {
        // OBJECT may be dynamically created as either a GenericIon or nucleus
        // G4Nucleus exists and therefore has particle type nucleus
        // genericIon:

        //multiple scattering
        pmanager->AddProcess(new G4hMultipleScattering,-1,1,1);

        //ionisation
        G4ionIonisation* ionIoni = new G4ionIonisation();
        ionIoni->SetEmModel(new G4IonParametrisedLossModel());
        ionIoni->SetStepFunction(1e-5, 0.1*um);
        pmanager->AddProcess(ionIoni,                   -1, 2, 2);
      }
  }
  // Neutron tracking cut --> not by default
  // RegisterPhysics( new G4NeutronTrackingCut(verbose));

  //Others
  //RegisterPhysics(new G4OpticalPhysics());
  // Synchroton Radiation & GN Physics
  RegisterPhysics(new G4EmExtraPhysics());


  // Em options  
  //emOptions.SetMaxEnergy(100*GeV);
  //emOptions..SetDEDXBinning(200);
  //emOptions.SetLambdaBinning(200);//TO BE TESTED
  //emOptions.SetDeexcitationActiveRegion(true); //TBC
  G4EmParameters* emParameters = G4EmParameters::Instance();
  emParameters->SetFluo(true);
  emParameters->SetPixe(true);
  emParameters->SetAuger(true);

  defaultCutValue = 0.001*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;


  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CYGNOPhysicsList::~CYGNOPhysicsList()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CYGNOPhysicsList::ConstructParticle()
{
  G4VModularPhysicsList::ConstructParticle();
  G4OpticalPhoton::Definition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CYGNOPhysicsList::ConstructProcess()
{
/*  AddTransportation();
  emPhysicsList->ConstructProcess();
  AddDecay();  
*/
  G4VModularPhysicsList::ConstructProcess();
  AddStepMax();
  //ConstructOp();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void CYGNOPhysicsList::ConstructOp()
{
  //Don't forget to call within ConstructProcess()!
  fCerenkovProcess           = new G4Cerenkov("Cerenkov");
  //MyCerenkov* fCerenkovProcess           = new MyCerenkov("Cerenkov");
  fScintillationProcess = new G4Scintillation("Scintillation");
  fAbsorptionProcess         = new G4OpAbsorption();
  fRayleighScatteringProcess = new G4OpRayleigh();
  fMieHGScatteringProcess    = new G4OpMieHG();
  fBoundaryProcess = new G4OpBoundaryProcess();
  //fMyBoundaryProcess = new MyOpBoundaryProcess();
  fWLSProcess = new G4OpWLS();
  //MyOpWLS* fWLSProcess = new MyOpWLS();

//  fCerenkovProcess->DumpPhysicsTable();
//  fScintillationProcess->DumpPhysicsTable();
//  fRayleighScatteringProcess->DumpPhysicsTable();

  
  fCerenkovProcess->SetMaxNumPhotonsPerStep(200);
  fCerenkovProcess->SetMaxBetaChangePerStep(10.0);
  fCerenkovProcess->SetTrackSecondariesFirst(true);
  //fCerenkovProcess->SetVerboseLevel(2);
  
  // fScintillationProcess->SetScintillationYieldFactor(1.);
  fScintillationProcess->SetTrackSecondariesFirst(true);

  // Use Birks Correction in the Scintillation process

  //load the Ex/Em data into memory:
  //fWLSProcess->SetExEmData("ExEmMatrix.root");

  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  fScintillationProcess->AddSaturation(emSaturation);

  auto theParticleIterator=GetParticleIterator();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    //G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper(); 
    
    if (fCerenkovProcess->IsApplicable(*particle)) {
      //G4cout << "Adding Cherenkov to " << particleName << G4endl;
      ph->RegisterProcess(fCerenkovProcess, particle);
      //pmanager->AddProcess(fCerenkovProcess);
      //pmanager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
    }
    if (fScintillationProcess->IsApplicable(*particle)) {
      //G4cout << "Adding Scintillation to " << particleName << G4endl;
      ph->RegisterProcess(fScintillationProcess, particle);
      //pmanager->AddProcess(fScintillationProcess);
      //pmanager->SetProcessOrderingToLast(fScintillationProcess, idxAtRest);
      //pmanager->SetProcessOrderingToLast(fScintillationProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
      //G4cout << "Adding OpticalPhoton processes." << G4endl;
      /*pmanager->AddDiscreteProcess(fAbsorptionProcess);
      pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(fMieHGScatteringProcess);
      pmanager->AddDiscreteProcess(fBoundaryProcess);
      pmanager->AddDiscreteProcess(fWLSProcess);
      */
      ph->RegisterProcess(fAbsorptionProcess, particle);
      ph->RegisterProcess(fRayleighScatteringProcess, particle);
      ph->RegisterProcess(fMieHGScatteringProcess, particle);
      ph->RegisterProcess(fBoundaryProcess, particle);
      ph->RegisterProcess(fWLSProcess, particle);
    }
  }
}

void CYGNOPhysicsList::SetCuts()
{
  
  if (verboseLevel >0){
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }
  
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  
  if (verboseLevel>0) DumpCutValuesTable();
  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void CYGNOPhysicsList::AddDecay()
{
  // Add Decay Process

  G4Decay* fDecayProcess = new G4Decay();
  G4RadioactiveDecay* radDecayProc = new G4RadioactiveDecay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (fDecayProcess->IsApplicable(*particle) && !particle->IsShortLived()) { 

      pmanager ->AddProcess(fDecayProcess);

      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);

    }

    if (particle->GetParticleName() == "GenericIon") { 

      pmanager ->AddDiscreteProcess(radDecayProc);
      
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(radDecayProc, idxPostStep);
      pmanager ->SetProcessOrdering(radDecayProc, idxAtRest);
      
    }
  }
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CYGNOPhysicsList::AddStepMax()
{
  // Step limitation seen as a process
  CYGNOStepMax* stepMaxProcess = new CYGNOStepMax();

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  auto theParticleIterator=GetParticleIterator();
  theParticleIterator->reset();
  while ((*theParticleIterator)()){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (stepMaxProcess->IsApplicable(*particle) && !particle->IsShortLived())
        {
	  pmanager ->AddDiscreteProcess(stepMaxProcess);
	}
  }
}

