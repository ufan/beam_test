{
  gROOT->Reset();
  TCanvas* ccmp=new TCanvas("ccmp","ccmp",1000,500);
  ccmp->Divide(2,1);
  
  TFile* fint=new TFile("A2Data00_20150317-210003_ADC-Simple.root");
  TFile* fext=new TFile("A2Data00_20150318-052154_ADC-Simple.root");
  
  //
  TH1F*  hint=(TH1F*)fint->Get("hX_28_Pos_Dy5");
  hint->Rebin(50);
  hint->SetTitle("Int trig");hint->SetLineColor(kRed);
  
  TH1F*  hext=(TH1F*)fext->Get("hX_28_Pos_Dy5");
  hext->Rebin(50);
  hext->SetTitle("Ext trig");hext->SetLineColor(kBlue);
  
  hint->Scale(1./hint->Integral());
  hext->Scale(1./hext->Integral());
  
  ccmp->cd(1);
  hint->Draw();
  hext->Draw("same");
  
  //
  hint=(TH1F*)fint->Get("hX_28_Pos_Dy8");
  hint->Rebin(50);
  hint->SetTitle("Int trig");hint->SetLineColor(kRed);
  
  hext=(TH1F*)fext->Get("hX_28_Pos_Dy8");
  hext->Rebin(50);
  hext->SetTitle("Ext trig");hext->SetLineColor(kBlue);
  
  hint->Scale(1./hint->Integral());
  hext->Scale(1./hext->Integral());
  
  ccmp->cd(2);
  hint->Draw();
  hext->Draw("same");
  //
  ccmp->BuildLegend();
}