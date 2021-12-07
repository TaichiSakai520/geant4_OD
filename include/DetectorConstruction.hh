// ====================================================================
//    DetectorConstruction.hh
//
// ====================================================================
#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

// ====================================================================
//
// class definition
//
// ====================================================================

class DetectorConstruction : public G4VUserDetectorConstruction
{

public:
  DetectorConstruction();
  ~DetectorConstruction();

  void DefineMaterials();
  virtual G4VPhysicalVolume *Construct();
  //virtual void ConstructSDandField();
private:
  std::ifstream *fin;
};

#endif
