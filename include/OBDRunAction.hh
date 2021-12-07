
//===========================================================
//                        2021/02/08
//                       Taichi Sakai
//                   copy from example B4d
//===========================================================

// $Id: OBDRunAction.hh 74265 2013-10-02 14:41:20Z gcosmo $
//
/// \file OBDRunAction.hh
/// \brief Definition of the OBDRunAction class

#ifndef OBDRunAction_h
#define OBDRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class OBDRunAction : public G4UserRunAction
{
public:
  OBDRunAction();
  virtual ~OBDRunAction();

  virtual void BeginOfRunAction(const G4Run *);
  virtual void EndOfRunAction(const G4Run *);
  G4String _filename2;
  void SetFileName(G4String fname) { _filename2 = fname; }

  //std::ifstream *_ifs;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
