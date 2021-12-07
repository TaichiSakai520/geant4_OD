#include "OBDEnergy.hh"
#include "DetectorConstruction.hh"
//#include "g4std/fstream"
#include <fstream>

G4double OBDEnergy::LS_totEdep = 0;
G4ThreeVector OBDEnergy::LS_scintCentroidSum = G4ThreeVector();

kuFUNC *OBDEnergy::_Electron = 0;
kuFUNC *OBDEnergy::_Proton = 0;
kuFUNC *OBDEnergy::_Gamma = 0;
G4double OBDEnergy::_Electron_Emin = 0;
G4double OBDEnergy::_Electron_Emax = 0;
G4double OBDEnergy::_Proton_Emin = 0;
G4double OBDEnergy::_Proton_Emax = 0;
G4double OBDEnergy::_Gamma_Emin = 0;
G4double OBDEnergy::_Gamma_Emax = 0;
//================================================================================
G4double time_window = 100; // nsec
//================================================================================
bool OBDEnergy::_enableVisibleEnergy = true; // if "false", non-Visible Energy

void OBDEnergy::PostStepDoIt(const G4Step &pStep)
{

  // get Volume name
  //

  G4Track *track = pStep.GetTrack();
  const G4VPhysicalVolume *Vol = track->GetVolume();
  G4String VolName = Vol->GetName();
  G4double globalTime = track->GetGlobalTime();
  // get Position and Energy data
  // devide events into each PhysicalVolume
  //
  // --------------- LS
  //if gamma enter into LS
  //if (track->GetVolume() ==  GammaDetectorConstruction::GetAbsorber()){
  if (globalTime < time_window)
  {
    if (VolName == "PhysVol_LS")
    {
      G4StepPoint *pPreStepPoint = pStep.GetPreStepPoint();

      G4ThreeVector x0 = pPreStepPoint->GetPosition();
      G4ThreeVector p0 = pPreStepPoint->GetMomentumDirection();

      G4String ParticleName = track->GetDefinition()->GetParticleName();
      G4double KineticEnergy = track->GetKineticEnergy();
      G4double TotalEnergyDeposit = pStep.GetTotalEnergyDeposit();

      // check
      //    G4cout << "LS: " << ParticleName << " " << x0 << " " << TotalEnergyDeposit << " " << LS_totEdep << G4endl;

      G4double VisibleEnergy_Step = 0;

      if (_enableVisibleEnergy == false)
      {
        VisibleEnergy_Step += TotalEnergyDeposit;
      }
      else
      {
        /*
        if (ParticleName == "e-" || ParticleName == "e+")
        {
          VisibleEnergy_Step += ElectronToVisible_Step(KineticEnergy, TotalEnergyDeposit);
        }
        */
        if (ParticleName == "proton")
        {
          VisibleEnergy_Step += ProtonToVisible_Step(KineticEnergy, TotalEnergyDeposit);
        }
        /*
        else if (ParticleName == "gamma")
        {
          VisibleEnergy_Step += GammaToVisible_Step(KineticEnergy, TotalEnergyDeposit);
        }
        */
      }

      //printf("%10.5f  %10.5f  %10.5f\n", KineticEnergy, TotalEnergyDeposit, VisibleEnergy_Step);

      LS_totEdep += VisibleEnergy_Step;
      // G4cout << "sakai " << LS_totEdep << G4endl;
      LS_scintCentroidSum += VisibleEnergy_Step *
                             (x0 + p0 * (0.5 * pStep.GetStepLength()));

      //     LS_totEdep += TotalEnergyDeposit;
      //     LS_scintCentroidSum += TotalEnergyDeposit *
      //       ( x0 + p0*(0.5*pStep.GetStepLength()) );
    }
  }
}
G4double OBDEnergy::ElectronToVisible_Step(G4double KineticEnergy, G4double TotalEnergyDeposit)
{
  if (_Electron == 0)
  {

    double x[10000], y[10000];
    int N;

    std::ifstream fElectron("../Electron.dat");

    N = 0;
    while (fElectron >> x[N] >> y[N])
    {
      //G4cout << N << " " << x[N] << " " << y[N] << G4endl;
      N++;
    }
    _Electron = new kuFUNC(N, x, y);

    _Electron_Emin = x[0];
    _Electron_Emax = x[N - 1];
  }

  G4double InitialEnergy = KineticEnergy + TotalEnergyDeposit;
  G4double FinalEnergy = KineticEnergy;

  G4double initial_energy = InitialEnergy;
  G4double final_energy = FinalEnergy;

  if (initial_energy <= _Electron_Emin)
    initial_energy = _Electron_Emin;
  if (initial_energy >= _Electron_Emax)
    initial_energy = _Electron_Emax;

  if (final_energy <= _Electron_Emin)
    final_energy = _Electron_Emin;
  if (final_energy >= _Electron_Emax)
    final_energy = _Electron_Emax;

  G4double initial_ratio = _Electron->linint(initial_energy);
  G4double final_ratio = _Electron->linint(final_energy);

  G4double VisibleEnergy_Step = (InitialEnergy * initial_ratio) - (FinalEnergy * final_ratio);

  return VisibleEnergy_Step;
}

G4double OBDEnergy::ProtonToVisible_Step(G4double KineticEnergy, G4double TotalEnergyDeposit)
{

  double x[10000], y[10000];
  int N;

  N = 0;
  for (double E = 0.1; E < 10.0 - 0.001; E = E + 0.1)
  {
    x[N] = E;
    N++;
  }

  for (double E = 10.0; E < 100.0 - 0.001; E = E + 1.0)
  {
    x[N] = E;
    N++;
  }

  for (double E = 100.0; E < 1000.0 - 0.001; E = E + 10.0)
  {
    x[N] = E;
    N++;
  }

  for (int i = 0; i < N; i++)
  {

    // from Ichimura 2007.4.17
    // proton quench measurement by Yoshida

    const double p1 = -0.2072;
    const double p2 = -0.03349;
    const double p3 = 0.5806;

    y[i] = ((1.0 - exp(p1 * x[i] + p2)) * x[i] * p3) / x[i];

    // x_Evis[i] = x[i] * y[i];
  }

  _Proton = new kuFUNC(N, x, y);

  _Proton_Emin = x[0];
  _Proton_Emax = x[N - 1];

  G4double InitialEnergy = KineticEnergy + TotalEnergyDeposit;
  G4double FinalEnergy = KineticEnergy;

  G4double initial_energy = InitialEnergy;
  G4double final_energy = FinalEnergy;

  if (initial_energy <= _Proton_Emin)
    initial_energy = _Proton_Emin;
  if (initial_energy >= _Proton_Emax)
    initial_energy = _Proton_Emax;

  if (final_energy <= _Proton_Emin)
    final_energy = _Proton_Emin;
  if (final_energy >= _Proton_Emax)
    final_energy = _Proton_Emax;

  G4double initial_ratio = _Proton->linint(initial_energy);
  G4double final_ratio = _Proton->linint(final_energy);

  G4double VisibleEnergy_Step = (InitialEnergy * initial_ratio) - (FinalEnergy * final_ratio);

  return VisibleEnergy_Step;
}
G4double OBDEnergy::GammaToVisible_Step(G4double KineticEnergy, G4double TotalEnergyDeposit)
{

  double x[10000], y[10000];
  int N;

  std::ifstream fGamma("../Gamma.table");
  double dummy;
  N = 0;
  while (fGamma >> x[N] >> y[N] >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy)
  {
    //G4cout << N << " " << x[N] << " " << y[N] << G4endl;
    N++;
  }
  _Gamma = new kuFUNC(N, x, y);

  _Gamma_Emin = x[0];
  _Gamma_Emax = x[N - 1];

  G4double InitialEnergy = KineticEnergy + TotalEnergyDeposit;
  G4double FinalEnergy = KineticEnergy;

  G4double initial_energy = InitialEnergy;
  G4double final_energy = FinalEnergy;

  if (initial_energy <= _Gamma_Emin)
    initial_energy = _Gamma_Emin;
  if (initial_energy >= _Gamma_Emax)
    initial_energy = _Gamma_Emax;

  if (final_energy <= _Gamma_Emin)
    final_energy = _Gamma_Emin;
  if (final_energy >= _Gamma_Emax)
    final_energy = _Gamma_Emax;

  G4double initial_ratio = _Gamma->linint(initial_energy);
  G4double final_ratio = _Gamma->linint(final_energy);

  G4double VisibleEnergy_Step = (InitialEnergy * initial_ratio) - (FinalEnergy * final_ratio);

  return VisibleEnergy_Step;
}
