#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"

int analyze_psd(const char* filename)
{
  static const char LAYERNAME[2][2]={"X","Y"};
  static const char SIDENAME[2][4]={"Neg","Pos"};
  static const char DYNODENAME[2][4]={"Dy5","Dy8"};
  //
  TFile *infile=new TFile(filename);
  TTree *tree=(TTree*)infile->Get("Psd");
  
  //fAdc[layer][bar][side][dynode]:
  //	layer: 0 is X-layer, 1 is Y-layer
  //	bar:   0->40 neg->pos
  //	side:  0 is negative side, 1 is positive side
  //	dynode:0 is dynode_5,      1 is dynode_8 
  double fAdc[2][41][2][2];
  tree->SetBranchAddress("Adc",fAdc);
  
  TH1F* hist[2][41][2][2];
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      for(int side=0;side<2;side++){
	for(int dynode=0;dynode<2;dynode++){
	  hist[layer][bar][side][dynode]=new TH1F(Form("h%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[dynode]),Form("%s_%d_%s_%s",LAYERNAME[layer],bar+1,SIDENAME[side],DYNODENAME[dynode]),1700,-0.5,16999.5);
	}
      }
    }
  }
  //
  int entries=tree->GetEntries();
  for(int i=0;i<entries;i++){
    tree->GetEntry(i);
    
    for(int layer=0;layer<2;layer++){
      for(int bar=0;bar<41;bar++){
	for(int side=0;side<2;side++){
	  for(int dynode=0;dynode<2;dynode++){
	    hist[layer][bar][side][dynode]->Fill(fAdc[layer][bar][side][dynode]);
	  }
	}
      }
    }
    
  }
  //
  TCanvas c1("c1","c1",1000,1000);
  c1.Divide(2,2);
  for(int i=0;i<4;i++){
    c1.cd(i+1);
    gPad->SetLogy();
  }
  
  c1.Print("psd_result.pdf[");
  for(int layer=0;layer<2;layer++){
    for(int bar=0;bar<41;bar++){
      c1.cd(1);
      hist[layer][bar][0][0]->Draw();
      
      c1.cd(2);
      hist[layer][bar][0][1]->Draw();
      
      c1.cd(3);
      hist[layer][bar][1][0]->Draw();
      
      c1.cd(4);
      hist[layer][bar][1][1]->Draw();
      
      c1.Print("psd_result.pdf");
      
    }
  }
  c1.Print("psd_result.pdf]");
  //
  
  delete infile;
  
  return 0;
}