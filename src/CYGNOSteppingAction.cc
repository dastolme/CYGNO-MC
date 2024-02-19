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

CYGNOSteppingAction::CYGNOSteppingAction(CYGNODetectorConstruction* det)://, CYGNOEventAction* evt ):
fDetector(det) {energyOutputFile.open("gamma_spectrum.txt", std::ios::app);}//, fEventAction(evt){ }

void CYGNOSteppingAction::UserSteppingAction(const G4Step* fStep)
{ 
  // get analysis instance
  CYGNOAnalysis* analysis = CYGNOAnalysis::getInstance();

 // get volume of the current step
  G4VPhysicalVolume* volume = fStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4VPhysicalVolume* nextvolume = fStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  G4String name = volume->GetName();
  G4String nextname = "OutOfWorld";
  if(nextvolume) nextname=nextvolume->GetName();

  G4Track* fTrack = fStep->GetTrack();
  G4int StepNo = fTrack->GetCurrentStepNumber();

  // if (fTrack->GetDefinition()->GetParticleType() == "nucleus" && fStep->GetTrack()->GetParentID()>0)
  // {
    //G4cout <<"StepNo "<<StepNo<<" secondary nucleus "<<fTrack->GetDefinition()->GetParticleName()<<G4endl;
    // G4double energy = fTrack->GetKineticEnergy();
    // if (energy < 0.1*keV) // FIXME: check this value of energy
    //   {
    //     G4Ions* ion = (G4Ions*) fTrack->GetDefinition();
    //     G4double lifetime = ion->GetPDGLifeTime();
    //     G4double excitationEnergy = ion->GetExcitationEnergy();

        //stable and excited nuclei --> track them as usual 
        //if (lifetime < 0 || excitationEnergy > 0) return;
        // if (lifetime < 0 || (excitationEnergy > 0 && fTrack->GetDefinition()->GetParticleName()!="Pa234[73.920]")) return;
        
        //                                                                                             
                  // if (lifetime > 1.0*microsecond) //kill long-lived nuclei
                  //   {
                  //     G4String particleName = fTrack->GetDefinition()->GetParticleName();
                      // old killing
                //fTrack->SetTrackStatus(fStopAndKill);
                // new killing
                //Notice: the StepAction is not called at step#0. Secondaries
                //are generated if RadioactiveDecay takes place at step#1
                // G4TrackStatus newstatus =
                // (fTrack->GetCurrentStepNumber() > 1) ?
                // fStopAndKill : fKillTrackAndSecondaries;
                // fTrack->SetTrackStatus(newstatus);
        // if (lifetime > 1.0 * microsecond) {
        //     G4String particleName = fTrack->GetDefinition()->GetParticleName();
        //     // Stop but keep alive
        //     fTrack->SetTrackStatus(fStopButAlive);
        
        // }
        
        //  else if (lifetime < 1.0*microsecond && lifetime > 0) //decay short-lived nuclei            
        //        {                                                                                    
        //          G4String particleName = fStep->GetTrack()->GetDefinition()->GetParticleName();     
        //          fStep->GetTrack()->SetTrackStatus(fStopButAlive);                                  
        //          //G4cout << "Allows decay of track: " << particleName << " (life time: " <<        
        //          //        lifetime/s << " s)" << G4endl;                                           
        //        }                                                                                    

        //stable and short-lived nuclei are unaffected                                                 
  //     }
  // }
  
  G4int trackID = fTrack->GetTrackID();
  G4int preVolNo = analysis->GetPreVolNo(fTrack);
  G4int nextVolNo = analysis->GetVolNo(fTrack);
  G4int nextCopyNo = analysis->GetCopyNo(fTrack);
  G4int PDGcode = fTrack->GetDefinition()->GetPDGEncoding();
  G4ThreeVector preStepPoint = fStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector postStepPoint = fStep->GetPostStepPoint()->GetPosition();
  G4LorentzVector quadriMom = fTrack->GetDynamicParticle()->Get4Momentum();
  G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();


  if((name!="Shield0" && nextname=="Shield0") || 
     (name!="Shield1" && nextname=="Shield1") || 
     (name!="Shield2" && nextname=="Shield2") || 
     (name!="Shield3" && nextname=="Shield3") || 
     (name!="AirBox" && nextname=="AirBox") || 
     (name!="CYGNO_gas" && nextname=="CYGNO_gas")) {

     //G4cout << "trackID = " << trackID << "  vol name = " << name << "  next vol name = " << nextname << G4endl;
     analysis->RegisterParticle(trackID, preVolNo, nextVolNo, nextCopyNo, PDGcode, preStepPoint, postStepPoint, quadriMom);
    
  }

  // Get the particles current across a 70.cm radius sphere surrounding the sensitive gas. Assuming gas to be placed around the center of the global ref frame origin
  // G4double sphereRadius = 70.0*cm;
  //if (preStepPoint.mag() >= sphereRadius && postStepPoint.mag() < sphereRadius){
  //  G4int sphereNo = SPHERE;
  //  G4int sphereCopyNo = -11;
  //  analysis->RegisterParticle(sphereNo, sphereCopyNo, PDGcode, preStepPoint, postStepPoint, quadriMom);
  //}

  
  // Check for neutrons in the gas
  if (fTrack->GetDefinition() == G4Neutron::NeutronDefinition())
    {
      //if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
      if (name=="CYGNO_gas")
        {
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
          G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();
	  analysis->RegisterNeutron(fTrack->GetTrackID(), fTrack->GetParentID(), postStpPt, fourMom);
	  analysis->SetNeutronFlag(1);
        }
      ////// ??
      //G4double neuEnergy = fTrack->GetKineticEnergy();
      //if (neuEnergy > 19.999*MeV && neuEnergy < 20.001*MeV)
      //  {
      //    fTrack->SetKineticEnergy(19.99*MeV);
      //    //this is due to the fact that 20-MeV neutrons may enter in an infinite loop
      //  }
    }

  // register ionizing particles
  // Check for ions in the gas

  G4String particleType = fTrack->GetDefinition()->GetParticleType();
  //ions in the gas
  if (particleType == "nucleus")
    {
      //if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
      if (name=="CYGNO_gas")
        {
        //if (lifetime > 1e18*second) lifetime = -1; // do not consider very long-lived isotopes > 3e10 yrs
        //if (lifetime > 0 || excitationEnergy > 0)
        //{

          //}
          G4Ions* ion = (G4Ions*) fTrack->GetDefinition();
          G4double lifetime = ion->GetPDGLifeTime();
          G4double trackID = fTrack->GetTrackID();
          G4int pdg = fTrack->GetDefinition()->GetPDGEncoding();
          G4int volNo = analysis->GetVolNo(fTrack);
          G4int copyNo = analysis->GetCopyNo(fTrack);
          G4int A = ion->GetAtomicMass();
          G4int Z = ion->GetAtomicNumber();
          G4double excitationEnergy = ion->GetExcitationEnergy();
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
          G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();
	  analysis->RegisterIon(A, Z, pdg, volNo, copyNo, trackID, fTrack->GetParentID(), postStpPt, fourMom, kinE_prestep);
        }
    }

  // electrons
  if (fTrack->GetDefinition() == G4Electron::Definition())
    {
      //if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
      if (name=="CYGNO_gas")
        {
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
          G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();
	  analysis->RegisterElectron(fTrack->GetTrackID(), fTrack->GetParentID(), postStpPt, fourMom);
        }
    }

  // positrons
   if(fTrack->GetDefinition() == G4Positron::Definition()) 
   {
      //if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
      if (name=="CYGNO_gas")
        {
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
          G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();
	  analysis->RegisterPositron(fTrack->GetTrackID(), fTrack->GetParentID(), postStpPt, fourMom);
        }
   }

  
  // protons
  if (fTrack->GetDefinition() == G4Proton::Definition())
    {
      //if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
      if (name=="CYGNO_gas")
        {
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
          G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();
	  analysis->RegisterProton(fTrack->GetTrackID(), fTrack->GetParentID(), postStpPt, fourMom);
        }
    }

  // muons
  if (fTrack->GetDefinition() == G4MuonMinus::Definition() ||
      fTrack->GetDefinition() == G4MuonPlus::Definition())
    {
      //if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
      if (name=="CYGNO_gas")
        {
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
          G4double kinE_prestep = fStep->GetPreStepPoint()->GetKineticEnergy();
	  analysis->RegisterProton(fTrack->GetTrackID(), fTrack->GetParentID(), postStpPt, fourMom);
	  analysis->SetMuonFlag(1);
        }
    }
  
  //neutron capture flag
  //if ((fTrack->GetMaterial()->GetName() == "CYGNO_gas") &&
  if (name=="CYGNO_gas" &&
      (fTrack->GetDefinition() == G4Gamma::Definition()))
    {
      if (fTrack->GetParentID()>0)
        {
	  G4String processname = fTrack->GetCreatorProcess()->GetProcessName();
	  if (processname == "nCapture" || processname == "nInelastic")
            {
	      analysis->SetInelasticFlag(1);
            }
        }
    }

  // Get the track associated with the current step
  G4Track* track = fStep->GetTrack();
  
  // Check if it's a secondary gamma particle
  if (track->GetParentID() > 0) {  // Positive ParentID indicates secondary
      G4ParticleDefinition* particleDef = track->GetDefinition();
      if (particleDef == G4Gamma::GammaDefinition()) {
          G4double energy = track->GetKineticEnergy();

          // Print the energy in keV for gamma particles to the file
          energyOutputFile << energy / keV << std::endl;
      }
  }
}


