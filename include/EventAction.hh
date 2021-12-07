#ifndef __EVENTATION_HH__
#define __EVENTATION_HH__

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include <fstream>
class EventAction: public G4UserEventAction{
public:
  EventAction();
  virtual ~EventAction();

public:
  void OpenFile(const char* filename);
  void CloseFile();
  virtual void EndOfEventAction(const G4Event* evt);

protected:
  std::ofstream* _ofs;
};

#endif
