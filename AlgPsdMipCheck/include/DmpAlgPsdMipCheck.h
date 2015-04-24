#ifndef DmpAlgPsdMipCheck_H
#define DmpAlgPsdMipCheck_H

#include "DmpVAlg.h"
#include "DmpEvtPsdRaw.h"
#include <string>
#include <stdio.h>

class TF1;
class TH1F;
class TH2F;

class DmpAlgPsdMipCheck : public DmpVAlg{
/*
 *  DmpAlgPsdMipCheck
 *
 */
public:
  DmpAlgPsdMipCheck();
  ~DmpAlgPsdMipCheck();

  void Set(const std::string &type,const std::string &value);
  // if you need to set some options for your algorithm at run time. Overload Set()
  bool Initialize();
  bool ProcessThisEvent();    // only for algorithm
  bool Finalize();
  
private:
  TH1F		*fHist[2][41][2][2];
  TH2F		*fSideSide[2][41][2];
  TH2F		*fDy58[2][41][2];
  DmpEvtPsdRaw	*fRaw;
  FILE		*fFile[4];
  
private:
  std::string	fDirName;
  std::string	fPrefix;
  
  std::string	fCalibParaPath;
  double fPedMeanSeed[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaSeed[2][41][2][2];
  
  double fPedMeanFit[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaFit[2][41][2][2];
  
  double PsdMips[2][41][2];//layer,bar,side
  double PsdMipSqrt[2][41];
  //
  double fAdc[2][41][2][2];
  
private:
  bool		GetPedSeed();
  void		GetPedFit();
  TF1*		FitPed(TH1F* poHist,double& ped_mean,double& ped_sigma,double& pedout_mean,double& pedout_sigma);
  void		Clear();
private:
  static const char LAYERNAME[2][2];
  static const char SIDENAME[2][4];
  static const char DYNODENAME[2][4];
};

#endif
