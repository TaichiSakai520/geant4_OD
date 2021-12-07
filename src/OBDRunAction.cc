//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: OBDRunAction.cc 87359 2014-12-01 16:04:27Z gcosmo $
//
/// \file OBDRunAction.cc
/// \brief Implementation of the OBDRunAction class

#include "OBDRunAction.hh"
#include "OBDAnalysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
OBDRunAction::OBDRunAction()
    : G4UserRunAction()
{
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in OBDAnalysis.hh
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories
  analysisManager->SetHistoDirectoryName("histograms");
  analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFirstHistoId(1);
  // _ifs = NULL;
  // Book histograms, ntuple
  //
  //_filename = "OBD";
  // Creating histograms

  //analysisManager->CreateH1("1", "photon Hit Num", 100, 0., 100000);
  // analysisManager->CreateH1("2", "photon Yield Num", 1000, 0., 1000000);

  // Creating ntuple
  //
  // analysisManager->CreateNtuple("OBD", "photon num");
  // analysisManager->CreateNtupleDColumn("HitNum");
  // analysisManager->CreateNtupleDColumn("YieldNum");

  // analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OBDRunAction::~OBDRunAction()
{
  delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OBDRunAction::BeginOfRunAction(const G4Run * /*run*/)
{

  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  //_filename2 = "photonSimdata/OBD";
  // Open an output file
  //
  //    G4String fileName = "OBD";
  analysisManager->OpenFile(_filename2);

  analysisManager->CreateH1("1", "EventNum", 100, 0., 100);
  analysisManager->CreateH1("2", "photon Hit Num", 100, 0., 100000);
  analysisManager->CreateH1("3", "photon Yield Num", 1000, 0., 1000000);
  analysisManager->CreateH1("4", "PMT Num", 300, 0., 300);
  analysisManager->CreateH1("5", "Hit Time", 30000, 0., 300);
  analysisManager->CreateH1("6", "PMT X", 200, 0., 20000);
  analysisManager->CreateH1("7", "PMT Y", 200, 0., 20000);
  analysisManager->CreateH1("8", "PMT Z", 200, 0., 20000);

  // Creating ntuple
  //
  analysisManager->CreateNtuple("OBD", "");
  analysisManager->CreateNtupleDColumn("EventNum");
  analysisManager->CreateNtupleDColumn("HitNum");
  analysisManager->CreateNtupleDColumn("YieldNum");
  analysisManager->CreateNtupleDColumn("PMTID");
  analysisManager->CreateNtupleDColumn("HitTime");
  analysisManager->CreateNtupleDColumn("PMTX");
  analysisManager->CreateNtupleDColumn("PMTY");
  analysisManager->CreateNtupleDColumn("PMTZ");

  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OBDRunAction::EndOfRunAction(const G4Run * /*run*/)
{
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  // save histograms & ntuple
  //

  // analysisManager->Write();
  //analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
