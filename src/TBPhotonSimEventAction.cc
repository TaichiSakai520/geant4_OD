#include "TBPhotonSimEventAction.hh"
#include "TBPhotonSimSteppingAction.hh"

#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>
#include <sstream>

#ifdef G4VIS_USE
#include "G4VVisManager.hh"
#endif
//#include "G4Event.hh"
//#include "G4EventManager.hh"
#include "G4UImanager.hh"
#include "G4Trajectory.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHandle.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4EventManager.hh"
#include "OBDEnergy.hh"
#include "OBDAnalysis.hh"

#include "G4HCofThisEvent.hh"
#include "globals.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"

//#define OpticalMode

std::ofstream *TBPhotonSimEventAction::_ofs = NULL;

TBPhotonSimEventAction::TBPhotonSimEventAction()
{
	_filename = "event.dat";
	_ofs = NULL;
	counter = 0;
	ifs = new std::ifstream("PMTPosition.dat");
}

TBPhotonSimEventAction::~TBPhotonSimEventAction()
{
	if (_ofs)
		delete _ofs;
	std::cerr << "### Last " << counter << std::endl;
}

void TBPhotonSimEventAction::BeginOfEventAction(const G4Event *evt)
{
	if (evt->GetEventID() % 10000 == 0)
	{
		std::cerr << "###" << evt->GetEventID() << std::endl;
		// 	if(evt->GetEventID()!=0)_ofs->flush();
	}

	OBDEnergy::ResetTotEdep();

	//     if(evt->GetEventID()%100000==0){
	// 	int id=int(evt->GetEventID()/100000);
	// 	std::ostringstream os;
	// 	os << _filename.c_str() << "."<<std::setw(3) << std::setfill('0') << id << std::ends;
	// 	if(_ofs) {_ofs->close(); delete _ofs;}
	// 	_ofs = new std::ofstream(os.str().c_str());
	//         if(!_ofs->good()){
	//             std::cerr << "File(" << os.str().c_str() << ") I/O Error" << std::endl;
	//             delete _ofs;
	//             abort();
	//         }
	// 	std::cerr << "open (" << os.str().c_str() << ")..." << std::endl;
	//     }
}

void TBPhotonSimEventAction::ReadPMTPosition()
{

	char Buffer[256];
	G4int PMTNum;

	while (ifs->getline(Buffer, sizeof(Buffer)))
	{
		G4int PMTNumtmp;
		G4double PMT_xtmp, PMT_ytmp, PMT_ztmp;
		std::stringstream strin(Buffer);
		strin >> PMTNumtmp >> PMT_xtmp >> PMT_ytmp >> PMT_ztmp;
		PMT[PMTNumtmp].PMTNum = PMTNumtmp;
		PMT[PMTNumtmp].PMT_x = PMT_xtmp;
		PMT[PMTNumtmp].PMT_y = PMT_ytmp;
		PMT[PMTNumtmp].PMT_z = PMT_ztmp;
	}
}
G4double TBPhotonSimEventAction::FindPmtX(G4int i)
{
	return PMT[i].PMT_x;
}
G4double TBPhotonSimEventAction::FindPmtY(G4int i)
{
	return PMT[i].PMT_y;
}
G4double TBPhotonSimEventAction::FindPmtZ(G4int i)
{
	return PMT[i].PMT_z;
}

void TBPhotonSimEventAction::EndOfEventAction(const G4Event *evt)
{

	if (!_ofs)
	{
		_ofs = new std::ofstream(_filename.c_str());
		if (!_ofs->good())
		{
			std::cerr << "File(" << _filename.c_str() << ") I/O Error" << std::endl;
			delete _ofs;
			abort();
		}
	}
	double cap_x, cap_y, cap_z;
	double captureTime;
	// if(TBPhotonSimSteppingAction::_processID==1){
	bool recordflag = false;
	*_ofs << "# " << evt->GetEventID();
	//	G4cout << "here" << G4endl;
	if (TBPhotonSimSteppingAction::_capturedata.size() == 0)
	{
		//	*_ofs << " 0" << std::endl;
		*_ofs << std::endl;
	}
	else
	{
		//	*_ofs << " " << TBPhotonSimSteppingAction::_processID
		//		<< " " << TBPhotonSimSteppingAction::_capturedata[0].time / microsecond
		//			<< " [microsec]";
		for (size_t j = 0; j < TBPhotonSimSteppingAction::_capturedata.size(); j++)
		{
			//	*_ofs << " " << TBPhotonSimSteppingAction::_capturedata[j].name
			//			<< "(" << TBPhotonSimSteppingAction::_capturedata[j].kin / MeV
			//			<< " MeV)";

			if (TBPhotonSimSteppingAction::_capturedata[j].ID == 5000)
			{
				recordflag = true;
				cap_x = TBPhotonSimSteppingAction::_capturedata[j].pos.x();
				cap_y = TBPhotonSimSteppingAction::_capturedata[j].pos.y();
				cap_z = TBPhotonSimSteppingAction::_capturedata[j].pos.z();
				captureTime = TBPhotonSimSteppingAction::_capturedata[j].time;
			}
		}
		*_ofs << std::endl;
	}

	//  }
	// *_ofs <<"%"<<TBPhotonSimSteppingAction::_edepdata.size() << G4endl;
	if (TBPhotonSimSteppingAction::_edepdata.size() > 0)
	{
		// G4double w_x1=0,w_y1=0,w_z1=0,w_t1=0;
		//  G4double e_sum1=0;

		//  G4double w_x2=0,w_y2=0,w_z2=0,w_t2=0;
		//  G4double e_sum2=0;
		// if(TBPhotonSimSteppingAction::_processID==1){

		ReadPMTPosition();

		double energy = 0.;
		int HitNum = 0;
		int yieldNum = 0;
		int PMTID;
		double HitTime;
		double pmtx, pmty, pmtz;
		bool IsItFirstHit = true;
		double DetectorTime;
		double initx, inity, initz, initenergy, xdir, ydir, zdir;
		double muInx, muIny, muInz, muOutx, muOuty, muOutz, trackLength;
		double weighted_x, weighted_y, weighted_z;
		double p_x = 0, p_y = 0, p_z = 0;
		bool muonflag = false;
		G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
		for (int i = 0; i < TBPhotonSimSteppingAction::_edepdata.size(); i++)
		{

			TBPhotonSimEdepData edata = TBPhotonSimSteppingAction::_edepdata[i];
//	G4cout << "Here" << edata.absorbFlug << " " << edata.edep << " " << edata.ParticleName << " " << edata.ID << G4endl;
#ifdef OpticalMode
			if (edata.ID > 2000 && edata.ID < 3000 && edata.absorbFlug == true && edata.ParticleName == "opticalphoton")
			{
				int EventNum = evt->GetEventID();
				HitNum++;
				PMTID = edata.ID - 2000;
				analysisManager->FillH1(1, EventNum);
				analysisManager->FillH1(4, PMTID);
				analysisManager->FillNtupleDColumn(3, PMTID);
				pmtx = FindPmtX(PMTID);
				pmty = FindPmtY(PMTID);
				pmtz = FindPmtZ(PMTID);
				analysisManager->FillH1(6, pmtx);
				analysisManager->FillNtupleDColumn(5, pmtx);
				analysisManager->FillH1(7, pmty);
				analysisManager->FillNtupleDColumn(6, pmty);
				analysisManager->FillH1(8, pmtz);
				analysisManager->FillNtupleDColumn(7, pmtz);
				/*
				if (IsItFirstHit == true)
				{
					DetectorTime = edata.time;
					IsItFirstHit = false;
				}
				*/
				//	HitTime = edata.time - DetectorTime;
				HitTime = edata.time;
				analysisManager->FillNtupleDColumn(0, EventNum);
				analysisManager->FillH1(5, HitTime);
				analysisManager->FillNtupleDColumn(4, HitTime);
				analysisManager->AddNtupleRow();
			}

			if (edata.photonCount == true)
			{
				yieldNum++;
			}
#else
			if (edata.FirstRecord == true)
			{
				initx = edata.Initx;
				inity = edata.Inity;
				initz = edata.Initz;
				xdir = edata.xDirection;
				ydir = edata.yDirection;
				zdir = edata.zDirection;
				initenergy = edata.InitEnergy;
				//	G4cout << "here " << initx << " " << inity << " " << initz << G4endl;
				edata.FirstRecord = false;
			}

			//G4cout << "here " << initx << " " << inity << " " << initz << G4endl;
			//G4cout << edata.ID << G4endl;
			if (edata.ID == 5000 && edata.ParticleName == "mu-")
			{
				muonflag = true;
			}
			if (edata.ID == 5000 && edata.time < (captureTime - 10000) && edata.time > (captureTime - 1000000) /*&& || edata.ParticleName == "e-" || edata.ParticleName == "gamma")*/)
			{
				energy = energy + edata.edep;
				p_x += edata.edep * edata.pos.x();
				p_y += edata.edep * edata.pos.y();
				p_z += edata.edep * edata.pos.z();
			}
			weighted_x = p_x / energy;
			weighted_y = p_y / energy;
			weighted_z = p_z / energy;
#endif
		}
		trackLength = sqrt(pow((muInx - muOutx), 2) + pow((muIny - muOuty), 2) + pow((muInz - muOutz), 2));
		// get analysis manager

#ifdef OpticalMode
		*_ofs
				<< energy << " " << HitNum << " " << yieldNum << G4endl;

#else
		//	G4cout << "here " << energy << G4endl;
		if (recordflag == true && muonflag == false)
		{
			*_ofs << evt->GetEventID() << " " << energy << " " << weighted_x << " " << weighted_y << " " << weighted_z << " " << cap_x << " " << cap_y << " " << cap_z << " " << initx << " " << inity << " " << initz << " " << xdir << " " << ydir << " " << zdir << " " << initenergy << G4endl;
		}
		//	G4cout << "here " << initx << " " << inity << " " << initz << G4endl;
#endif
		//analysisManager->FillH1(2, HitNum);
		//analysisManager->FillH1(3, yieldNum);

		//	analysisManager->FillNtupleDColumn(1, HitNum);
		//analysisManager->FillNtupleDColumn(2, yieldNum);

		//G4cout<<std::scientific<<w_x<<" "<<w_y<<" "<<w_z<<" "<<ene<<G4endl;

		//}
	}
	TBPhotonSimSteppingAction::_capturedata.clear();
	TBPhotonSimSteppingAction::_edepdata.clear();
	TBPhotonSimSteppingAction::_processID = 0;
	//
	//
	//
#ifdef G4VIS_USE

	if (G4VVisManager::GetConcreteInstance())
	{
		G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");
		G4TrajectoryContainer *trajContainer = evt->GetTrajectoryContainer();
		G4int n_trajectories = 0;
		G4String drawTrksFlag = "all";
		if (trajContainer)
			n_trajectories = trajContainer->entries();
		for (G4int i = 0; i < n_trajectories; i++)
		{
			G4Trajectory *trj = (G4Trajectory *)(*trajContainer)[i];
			if (drawTrksFlag == "all")
				trj->DrawTrajectory();
			else if ((drawTrksFlag == "charged") && (trj->GetCharge() != 0.))
				trj->DrawTrajectory();
			else if ((drawTrksFlag == "noscint") && (trj->GetParticleName() != "opticalphoton"))
				trj->DrawTrajectory();
		}

		// G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
	}
#endif
}
