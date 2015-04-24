#include <fstream>
#include <iostream>
#include <string>
#include "stdio.h"

bool GetPsdMips(){
  
	double PsdMips[2][41][2][3];
	double PsdMipSqrt[2][41];
	std::ifstream PMpar;
	std::string fBgoPsdCalibPath="/home/ufan/software/DmpSoftware/share/Calibration/ParametersBgoPsd_BT2014_v1/";
	PMpar.open((fBgoPsdCalibPath + "PsdPar/PsdMips").c_str());
	if (!PMpar.good()) {
		std::cout << "Can not open Psd Mips file!" << std::endl;
		return false;
	}
	for (int l = 0; l < 2; ++l) {
		for (int b = 0; b < 41; ++b) {
			for (int s = 0; s < 2; ++s) {
				for (int p = 0; p < 3; ++p) {
					PMpar >> PsdMips[l][b][1-s][p];

				}
			}
			PsdMipSqrt[l][b]=TMath::Sqrt(PsdMips[l][b][0][0]*PsdMips[l][b][1][0]);
		}
	}
	PMpar.close();
	//
	FILE *fp=fopen("MipSVN.csv","w");
	fprintf(fp,"x_neg,x_pos,y_neg,y_pos\n");
	for(int b=0;b<41;b++){
	  for(int l=0;l<2;l++){
	    for(int s=0;s<2;s++){
	      fprintf(fp,"%.2f,",PsdMips[l][b][s][0]);
	    }
	  }
	  fprintf(fp,"\n");
	}
	fclose(fp);
	
	
	return true;
}