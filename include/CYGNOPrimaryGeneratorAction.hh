#ifndef CYGNOPrimaryGeneratorAction_h
#define CYGNOPrimaryGeneratorAction_h 1

//#include "CYGNOGeneratorPositionSampling.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include <fstream>
#include <stdio.h>
#include <vector>
#include "globals.hh"

class CYGNOPrimaryGeneratorActionMessenger;
class CYGNODetectorConstruction;
class G4GeneralParticleSource;
class G4Event;

class CYGNOPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    CYGNOPrimaryGeneratorAction(CYGNODetectorConstruction*);    
   ~CYGNOPrimaryGeneratorAction();

  public:
  void GeneratePrimaries(G4Event*);  
  void SetEnergy(G4double ene) {particle_energy = ene;}
  G4double GetEnergy() {return particle_energy;}

  void SetFillNtuple(bool fill) { fFillNtuple = fill; }

  private:
  G4GeneralParticleSource* particleGun;
  CYGNODetectorConstruction* myDetector;
  G4int n_particle;
  G4double particle_energy;

  bool fFillNtuple;
  G4double energy_pri;

  G4String filename; // Member variable to store the filename

  std::ifstream inputFile;
 
  G4ThreeVector position;
};

#endif


