#include "DmpAlgPsdMipCheck.h"
#include "DmpIOSvc.h"
#include "DmpCore.h"
#include "RecoConfigParser.h"
#include "DmpPsdBase.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include <cstdio>
#include <fstream>

const char DmpAlgPsdMipCheck::LAYERNAME[2][2]={"X","Y"};
const char DmpAlgPsdMipCheck::SIDENAME[2][4]={"Neg","Pos"};
const char DmpAlgPsdMipCheck::DYNODENAME[2][4]={"Dy5","Dy8"};

//-------------------------------------------------------------------
DmpAlgPsdMipCheck::DmpAlgPsdMipCheck()
 :DmpVAlg("PsdMipCheck"),
  fDirName("./"),
  fPrefix(""),
  fCalibParaPath(""),
  fRaw(0)
{
  OptMap.insert(std::make_pair("dir",0));
  OptMap.insert(std::make_pair("prefix",1));
}

//-------------------------------------------------------------------
DmpAlgPsdMipCheck::~DmpAlgPsdMipCheck(){
}

//
void DmpAlgPsdMipCheck::Set(const std::string &type,const std::string &value)
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
bool DmpAlgPsdMipCheck::Initialize(){
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
  for(int layer=0;layer<2;layer++){
    for(int side=0;side<2;side++){
      fFile[2*layer+side]=fopen((fDirName+fPrefix+"-"+LAYERNAME[layer]+"-"+SIDENAME[side]+".csv").c_str(),"w");
    }    
  }
  //
  gIOSvc->GetContainer("DmpEvtPsdRaw",fRaw);
  //
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	fDy58[layer][bar][side]=new TH2F(Form("h%s_%d_%s",LAYERNAME[layer],bar+1,SIDENAME[side]),Form("%s_%d_%s",LAYERNAME[layer],bar+1,SIDENAME[side]),200,-0.5,1999.5,1700,-0.5,16999.5);
	//for(int dynode=0;dynode<2;dynode++){
	{  
	  fHist[layer][bar][side][0]=new TH1F(Form("h%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[0]),Form("%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[0]),1500,-0.5,3999.5);
	  fHist[layer][bar][side][1]=new TH1F(Form("h%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[1]),Form("%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[1]),3400,-0.5,16999.5);	  
	}
      }
      //
      //for(int dynode0=0;dynode<2;dynode++){
      {
	fSideSide[layer][bar][0]=new TH2F(Form("h%s_%d_%s",LAYERNAME[layer],bar+1,DYNODENAME[0]),Form("%s_%d_%s",LAYERNAME[layer],bar+1,DYNODENAME[0]),200,-0.5,1999.5,200,-0.5,1999.5);
	fSideSide[layer][bar][1]=new TH2F(Form("h%s_%d_%s",LAYERNAME[layer],bar+1,DYNODENAME[1]),Form("%s_%d_%s",LAYERNAME[layer],bar+1,DYNODENAME[1]),1700,-0.5,16999.5,1700,-0.5,16999.5);
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdMipCheck::ProcessThisEvent(){
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
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	fDy58[layer][bar][side]->Fill(fAdc[layer][bar][side][0],fAdc[layer][bar][side][1]);
      }
      for(int dynode=0;dynode<2;dynode++){
	fSideSide[layer][bar][dynode]->Fill(fAdc[layer][bar][0][dynode],fAdc[layer][bar][1][dynode]);
      }
    }
  }
  
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdMipCheck::Finalize(){
  TF1* ftemp[4];
  
  TCanvas c1("c1","c1",1000,1000);
  c1.Divide(2,2);
  for(int i=0;i<4;i++){
    c1.cd(i+1);
    gPad->SetLogy();
  }
  
  c1.Print((fDirName+"Ped"+"-"+fPrefix+".pdf[").c_str());
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      c1.cd(1);
      ftemp[0]=FitPed(fHist[layer][bar][0][0],fPedMeanSeed[layer][bar][0][0],fPedSigmaSeed[layer][bar][0][0],
		       fPedMeanFit[layer][bar][0][0],fPedSigmaFit[layer][bar][0][0]);
      fHist[layer][bar][0][0]->Draw();
      ftemp[0]->Draw("lsame");
      
      c1.cd(2);
      ftemp[1]=FitPed(fHist[layer][bar][0][1],fPedMeanSeed[layer][bar][0][1],fPedSigmaSeed[layer][bar][0][1],
		       fPedMeanFit[layer][bar][0][1],fPedSigmaFit[layer][bar][0][1]);
      fHist[layer][bar][0][1]->Draw();
      ftemp[1]->Draw("lsame");
      
      c1.cd(3);
      ftemp[2]=FitPed(fHist[layer][bar][1][0],fPedMeanSeed[layer][bar][1][0],fPedSigmaSeed[layer][bar][1][0],
		       fPedMeanFit[layer][bar][1][0],fPedSigmaFit[layer][bar][1][0]);
      fHist[layer][bar][1][0]->Draw();
      ftemp[2]->Draw("lsame");
      
      c1.cd(4);
      ftemp[3]=FitPed(fHist[layer][bar][1][1],fPedMeanSeed[layer][bar][1][1],fPedSigmaSeed[layer][bar][1][1],
		       fPedMeanFit[layer][bar][1][1],fPedSigmaFit[layer][bar][1][1]);
      fHist[layer][bar][1][1]->Draw();
      ftemp[3]->Draw("lsame");
      
      c1.Print((fDirName+"Ped"+"-"+fPrefix+".pdf").c_str());
      
    }
  }
  c1.Print((fDirName+"Ped"+"-"+fPrefix+".pdf]").c_str());
  //
  TCanvas c2("c2","c2",1000,500);
  c2.Divide(2,1);  
  c2.Print((fDirName+"Dy58"+"-"+fPrefix+".pdf[").c_str());
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      c2.cd(1);
      fDy58[layer][bar][0]->Draw();
      
      c2.cd(2);
      fDy58[layer][bar][1]->Draw();
      
      c2.Print((fDirName+"Dy58"+"-"+fPrefix+".pdf").c_str());
      
    }
  }
  c2.Print((fDirName+"Dy58"+"-"+fPrefix+".pdf]").c_str());
  // 
  c2.Print((fDirName+"SideSide"+"-"+fPrefix+".pdf[").c_str());
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      c2.cd(1);
      fSideSide[layer][bar][0]->Draw();
      
      c2.cd(2);
      fSideSide[layer][bar][1]->Draw();
      
      c2.Print((fDirName+"SideSide"+"-"+fPrefix+".pdf").c_str());
      
    }
  }
  c2.Print((fDirName+"SideSide"+"-"+fPrefix+".pdf]").c_str());
  //
  for(int layer=0;layer<2;layer++){
    for(int side=0;side<2;side++){
      fprintf(fFile[2*layer+side],"dy5_mean,dy5_sigma,dy8_mean,dy8_sigma\n");
      for(int bar=0;bar<41;bar++){
	fprintf(fFile[2*layer+side],"%.3f,%.3f,%.3f,%.3f\n",
		fPedMeanFit[layer][bar][side][0],fPedSigmaFit[layer][bar][side][0],
		fPedMeanFit[layer][bar][side][1],fPedSigmaFit[layer][bar][side][1]
 	      );	
      }
    }    
  }
  //
  Clear();
  return true;
}

bool DmpAlgPsdMipCheck::GetPedSeed()
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


TF1* DmpAlgPsdMipCheck::FitPed(TH1F* poHist,double& ped_mean,double& ped_sigma,double& pedout_mean,double& pedout_sigma)
{
    //get pedestal from mips histogram
    TF1* fgaus=new TF1(Form("ped_%s",poHist->GetName()),"gaus",ped_mean-30.0,ped_mean+30.0);
    //fgaus->SetParameter(1,ped_mean);
    //fgaus->SetParameter(2,ped_sigma);
    poHist->Fit(Form("ped_%s",poHist->GetName()),"Rq0");
    pedout_mean=fgaus->GetParameter(1);
    pedout_sigma=fgaus->GetParameter(2);

    return fgaus;
}

void DmpAlgPsdMipCheck::Clear()
{
  //
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	for(int dynode=0;dynode<2;dynode++){
	  delete fHist[layer][bar][side][dynode];
	}
	delete fDy58[layer][bar][side];
	delete fSideSide[layer][bar][side];
      }
      //
    }
  }
  //
  for(int i=0;i<4;i++){
    fclose(fFile[i]);
  }
}