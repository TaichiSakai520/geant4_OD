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
// ====================================================================
//    K01PrimaryGeneratorAction.hh
//
//    A mandatory user action class for primary vertex/particle generation.
//
//    You must implement the virtual method GeneratePrimaries(),
//    which is invoked from G4RunManager during the event loop.
// ====================================================================
#ifndef TBPhotonSim_PRIMARY_GENERATOR_ACTION_H
#define TBPhotonSim_PRIMARY_GENERATOR_ACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Navigator.hh"

class G4ParticleGun;

class G4GeneralParticleSource;
class G4Event;

class TBPhotonSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
private:
  G4ParticleGun *particleGun; // particle gun provided by Geant4
  //G4GeneralParticleSource* particleGun;
public:
  TBPhotonSimPrimaryGeneratorAction();
  ~TBPhotonSimPrimaryGeneratorAction();
  void SetFileName(G4String fname) { _filename = fname; }
  static std::ifstream *_ifs;
  virtual void GeneratePrimaries(G4Event *anEvent);

private:
  // std::ifstream *fin;
  G4Navigator *gNav;

protected:
  G4String _filename;
};

#endif
