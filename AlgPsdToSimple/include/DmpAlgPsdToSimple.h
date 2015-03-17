#ifndef DmpAlgPsdToSimple_H
#define DmpAlgPsdToSimple_H

#include "DmpVAlg.h"
#include "DmpEvtPsdRaw.h"
#include <string>
#include <stdio.h>

class TFile;
class TTree;
class TF1;
class TH1F;
class TH2F;

class DmpAlgPsdToSimple : public DmpVAlg{
/*
 *  DmpAlgPsdToSimple
 *
 */
public:
  DmpAlgPsdToSimple();
  ~DmpAlgPsdToSimple();

  void Set(const std::string &type,const std::string &value);
  // if you need to set some options for your algorithm at run time. Overload Set()
  bool Initialize();
  bool ProcessThisEvent();    // only for algorithm
  bool Finalize();
  
private:
  TFile		*fFile;
  TTree		*fOutTree;
  TH1F		*fHist[2][41][2][2];
  DmpEvtPsdRaw	*fRaw;
  
private:
  std::string	fDirName;
  std::string	fPrefix;
  
  /*
  std::string	fCalibParaPath;
  double fPedMeanSeed[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaSeed[2][41][2][2];
  
  double fPedMeanFit[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaFit[2][41][2][2];
  */
  double fAdc[2][41][2][2];
  
private:
  void		Clear();
private:
  static const char LAYERNAME[2][2];
  static const char SIDENAME[2][4];
  static const char DYNODENAME[2][4];
};

#endif
