#ifndef DmpAlgPsdGeneralCheck_H
#define DmpAlgPsdGeneralCheck_H

#include "DmpVAlg.h"
#include "DmpEvtPsdRaw.h"
#include "TVector3.h"
#include <string>
#include <map>
#include <stdio.h>

class TF1;
class TH1F;
class TH2F;

class DmpAlgPsdGeneralCheck : public DmpVAlg{
/*
 *  DmpAlgPsdGeneralCheck
 *
 */
public:
  DmpAlgPsdGeneralCheck();
  ~DmpAlgPsdGeneralCheck();

  void Set(const std::string &type,const std::string &value);
  // if you need to set some options for your algorithm at run time. Overload Set()
  bool Initialize();
  bool ProcessThisEvent();    // only for algorithm
  bool Finalize();
  
private:
  //histograms
  TH1F		*fHist[2][41][2][2];
  TH2F		*fDy58[2][41][2];
  TH2F		*fHistPos;
  TH1F		*fMultiplicity[2];
  
  DmpEvtPsdRaw	*fRaw;
  FILE		*fFile[4];
  
private:
  std::string	fDirName;
  std::string	fPrefix;
  
  std::string	fCalibParaPath;
  double fPedMeanSeed[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaSeed[2][41][2][2];
  double fMipSeed[2][41][2][3];//layer,bar,side, 0:MPV 1:Gsigma 2:Lwidth
  double fMipSqrt[2][41];
  std::map<short,TVector3> fStripCentres;
  
  double fPedMeanFit[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaFit[2][41][2][2];
  
  double fDy58Fit[2][41][2];
  //
  double fAdc[2][41][2][2];
  double fAdcNoPed[2][41][2][2];
  
private:
  bool		GetPedSeed();
  bool		GetMipSeed();
  void		GetStripPosition();
  
  void		GetPedFit();
  TF1*		FitPed(TH1F* poHist,double& ped_mean,double& ped_sigma,double& pedout_mean,double& pedout_sigma);
  
  void		GetDy58Fit();
  TF1*		FitDy58(TH2F* hprofile,Double_t *fitrange,Double_t& dy58);
  
  void		FillMutiplicity();
  void		Clear();
private:
  static const char LAYERNAME[2][2];
  static const char SIDENAME[2][4];
  static const char DYNODENAME[2][4];
};

#endif
