#include "DmpAlgPsdToSimple.h"
#include "DmpIOSvc.h"
#include "DmpCore.h"
#include "RecoConfigParser.h"
#include "DmpPsdBase.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TTree.h"
#include "TFile.h"
#include <cstdio>
#include <fstream>

const char DmpAlgPsdToSimple::LAYERNAME[2][2]={"X","Y"};
const char DmpAlgPsdToSimple::SIDENAME[2][4]={"Neg","Pos"};
const char DmpAlgPsdToSimple::DYNODENAME[2][4]={"Dy5","Dy8"};

//-------------------------------------------------------------------
DmpAlgPsdToSimple::DmpAlgPsdToSimple()
 :DmpVAlg("PsdToSimple"),
  fDirName("./"),
  fPrefix(""),
  fRaw(0)
{
  OptMap.insert(std::make_pair("dir",0));
  OptMap.insert(std::make_pair("prefix",1));
}

//-------------------------------------------------------------------
DmpAlgPsdToSimple::~DmpAlgPsdToSimple(){
}

//
void DmpAlgPsdToSimple::Set(const std::string &type,const std::string &value)
{
  switch(OptMap[type])
  {
    case 0:
    {
      fDirName=value;
      break;
    }
    case 1:
    {
      fPrefix=value;
      break;
    }
    default:
      break;
  }
}
//-------------------------------------------------------------------
bool DmpAlgPsdToSimple::Initialize(){
  //
  /*
  RecoConfigParser* fCfgParser   = (RecoConfigParser*)(gCore->ServiceManager()->Get("RecoConfigParser"));
  fCalibParaPath =   fCfgParser->Get("BGO", "Calibration", "");
  char* rootPath = NULL;
  rootPath = getenv ("DMPSWSYS");
  if(rootPath == NULL){
    DmpLogError<<"No DMPSWSYS environment variable! ==> throwing exception!"<<DmpLogEndl;
    throw;
  }
  fCalibParaPath = std::string(rootPath) + "/share/Calibration/" + fCalibParaPath + "/";
  if(!GetPedSeed()){
    DmpLogError<<"Can't read PSD standard pedestal value:"<<fCalibParaPath<<DmpLogEndl;
    throw;
  }
  */
  //
  fFile=new TFile((fDirName+fPrefix+"-Simple.root").c_str(),"recreate");
  fOutTree=new TTree("Psd","Psd");
  fOutTree->Branch("Adc",fAdc,"Adc[2][41][2][2]/D");
  //
  gIOSvc->GetContainer("DmpEvtPsdRaw",fRaw);
  //
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	//for(int dynode=0;dynode<2;dynode++){
	{  
	  fHist[layer][bar][side][0]=new TH1F(Form("h%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[0]),Form("%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[0]),17000,-0.5,16999.5);
	  fHist[layer][bar][side][1]=new TH1F(Form("h%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[1]),Form("%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[1]),17000,-0.5,16999.5);	 
	}
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdToSimple::ProcessThisEvent(){
  short nS = fRaw->fGlobalDynodeID.size();
// std::cout<<"nSignal: "<<nS<<std::endl;
  short gid = 0, l = 0, b = 0, s = 0, d = 0;
  double adc = 0.;
  for (int i = 0; i < nS; i++) {
    gid = fRaw->fGlobalDynodeID[i];
    adc = fRaw->fADC[i];
    l = DmpPsdBase::GetLayerID(gid);
    b = DmpPsdBase::GetStripID(gid);
    s = DmpPsdBase::GetSideID(gid);
    d = DmpPsdBase::GetDynodeID(gid);
    if (b >= 41) {
      continue;
    }
    switch(d)
    {
      case 5:
      {
	fHist[l][b][s][0]->Fill(adc);
	fAdc[l][b][s][0]=adc;
	break;
      }
      case 8:
      {
	fHist[l][b][s][1]->Fill(adc);
	fAdc[l][b][s][1]=adc;
	break;
      }
      default:
	break;
    }
  }
  //
  fOutTree->Fill();
  
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdToSimple::Finalize(){

  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	for(int dynode=0;dynode<2;dynode++){
	  fHist[layer][bar][side][dynode]->Write();
	}
      }      
    }
  }

  fOutTree->Write();
  //
  Clear();
  return true;
}


void DmpAlgPsdToSimple::Clear()
{
  //
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	for(int dynode=0;dynode<2;dynode++){
	  delete fHist[layer][bar][side][dynode];
	}
      }
      //
    }
  }
  //
  delete fOutTree;
  delete fFile;
  
}