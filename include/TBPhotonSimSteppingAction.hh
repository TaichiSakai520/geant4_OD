#ifndef __TBPhotonSimSteppingAction_hh__
#define __TBPhotonSimSteppingAction_hh__

#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh" //test
#include <fstream>

class TBPhotonSimCaptureData
{
public:
  G4ThreeVector pos;
  G4ThreeVector mon;
  G4int pid;
  G4int ID = 0;
  G4double kin;
  G4double time;
  G4String name;
};

class TBPhotonSimEdepData
{
public:
  G4ThreeVector pos;
  G4double length;
  G4ThreeVector mon;
  G4int particleid;
  G4double kin;
  G4double time;
  G4double edep;
  G4int ID;
  G4double iniKin;
  G4String ParticleName;
  bool photonCount = false;
  bool absorbFlug = false;
  G4double InitEnergy;
  G4double Initx;
  G4double Inity;
  G4double Initz;
  // G4ThreeVector Direction;
  G4double xDirection;
  G4double yDirection;
  G4double zDirection;
  bool FirstRecord = false;

  G4double muInx;
  G4double muIny;
  G4double muInz;
  G4double muOutx;
  G4double muOuty;
  G4double muOutz;
};

class TBPhotonSimSteppingAction : public G4UserSteppingAction
{
public:
  TBPhotonSimSteppingAction();
  virtual ~TBPhotonSimSteppingAction();

  void UserSteppingAction(const G4Step *);

public:
  static std::vector<TBPhotonSimCaptureData> _capturedata;
  static std::vector<TBPhotonSimEdepData> _edepdata;
  static G4int _processID;

private:
  G4Navigator *_gNavigator;
};

#endif
