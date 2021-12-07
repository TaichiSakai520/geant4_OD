// ====================================================================
//                      PrimaryGeneratorAction.cc
// ====================================================================
#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "G4ParticleMomentum.hh"
 
// ====================================================================
//                           class description
// ====================================================================

////////////////////////////////////////////////
PrimaryGeneratorAction::PrimaryGeneratorAction()
////////////////////////////////////////////////
{
  particleGun= new G4ParticleGun;
}

/////////////////////////////////////////////////
PrimaryGeneratorAction::~PrimaryGeneratorAction()
/////////////////////////////////////////////////
{
  delete particleGun;
}

////////////////////////////////////////////////////////////////
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
////////////////////////////////////////////////////////////////
{
  G4double pi=3.14159265358979323846;
  G4double costheta=1.0-2.0*G4UniformRand();
  G4double phi=2.0*pi*G4UniformRand();
  G4double sintheta=sqrt((1.0-costheta)*(1.0+costheta));
  
  particleGun->SetParticleMomentumDirection(G4ParticleMomentum(cos(phi)*sintheta,sin(phi)*sintheta,costheta));
  particleGun-> GeneratePrimaryVertex(anEvent);

}

