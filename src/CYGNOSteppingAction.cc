#include "CYGNOSteppingAction.hh"
#include "CYGNOVolumes.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Ions.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "CYGNOEventAction.hh"
#include "G4EventManager.hh"

#include "G4DecayTable.hh"
#include "G4VDecayChannel.hh"
#include "G4DecayProducts.hh"

#include "G4ProcessType.hh"

CYGNOSteppingAction::CYGNOSteppingAction(CYGNODetectorConstruction* det)://, CYGNOEventAction* evt ):
fDetector(det) {;} //fEventAction(evt) {;}

void CYGNOSteppingAction::UserSteppingAction(const G4Step* fStep)
{
    G4Track* track = fStep->GetTrack();
    
    // Get the particle definition and the particle name
    G4ParticleDefinition* particleDefinition = track->GetDefinition();
    std::string particleName = particleDefinition->GetParticleName();

    // Check if the particle is an alpha
    if (particleDefinition->GetParticleName() == "alpha") {
        // Kill the alpha particle (stop tracking)
        track->SetTrackStatus(fStopAndKill);
    }

    // Check if the particle is a gamma
    if (particleName == "gamma") {
        // Get the pre-step point
        G4StepPoint* preStepPoint = fStep->GetPreStepPoint();
        
        // Get the volume the particle is entering
        G4LogicalVolume* preVolume = preStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
        std::string volumeName = preVolume->GetName();
        double energyMeV = track->GetKineticEnergy();
        
        // Convert the energy to keV
        double energyKeV = energyMeV / keV;
        
        // Check if the volume name is CYGNO_log
        if (volumeName == "CYGNO_log") {
            // Perform your actions here, e.g., logging or counting
            G4cout << "Gamma particle entered volume: CYGNO_log" << G4endl;
            
            G4AnalysisManager* man = G4AnalysisManager::Instance();
            //Fill ntuple #3
            man->FillNtupleSColumn(3,0,particleName);
            man->FillNtupleDColumn(3,1,energyKeV);
            man->AddNtupleRow(3);
        }
    }
}