#include "CYGNOPrimaryGeneratorAction.hh"
#include "CYGNODetectorConstruction.hh"
#include "Randomize.hh"
#include "globals.hh"
#include <math.h>
#include "G4GeneralParticleSource.hh"
#include "G4ThreeVector.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4TransportationManager.hh"
#include "G4RunManager.hh"
#include "G4Gamma.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "CYGNOEventAction.hh"
#include "G4AnalysisManager.hh"

#define PI 3.14159265
using namespace std;

CYGNOPrimaryGeneratorAction::CYGNOPrimaryGeneratorAction(CYGNODetectorConstruction* myDC):myDetector(myDC)
{
  n_particle = 1;
  particleGun = new G4GeneralParticleSource();
  // fMessenger = new CYGNOPrimaryGeneratorActionMessenger(this);

  // default particle
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e-");
  particleGun->SetParticleDefinition(particle);
  particle_energy = 1.0*MeV;

}

CYGNOPrimaryGeneratorAction::~CYGNOPrimaryGeneratorAction()
{
  // delete fMessenger;
  delete particleGun;
}

void CYGNOPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  // Generate primary vertex
  particleGun->GeneratePrimaryVertex(anEvent);

  if (fFillNtuple) {
    energy_pri = 0.;

    // Access particle energy
    energy_pri = particleGun->GetParticleEnergy();

    // Access particle position
    G4ThreeVector particlePosition = particleGun->GetParticlePosition();
    
    //Fill ntuple #1
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(1,0,energy_pri);
    man->FillNtupleDColumn(1,1,particlePosition[0]);
    man->FillNtupleDColumn(1,2,particlePosition[1]);
    man->FillNtupleDColumn(1,3,particlePosition[2]);
    man->AddNtupleRow(1);
  }

}

