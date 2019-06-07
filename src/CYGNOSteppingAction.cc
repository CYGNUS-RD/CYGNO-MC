#include "CYGNOSteppingAction.hh"
#include "CYGNOVolumes.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Ions.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

CYGNOSteppingAction::CYGNOSteppingAction(CYGNODetectorConstruction* det)://, CYGNOEventAction* evt ):
fDetector(det) {}//, fEventAction(evt){ }

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

  //if (fTrack->GetDefinition()->GetParticleType() == "nucleus" && fStep->GetTrack()->GetParentID()>0)
  //  {
  //    //G4cout <<"StepNo "<<StepNo<<" secondary nucleus "<<fTrack->GetDefinition()->GetParticleName()<<G4endl;
  //    G4double energy = fTrack->GetKineticEnergy();
  //    if (energy < 0.1*keV) // FIXME: check this value of energy
  //      {
  //        G4Ions* ion = (G4Ions*) fTrack->GetDefinition();
  //        G4double lifetime = ion->GetPDGLifeTime();
  //        G4double excitationEnergy = ion->GetExcitationEnergy();

  //        //stable and excited nuclei --> track them as usual 
  //        //if (lifetime < 0 || excitationEnergy > 0) return;
  //        if (lifetime < 0 || (excitationEnergy > 0 && fTrack->GetDefinition()->GetParticleName()!="Pa234[73.920]")) return;
  //       
  //        //                                                                                             
  //                  if (lifetime > 1.0*microsecond) //kill long-lived nuclei
  //                    {
  //                      G4String particleName = fTrack->GetDefinition()->GetParticleName();
  //                      // old killing
  //        	      //fTrack->SetTrackStatus(fStopAndKill);
  //        	      // new killing
  //        	      //Notice: the StepAction is not called at step#0. Secondaries
  //        	      //are generated if RadioactiveDecay takes place at step#1
  //        	      G4TrackStatus newstatus =
  //        	      (fTrack->GetCurrentStepNumber() > 1) ?
  //        	      fStopAndKill : fKillTrackAndSecondaries;
  //        	      fTrack->SetTrackStatus(newstatus);
  //        	    }
  //        
  //        //  else if (lifetime < 1.0*microsecond && lifetime > 0) //decay short-lived nuclei            
  //        //        {                                                                                    
  //        //          G4String particleName = fStep->GetTrack()->GetDefinition()->GetParticleName();     
  //        //          fStep->GetTrack()->SetTrackStatus(fStopButAlive);                                  
  //        //          //G4cout << "Allows decay of track: " << particleName << " (life time: " <<        
  //        //          //        lifetime/s << " s)" << G4endl;                                           
  //        //        }                                                                                    

  //        //stable and short-lived nuclei are unaffected                                                 
  //      }
  //  }

  
  G4int trackID = fTrack->GetTrackID();
  G4int nextVolNo = analysis->GetVolNo(fTrack);
  G4int nextCopyNo = analysis->GetCopyNo(fTrack);
  G4int PDGcode = fTrack->GetDefinition()->GetPDGEncoding();
  G4ThreeVector preStepPoint = fStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector postStepPoint = fStep->GetPostStepPoint()->GetPosition();
  G4LorentzVector quadriMom = fTrack->GetDynamicParticle()->Get4Momentum();


  if((name=="expHall" && nextname=="Shield0") || 
     (name=="Shield0" && nextname=="Shield1") || 
     (name=="Shield1" && nextname=="Shield2") || 
     (name=="Shield2" && nextname=="Shield3") || 
     (name=="Shield3" && nextname=="AirBox")  ||
     (name=="cad_shell_physical" && nextname=="CYGNO_gas")) {

     //G4cout << "trackID = " << trackID << "  vol name = " << name << "  next vol name = " << nextname << G4endl;
     analysis->RegisterParticle(trackID, nextVolNo, nextCopyNo, PDGcode, preStepPoint, postStepPoint, quadriMom);
    
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
      if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
        {
	  G4ThreeVector postStpPt = fTrack->GetPosition();
	  G4LorentzVector fourMom = fTrack->GetDynamicParticle()->Get4Momentum();
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
  if (fTrack->GetDefinition() == G4Proton::Definition())
    {
      if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
        {
	  analysis->SetProtonFlag(1);
        }
    }
  if (fTrack->GetDefinition() == G4MuonMinus::Definition() ||
      fTrack->GetDefinition() == G4MuonPlus::Definition())
    {
      if (fTrack->GetMaterial()->GetName() == "CYGNO_gas")
        {
	  analysis->SetMuonFlag(1);
        }
    }
  if ((fTrack->GetMaterial()->GetName() == "CYGNO_gas") &&
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

  if(StepNo >= 10000) fTrack->SetTrackStatus(fStopAndKill);
}

