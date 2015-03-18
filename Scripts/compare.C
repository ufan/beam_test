{
  gROOT->Reset();
  
  TFile* fint=new TFile("A2Data00_20150317-210003_ADC-Simple.root");
  TH1F*  hint=(TH1F*)fint->Get("hX_28_Pos_Dy8");
  hint->SetName("hint");hint->SetLineColor(kRed);
  
  TFile* fext=new TFile("A2Data00_20150318-052154_ADC-Simple.root");
  TH1F*  hext=(TH1F*)fext->Get("hX_28_Pos_Dy8");
  hext->SetName("hext");hext->SetLineColor(kBlue);
  
  hint->Scale(hint->Integral());
  hext->Scale(hext->Integral());
  
  TCanvas* ccmp=new TCanvas("ccmp","ccmp",500,500);
  hint->Draw();
  hext->Draw("same");
  //
  ccmp->BuildLegend();
}