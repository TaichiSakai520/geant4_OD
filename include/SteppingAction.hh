/////////////////////////////////////////////////////////////////////////
//                          SteppingAction.hh
/////////////////////////////////////////////////////////////////////////

#ifndef SteppingAction_h
#define SteppingAction_h 1
#include <fstream> // for wrighting "all" data to file
#include "G4UserSteppingAction.hh"
#include "G4Navigator.hh"

//////////////////////////////////////////////////////////////////////////
class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction();
  virtual  ~SteppingAction();

  virtual  void UserSteppingAction(const G4Step*);

  static std::ofstream* _stepofs; // for wrighting "all" data to file
protected:
  G4Navigator* _gNavigator; 


};

//////////////////////////////////////////////////////////////////////////

#endif
