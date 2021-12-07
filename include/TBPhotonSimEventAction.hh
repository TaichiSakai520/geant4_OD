#ifndef __TBPhotonSimEventAction_hh__
#define __TBPhotonSimEventAction_hh__

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "OBDCalorHit.hh"
#include <fstream>
#include "G4VUserPrimaryGeneratorAction.hh"
class TBPhotonSimEventAction : public G4UserEventAction
{
public:
  TBPhotonSimEventAction();
  virtual ~TBPhotonSimEventAction();

protected:
  G4String _filename;

public:
  void SetFileName(G4String fname) { _filename = fname; }
  virtual void BeginOfEventAction(const G4Event *);
  virtual void EndOfEventAction(const G4Event *);
  G4int counter;
  static std::ofstream *_ofs;
  typedef struct
  {
    G4int PMTNum;
    G4double PMT_x, PMT_y, PMT_z;
  } PMT_t;
  PMT_t PMT[400];

private:
  std::ifstream *ifs;
  void ReadPMTPosition();
  G4double FindPmtX(G4int);
  G4double FindPmtY(G4int);
  G4double FindPmtZ(G4int);
};

#endif
