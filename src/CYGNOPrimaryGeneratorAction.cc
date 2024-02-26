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

#define PI 3.14159265
using namespace std;

CYGNOPrimaryGeneratorAction::CYGNOPrimaryGeneratorAction(CYGNODetectorConstruction* myDC):myDetector(myDC)
{
  n_particle = 1;
  particleGun = new G4GeneralParticleSource();
  //fMessenger = new CYGNOPrimaryGeneratorActionMessenger(this);

  // default particle
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e-");
  particleGun->SetParticleDefinition(particle);
  particle_energy = 1.0*MeV;

}

CYGNOPrimaryGeneratorAction::~CYGNOPrimaryGeneratorAction()
{
  //delete fMessenger;
  delete particleGun;
  // inputFile.close();
}

void CYGNOPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  G4int numParticles=1;
  particleGun->SetNumberOfParticles(numParticles);

  // Read position from binary file
  // G4ThreeVector position;
  // double x, y, z;

  // inputFile.read(reinterpret_cast<char*>(&x), sizeof(double));
  // inputFile.read(reinterpret_cast<char*>(&y), sizeof(double));
  // inputFile.read(reinterpret_cast<char*>(&z), sizeof(double));

  // Set position dynamically in GPS
  // position.set(x, y, z);
  // particleGun->SetParticlePosition(position);

  // Generate primary vertex
  particleGun->GeneratePrimaryVertex(anEvent);
}

void CYGNOPrimaryGeneratorAction::SetFilename(const G4String& newFilename) {
  filename = newFilename;
  inputFile.open(filename, std::ios::binary);

  if (!inputFile.is_open()) {
      G4Exception("CYGNOPrimaryGeneratorAction::SetFilename",
                  "FileOpenError", FatalException, "Failed to open binary file.");
  }
}

