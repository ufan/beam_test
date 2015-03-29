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
  int		fDampeOffset;
  int		fDampeOffsetCounter;
  
  
  TFile		*fAncFile;
  TTree		*fAncTree;
  UInt_t	fAncEntry;
  std::string	fAncDirName;
  std::string	fAncFileName;
  int		fAncOffset;
  int 		fV785[1][32];
  bool		fMergeAncFlag;
  int	 	fSi[2][2];//Si1"[0][0]:ch0,[0][1]:ch16",Si2"[1][0]:ch1,[1][1]:ch17"
  int		fV792[1][32];
  int		fSc[2][3];
  
private:
  std::string	fDirName;
  std::string	fPrefix;
  
  std::string	fCalibParaPath;
  double fPedMeanSeed[2][41][2][2];//layer,bar,side,dynode
  double fPedSigmaSeed[2][41][2][2];
  
  double 	fAdc[2][41][2][2];
  double 	fAdcNoPed[2][41][2][2];
  int		fMultiplicity[2];
  
private:
  bool		GetPedSeed();
  void		FillMutiplicity();
  void		Clear();
  
private:
  static const char LAYERNAME[2][2];
  static const char SIDENAME[2][4];
  static const char DYNODENAME[2][4];
};

#endif
