// ====================================================================
//                               ex02.cc
// ====================================================================
#include "G4RunManager.hh"
#include "G4UIterminal.hh"

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#include "DetectorConstruction.hh"              //1
#include "TBPhotonSimPrimaryGeneratorAction.hh" //2
#include "KLPhysicsList.hh"                     //3
#include "TBPhotonSimSteppingAction.hh"
#include "TBPhotonSimEventAction.hh"
#include "OBDRunAction.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

static const char _defaultStepFileName[] = "step.dat";
static const char _defaultPrimaryFileName[] = "EdepMuonNum.dat";
static const char _defaultinputFileName[] = "target_dr/MuonData_100000.dat_1";
static const char _defaultrootFileName[] = "photonSimdata/OBD";
#include "QGSP_BIC_HP.hh"
// ====================================================================
//                                main
// ====================================================================
int main(int argc, char **argv)
{
  //construct the default run manager
  G4RunManager *runManager = new G4RunManager;
  //G4cout << G4endl;

  // set mandatory user initialization classes...

  // detector setup //1
  runManager->SetUserInitialization(new DetectorConstruction);

  // particles and physics processes //3
  //runManager->SetUserInitialization(new KLPhysicsList);
  runManager->SetUserInitialization(new QGSP_BIC_HP);

  // primary generator //2
  TBPhotonSimPrimaryGeneratorAction *prAction = new TBPhotonSimPrimaryGeneratorAction;
  if (argc > 3)
  {
    prAction->SetFileName(argv[3]);
  }
  else
  {
    prAction->SetFileName(_defaultinputFileName);
  }
  runManager->SetUserAction(prAction);

  OBDRunAction *runAction = new OBDRunAction;
  if (argc > 4)
  {
    runAction->SetFileName(argv[4]);
  }
  else
  {
    runAction->SetFileName(_defaultrootFileName);
  }
  runManager->SetUserAction(runAction);
  // stepping action
  runManager->SetUserAction(new TBPhotonSimSteppingAction);

  TBPhotonSimEventAction *evAction = new TBPhotonSimEventAction;
  if (argc > 2)
  {
    evAction->SetFileName(argv[2]);
  }
  else
  {
    evAction->SetFileName(_defaultPrimaryFileName);
  }
  runManager->SetUserAction(evAction);

#ifdef G4VIS_USE
  // initialize visualization package
  G4VisManager *visManager = new G4VisExecutive;
  visManager->Initialize();
  G4cout << G4endl;
#endif

  // initialize G4 kernel (user session..)
  runManager->Initialize();

  if (argc == 1)
  { // interactive session, if no arguments given
#ifdef G4UI_USE_TCSH
    // tcsh-like
    G4UItcsh *tcsh = new G4UItcsh("ex02(%s)[%/]:");
    G4UIterminal *session = new G4UIterminal(tcsh);
#else
    // csh-like
    G4UIterminal *session = new G4UIterminal();
    session->SetPrompt("ex02(%s)[%/]:");
#endif
    session->SessionStart();
    delete session;
  }
  else
  { // batch mode
    G4UImanager *UImanager = G4UImanager::GetUIpointer();
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  // terminating...
#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;
  G4cout << G4endl;
  return 0;
}
