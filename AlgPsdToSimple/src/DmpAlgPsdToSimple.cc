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
#include <cstdlib>
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
  fRaw(0),
  fMergeAncFlag(false),
  fAncDirName("./"),
  fAncFileName(""),
  fAncEntry(0),
  fAncOffset(0),
  fDampeOffset(0),
  fDampeOffsetCounter(0)
{
  OptMap.insert(std::make_pair("dir",0));
  OptMap.insert(std::make_pair("prefix",1));
  OptMap.insert(std::make_pair("useAnc",2));
  OptMap.insert(std::make_pair("dirAnc",3));
  OptMap.insert(std::make_pair("nameAnc",4));
  OptMap.insert(std::make_pair("ancoffset",6));
  OptMap.insert(std::make_pair("dampeoffset",5));
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
    case 2:
    {
      if(value== "true" || value=="True"|| value=="TRUE")
	fMergeAncFlag=true;
      else if(value=="false"||value=="False"||value=="FALSE")
	fMergeAncFlag==false;
      else{
	DmpLogError<<"[DmpAlgPsdToSimple::Set] Unknown value = "<< value << " for parameter = "<< type << " ==> throwing exception!"<< DmpLogEndl;
      }
      break;
    }
    case 3:
    {
      fAncDirName=value;
      break;
    }
    case 4:
    {
      fAncFileName=value;
      break;
    }
    case 5:
    {
      fDampeOffset=std::atoi(value.c_str());
      break;
    }
    case 6:
    {
      fAncOffset=std::atoi(value.c_str());
      fAncEntry=fAncOffset;
      break;
    }
    default:
      break;
  }
}
//-------------------------------------------------------------------
bool DmpAlgPsdToSimple::Initialize(){
  //
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
  
  //
  fFile=new TFile((fDirName+fPrefix+"-Simple.root").c_str(),"recreate");
  fOutTree=new TTree("Psd","Psd");
  fOutTree->Branch("Adc",fAdc,"Adc[2][41][2][2]/D");
  fOutTree->Branch("Multiplicity",fMultiplicity,"Multiplicity[2]/I");
  fOutTree->Branch("AdcNoPed",fAdcNoPed,"AdcNoPed[2][41][2][2]/D");
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
  //
  if(fMergeAncFlag){
    fAncFile=new TFile((fAncDirName+fAncFileName).c_str());
    fAncTree=(TTree*)fAncFile->Get("AncillaryEvent");
 
    fAncTree->SetBranchAddress("V785",fV785);
    fAncTree->SetBranchAddress("V792",fV792);
    
    fOutTree->Branch("Si",fSi,"Si[2][2]/I");
    fOutTree->Branch("Sc",fSc,"Sc[2][3]/I");
  }
  //
  
  
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdToSimple::ProcessThisEvent(){
  fDampeOffsetCounter++;
  if(fDampeOffsetCounter>fDampeOffset){
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
    FillMutiplicity();
    //
    if(fMergeAncFlag){
      fAncTree->GetEntry(fAncEntry);
      fAncEntry++;
      //
      fSi[0][0]=fV785[0][0];//Si1 low gain
      fSi[0][1]=fV785[0][16];//Si1 high gain
      fSi[1][0]=fV785[0][4];//Si2 low gain
      fSi[1][1]=fV785[0][20];//Si2 high gain
      //
      fSc[0][0]=fV792[0][1];//Sc1 original
      fSc[0][1]=fV792[0][3];//Sc1 6db
      fSc[0][2]=fV792[0][4];//Sc1 12db
      fSc[1][0]=fV792[0][5];//Sc2 original
      fSc[1][1]=fV792[0][8];//Sc2 6db
      fSc[1][2]=fV792[0][11];//Sc2 12db
      //
      if(fAncEntry==fAncTree->GetEntries()){
	gCore->TerminateRun();
      }
    }
    //
    fOutTree->Fill();
  }
  
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdToSimple::Finalize(){
  fFile->cd();
  
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

bool DmpAlgPsdToSimple::GetPedSeed()
{
  std::ifstream PPpar;
  PPpar.open((fCalibParaPath + "PsdPar/PsdPed").c_str());
  if (!PPpar.good()) {
    std::cout << "Can not open Psd Par file!" << std::endl;
    return false;
  }
  for (int dy = 0; dy < 2; ++dy) {
    for (int la = 0; la < 2; ++la) {
      for (int ba = 0; ba < 41; ++ba) {
	for (int si = 0; si < 2; ++si) {
	  PPpar >> fPedMeanSeed[la][ba][1-si][dy];
	  PPpar >> fPedSigmaSeed[la][ba][1-si][dy];
	}
      }
    }
  }
  PPpar.close();
  return true;
}

void DmpAlgPsdToSimple::FillMutiplicity()
{
  
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	for(int dynode=0;dynode<2;dynode++){
	  fAdcNoPed[layer][bar][side][dynode]=fAdc[layer][bar][side][dynode]-fPedMeanSeed[layer][bar][side][dynode];
	}
      }
    }
  }
  //
  fMultiplicity[0]=0;fMultiplicity[1]=0;
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      if(fAdcNoPed[layer][bar][0][1]>=150 && fAdcNoPed[layer][bar][1][1]>=150){
	fMultiplicity[layer]++;
      }
    }
  }
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
  
  delete fAncFile;
}