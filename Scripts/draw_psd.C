{
  gROOT->Reset();
  
  TFile* f=new TFile("onlydampe.root");

  TTree* t=(TTree*)f->Get("Psd");

  TH1F* h1=new TH1F("h1","h1",400,-0.5,15999.5);
  
  TCanvas* can=new TCanvas("can","can",500,500);
  can->SetLogy();
  t->Draw("AdcNoPed[0][26][0][1]>>h1","Multiplicity[0]<6");
}
