void draw_multi(const char* filename)
{
    gROOT->Reset();
    
    TFile* f=new TFile(filename);
    TCanvas* c=(TCanvas*)f->Get("c4");
    
    c->Draw();
    
}