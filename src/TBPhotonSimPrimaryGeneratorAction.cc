
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

#include "TBPhotonSimPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include "G4ParticleMomentum.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "TBPhotonSimSteppingAction.hh"
#include <fstream>
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include <vector>
#include <sstream>
//#define UniformDistribution
//#define OpticalMode
std::ifstream *TBPhotonSimPrimaryGeneratorAction::_ifs = NULL;
//////////////////////////////////////////////////////
TBPhotonSimPrimaryGeneratorAction::TBPhotonSimPrimaryGeneratorAction()
//////////////////////////////////////////////////////
{
  particleGun = new G4ParticleGun;
  //particleGun = new G4GeneralParticleSource;
  gNav = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  //fin = new std::ifstream("target_dr/MuonData_100000.dat_1");

#ifndef UniformDistribution
#ifndef OpticalMode
  _filename = "target_dr/MuonData_100000.dat_1";
  /*
  //fin = new std::ifstream("target_dr/1st_excited.dat");
  //fin = new std::ifstream("target_dr/2nd_excited.dat");
  */
#endif
#endif

#ifdef OpticalMode
  _filename = "photonSimdata/muon.dat";
#endif
}

///////////////////////////////////////////////////////
TBPhotonSimPrimaryGeneratorAction::~TBPhotonSimPrimaryGeneratorAction()
///////////////////////////////////////////////////////
{
  if (_ifs)
    delete _ifs;
  delete particleGun;
}

///////////////////////////////////////////////////////////////////
void TBPhotonSimPrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
///////////////////////////////////////////////////////////////////
{
  if (!_ifs)
  {
    _ifs = new std::ifstream(_filename.c_str());
    G4cout << _filename.c_str() << G4endl;
    if (!_ifs->good())
    {
      std::cerr << "File(" << _filename.c_str() << ") I/O Error" << std::endl;
      delete _ifs;
      abort();
    }
  }
#ifdef UniformDistribution
  G4double random_r, random_phi, random_z;
  G4ThreeVector particle_position;
  //G4double zmax = 23.0 * m;
  G4double zmax = 23.0 / 2 * m;
  G4double rmax = 12.5 * m;
  G4double R = 0.5 * rmax * rmax;
  int structureFrag = 0;
  G4VPhysicalVolume *theVolume;
  G4ThreeVector *ptr;

  while (1)
  {
    random_r = pow(2 * G4UniformRand() * R, 0.5);
    random_phi = 2.0 * pi * G4UniformRand();
    random_z = zmax * (1 - 2 * G4UniformRand());
    particle_position = G4ThreeVector(random_r * cos(random_phi), random_r * sin(random_phi), random_z);

    theVolume = gNav->LocateGlobalPointAndSetup(particle_position, ptr, true);
    // G4cout << "##################here##################" << G4endl;
    // G4cout << particle_position << G4endl;
    G4int copyID = theVolume->GetCopyNo();
    //G4cout << "##################here##################" << G4endl;
    if (copyID == structureFrag)
    {
      break;
    }
  }
  //G4cout << "##################here##################" << G4endl;
  G4double costheta = 1.0 - 2.0 * G4UniformRand();
  G4double phi = 2.0 * pi * G4UniformRand();
  G4double sintheta = sqrt((1.0 - costheta) * (1.0 + costheta));

  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  particleGun->SetParticleDefinition(particleTable->FindParticle("e-"));
  particleGun->SetParticleEnergy(1.46 * MeV);
  //particleGun->SetParticlePosition(G4ThreeVector(0. * mm, 0. * mm, 67.5 * mm));
  particleGun->SetParticlePosition(particle_position);
  particleGun->SetParticleMomentumDirection(G4ParticleMomentum(cos(phi) * sintheta, sin(phi) * sintheta, costheta));
  particleGun->GeneratePrimaryVertex(anEvent);
#endif
#ifdef OpticalMode

  G4int EventNum;
  char Buffer[256];
  G4double energy, X, Y, Z, Xdir, Ydir, Zdir;
  while (_ifs->getline(Buffer, sizeof(Buffer)))
  {
    std::stringstream strin(Buffer);
    strin >> energy >> X >> Y >> Z >> Xdir >> Ydir >> Zdir;
    particleGun->SetParticleEnergy(energy * GeV);
    particleGun->SetParticlePosition(G4ThreeVector(X, Y, Z));
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    //particleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
    particleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
    //particleGun->SetParticlePosition(G4ThreeVector(0 * m, 0 * m, -70 * m));
    particleGun->SetParticleMomentumDirection(G4ParticleMomentum(Xdir, Ydir, Zdir));
    // G4cout << " here " << G4ParticleMomentum(-sin(theta) * cos(Random_phi), -sin(theta) * sin(Random_phi), -cos(theta)).x() << G4endl;
    particleGun->GeneratePrimaryVertex(anEvent);
    break;
  }
#endif
#ifndef UniformDistribution
#ifndef OpticalMode
  G4double World_r = 80 * m, World_z = 160 * m;
  G4double DetectorHeight = 20.032 * m;
  G4double Detector_x = 0 * m, Detector_y = 0 * m, Detector_z = -World_z / 2 + DetectorHeight / 2;
  G4int EventNum;
  G4double Energy, theta; //MeV, rad
  G4double Random_phi;
  G4double Length = 40000 * mm; //enough to cover the detector
  G4double R = 0.5 * Length * Length;
  G4double Direction_x, Direction_y, Direction_z;
  G4double Shooting_x, Shooting_y, Shooting_z;
  G4double random_r, random_phi;
  G4double PI = 3.1415926535;
  char Buffer[256];
  while (_ifs->getline(Buffer, sizeof(Buffer)))
  {
    std::stringstream strin(Buffer);
    strin >> EventNum >> Energy >> theta;
    //==================================================================
    //                        test
    //theta = PI / 4;
    //Random_phi = 0;
    //==================================================================
    Random_phi = 2.0 * PI * G4UniformRand();
    random_r = pow(2 * G4UniformRand() * R, 0.5);
    random_phi = 2.0 * pi * G4UniformRand();
    G4cout << random_r << G4endl;
    double x_dd, y_dd, z_dd;
    x_dd = random_r * cos(random_phi);
    y_dd = random_r * sin(random_phi);
    //z_dd = Detector_z + 4000 * mm;
    z_dd = 40000 * mm;

    double x_d, y_d, z_d;
    /*
    x_d = x_dd * cos(Random_phi) - y_dd * sin(Random_phi);
    y_d = x_dd * sin(Random_phi) + y_dd * cos(Random_phi);
    z_d = z_dd;
    Shooting_x = x_d * cos(theta) + z_d * sin(theta);
    Shooting_y = y_d;
    Shooting_z = -x_d * sin(theta) + z_d * cos(theta) + Detector_z;
    */

    x_d = x_dd * cos(theta) + z_dd * sin(theta);
    y_d = y_dd;
    z_d = -x_dd * sin(theta) + z_dd * cos(theta);
    Shooting_x = x_d * cos(Random_phi) - y_d * sin(Random_phi);
    // G4cout << Shooting_x << G4endl;
    Shooting_y = x_d * sin(Random_phi) + y_d * cos(Random_phi);
    Shooting_z = z_d + Detector_z;

    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    //particleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
    particleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));
    if (Energy > 4000)
    {
      Energy = 4000;
    }
    particleGun->SetParticleEnergy(Energy * GeV);
    particleGun->SetParticlePosition(G4ThreeVector(Shooting_x, Shooting_y, Shooting_z));
    //particleGun->SetParticlePosition(G4ThreeVector(Detector_x, Detector_y, Detector_z));

    particleGun->SetParticleMomentumDirection(G4ParticleMomentum(-sin(theta) * cos(Random_phi), -sin(theta) * sin(Random_phi), -cos(theta)));
    // G4cout << " here " << G4ParticleMomentum(-sin(theta) * cos(Random_phi), -sin(theta) * sin(Random_phi), -cos(theta)).x() << G4endl;
    particleGun->GeneratePrimaryVertex(anEvent);
    break;
  }
#endif
#endif
}
