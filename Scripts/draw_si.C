void draw_si(const char* filename, int id1,int id2,int id3,int id4){
  gROOT->Reset();
  
  TFile* f=new TFile(filename);
  TTree* t=(TTree*)f->Get("Psd");
  TH2F* h2=new TH2F("h2","h2",350,0,3500,350,0,3500);
  t->Draw(Form("Si[%d][%d]:Si[%d][%d]>>h2",id1,id2,id3,id4),"","colz");
  
  
}

void draw_si_single(const char* filename, int id1,int id2){
  gROOT->Reset();
  
  TFile* f=new TFile(filename);
  TTree* t=(TTree*)f->Get("Psd");
  TH1F* h1=new TH2F("h1","h1",4096,0,4096);
  t->Draw(Form("Si[%d][%d]>>h1",id1,id2));
  
  
}

void draw_correlation(const char* filename,int padid,int chid)
{
    gROOT->Reset();
    
    TFile* f=new TFile(filename);
    TTree* t=(TTree*)f->Get("Psd");
    
    TH2F* h2=new TH2F("h2","h2",300,0,3000,350,0,3500);
    Psd->Draw(Form("AdcNoPed[0][%d][1][0]:Si[0][%d]>>h2",padid,chid),"","colz");
    
}


void draw_psd(const char* filename,int xchid,int ychid)
{
  gROOT->Reset();
  
  TFile* f=new TFile(filename);
  TTree* t=(TTree*)f->Get("Psd");
  
  TH2F* h2=new TH2F("h2","h2",4000,0,4000,4000,0,4000);
  t->Draw(Form("Adc[0][%d][1][0]:Adc[1][%d][1][0]",xchid,ychid),"","colz");
}

void draw_psd_single(const char* filename,int chid)
{
  gROOT->Reset();
  
  TFile* f=new TFile(filename);
  TTree* t=(TTree*)f->Get("Psd");
  
  TH1F* h1=new TH1F("h1","h1",4000,0,4000);
  t->Draw(Form("AdcNoPed[0][%d][1][0]>>h1",chid));
  
  TH1F* h2=new TH1F("h2","h2",16000,-0.5,15999.5);
  t->Draw(Form("AdcNoPed[0][%d][1][1]>>h2",chid));
}

void draw_multi()
{
  gROOT->Reset();
  
  TFile* f1=new TFile("/run/media/ufan/15beamtest/psd_simple/A2Data00_20150321-173433_ADC-Simple.root");
  TTree* t1=(TTree*)f1->Get("Psd");
  TH1F* h1=new TH1F("h1","exteranl+ANC+AMS",41,-0.5,40.5);
  t1->Draw("Multiplicity[0]>>h1");
  
  TFile* f2=new TFile("/run/media/ufan/15beamtest/psd_simple/A2Data00_20150322-145434_ADC-Simple.root");
  TTree* t2=(TTree*)f2->Get("Psd");
  TH1F* h2=new TH1F("h2","internal+ANC+AMS",41,-0.5,40.5);
  t2->Draw("Multiplicity[0]>>h2");
  
  TFile* f3=new TFile("/run/media/ufan/15beamtest/psd_simple/A2Data00_20150322-054235_ADC-Simple.root");
  TTree* t3=(TTree*)f3->Get("Psd");
  TH1F* h3=new TH1F("h3","internal,No_ANC/AMS",41,-0.5,40.5);
  t3->Draw("Multiplicity[0]>>h3");
  
  TCanvas* c1=new TCanvas("can","can",500,500);
  h1->Scale(1./h1->Integral(5,41));
  h1->SetLineWidth(2);
  h1->Draw();
  
  h3->Scale(1./h3->Integral(5,41));
  h3->SetLineWidth(2);
  h3->SetLineColor(kBlack);
  h3->Draw("same");
  
  h2->Scale(1./h2->Integral(5,41));
  h2->SetLineColor(kRed);
  h2->SetLineWidth(2);
  h2->Draw("same");
  c1->BuildLegend();
}