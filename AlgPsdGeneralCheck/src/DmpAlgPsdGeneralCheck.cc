#include "DmpAlgPsdGeneralCheck.h"
#include "DmpIOSvc.h"
#include "DmpCore.h"
#include "RecoConfigParser.h"
#include "DmpGeometryManager.h"
#include "DmpPsdBase.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TGaxis.h"
#include <cstdio>
#include <fstream>

const char DmpAlgPsdGeneralCheck::LAYERNAME[2][2]={"X","Y"};
const char DmpAlgPsdGeneralCheck::SIDENAME[2][4]={"Neg","Pos"};
const char DmpAlgPsdGeneralCheck::DYNODENAME[2][4]={"Dy5","Dy8"};

//-------------------------------------------------------------------
DmpAlgPsdGeneralCheck::DmpAlgPsdGeneralCheck()
 :DmpVAlg("PsdGeneralCheck"),
  fDirName("./"),
  fPrefix(""),
  fCalibParaPath(""),
  fRaw(0)
{
  OptMap.insert(std::make_pair("dir",0));
  OptMap.insert(std::make_pair("prefix",1));
}

//-------------------------------------------------------------------
DmpAlgPsdGeneralCheck::~DmpAlgPsdGeneralCheck(){
}

//
void DmpAlgPsdGeneralCheck::Set(const std::string &type,const std::string &value)
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
bool DmpAlgPsdGeneralCheck::Initialize(){
  
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
  if(!GetMipSeed()){
    DmpLogError<<"Can't read PSD standard mip value:"<<fCalibParaPath<<DmpLogEndl;
    throw;
  }
  
  GetStripPosition();
  
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
  //
  fHistPos=new TH2F("hPos","PSD Hitted Position",41,-410,410,41,-410,410);
  fHistPos->GetXaxis()->SetTitle("Y of DMAPE coordinate system");fHistPos->GetXaxis()->CenterTitle();
  fHistPos->GetYaxis()->SetTitle("X of DMAPE coordinate system");fHistPos->GetYaxis()->CenterTitle();
  
  fMultiplicity[0]=new TH1F("hXMulti","PSD X-Multiplicity",42,-0.5,41.5);
  fMultiplicity[1]=new TH1F("hYMulti","PSD Y-Multiplicity",42,-0.5,41.5);
  
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdGeneralCheck::ProcessThisEvent(){
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
  //
  FillMutiplicity();
  
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgPsdGeneralCheck::Finalize(){
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
  TCanvas c3("c3","c3",1000,1000);
  fHistPos->Draw("colz");
  
   // Redraw the new axis 
  fHistPos->GetXaxis()->SetLabelOffset(999);
  fHistPos->GetXaxis()->SetTickLength(0);
  gPad->Update();
  TGaxis *newaxisx = new TGaxis(gPad->GetUxmax(), 
				gPad->GetUymin(),
				gPad->GetUxmin(),
				gPad->GetUymin(),
				fHistPos->GetXaxis()->GetXmin(),
				fHistPos->GetXaxis()->GetXmax(),
				510,"-");
  newaxisx->SetLabelOffset(-0.03);
  newaxisx->Draw();
  
  // Redraw the new axis 
  fHistPos->GetYaxis()->SetLabelOffset(999);
  fHistPos->GetYaxis()->SetTickLength(0);
  
  gPad->Update();
  TGaxis *newaxisy = new TGaxis(gPad->GetUxmin(), 
				gPad->GetUymax(),
				gPad->GetUxmin()-0.001,
				gPad->GetUymin(),
				fHistPos->GetYaxis()->GetXmin(),
				fHistPos->GetYaxis()->GetXmax(),
				510,"+");
  newaxisy->SetLabelOffset(-0.03);
  newaxisy->Draw();
  c3.Print((fDirName+"HitPos"+"-"+fPrefix+".pdf").c_str());
  //
  TCanvas c4("c4","c4",1000,500);
  c4.Divide(2,1);
  for(int layer=0;layer<2;layer++){
    c4.cd(layer+1);
    fMultiplicity[layer]->Draw();
  }
  c4.Print((fDirName+"Multiplicity"+"-"+fPrefix+".eps").c_str());
  //
  Clear();
  return true;
}

bool DmpAlgPsdGeneralCheck::GetPedSeed()
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

bool DmpAlgPsdGeneralCheck::GetMipSeed(){
	ifstream PMpar;
	PMpar.open((fCalibParaPath + "PsdPar/PsdMips").c_str());
	if (!PMpar.good()) {
		std::cout << "Can not open Psd Mips file!" << std::endl;
		return false;
	}
	for (int l = 0; l < 2; ++l) {
		for (int b = 0; b < 41; ++b) {
			for (int s = 0; s < 2; ++s) {
				for (int p = 0; p < 3; ++p) {
					PMpar >> fMipSeed[l][b][1-s][p];

				}
			}
			fMipSqrt[l][b]=TMath::Sqrt(fMipSeed[l][b][0][0]*fMipSeed[l][b][1][0]);
		}
	}
	PMpar.close();
	return true;
}


void DmpAlgPsdGeneralCheck::GetStripPosition()
{
  DmpGeometryManager* geoMan=(DmpGeometryManager*)(gCore->ServiceManager()->Get("DmpGeometryManager"));
  
  fStripCentres.clear();
  
  fStripCentres=geoMan->getPSDStripCentres();
}


TF1* DmpAlgPsdGeneralCheck::FitPed(TH1F* poHist,double& ped_mean,double& ped_sigma,double& pedout_mean,double& pedout_sigma)
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

void DmpAlgPsdGeneralCheck::FillMutiplicity()
{
  //
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
  int multiplicity[2]={0,0};
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      if(fAdcNoPed[layer][bar][0][1]>=150 && fAdcNoPed[layer][bar][1][1]>=150){
	multiplicity[layer]++;
      }
    }
    if(multiplicity[layer]!=0){
      fMultiplicity[layer]->Fill(multiplicity[layer]);
    }
  }
  //
  int hitid[2];
  double tmpvalue[2][41];//dy5 of one side of each plane
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      tmpvalue[layer][bar]=fAdcNoPed[layer][bar][0][0];
    }
    hitid[layer]=TMath::LocMax(41,tmpvalue[layer]);
    hitid[layer]=DmpPsdBase::ConstructGlobalStripID(layer,hitid[layer]);
  }
  fHistPos->Fill(-fStripCentres[hitid[0]].y(),-fStripCentres[hitid[1]].x());
}

void DmpAlgPsdGeneralCheck::Clear()
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
  delete fHistPos;
  delete fMultiplicity[0];delete fMultiplicity[1];
  //
  for(int i=0;i<4;i++){
    fclose(fFile[i]);
  }
}