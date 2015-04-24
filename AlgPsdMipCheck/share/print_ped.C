#include "TFile.h"
#include "TTree.h"
#include "stdio.h"

int print_ped(const char* pedfile,const char* outDir)
{

    int id8[41]={0,1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,46,47,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66};
    int id5[41]={23,24,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,45,68,69,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88};
    TString label[4]={"xneg","xpos","yneg","ypos"};

    Float_t xdy5ped_mean[41][2],xdy5ped_sigma[41][2],ydy5ped_mean[41][2],ydy5ped_sigma[41][2];
    Float_t xdy8ped_mean[41][2],xdy8ped_sigma[41][2],ydy8ped_mean[41][2],ydy8ped_sigma[41][2];
    Float_t xmean_buffer,xsigma_buffer,ymean_buffer,ysigma_buffer;

    Float_t x,y;
    Int_t strip_id;

    TFile* fped=new TFile(pedfile);
    TTree* txped=0;
    TTree* typed=0;
    
    

    for(Int_t fee_id=0;fee_id<2;fee_id++){
        txped=(TTree*)fped->Get(Form("%s_ped",label[fee_id].Data()));
        txped->BuildIndex("channel");
        txped->SetBranchAddress("mean",&xmean_buffer);
        txped->SetBranchAddress("sigma",&xsigma_buffer);

        typed=(TTree*)fped->Get(Form("%s_ped",label[fee_id+2].Data()));
        typed->BuildIndex("channel");
        typed->SetBranchAddress("mean",&ymean_buffer);
        typed->SetBranchAddress("sigma",&ysigma_buffer);
        for(Int_t i=0;i<41;i++){
            if(fee_id==0){
                txped->GetEntryWithIndex(id8[40-i]+1);
                typed->GetEntryWithIndex(id8[i]+1);
		
		xdy8ped_mean[i][0]=xmean_buffer;
		xdy8ped_sigma[i][0]=xsigma_buffer;
		ydy8ped_mean[i][0]=ymean_buffer;
		ydy8ped_sigma[i][0]=ysigma_buffer;
            }
            else{
                txped->GetEntryWithIndex(id8[i]+1);
                typed->GetEntryWithIndex(id8[40-i]+1);
		
		xdy8ped_mean[i][1]=xmean_buffer;
		xdy8ped_sigma[i][1]=xsigma_buffer;
		ydy8ped_mean[i][1]=ymean_buffer;
		ydy8ped_sigma[i][1]=ysigma_buffer;
            }   
        }
    }

    for(Int_t fee_id=0;fee_id<2;fee_id++){
        txped=(TTree*)fped->Get(Form("%s_ped",label[fee_id].Data()));
        txped->BuildIndex("channel");
        txped->SetBranchAddress("mean",&xmean_buffer);
        txped->SetBranchAddress("sigma",&xsigma_buffer);

        typed=(TTree*)fped->Get(Form("%s_ped",label[fee_id+2].Data()));
        typed->BuildIndex("channel");
        typed->SetBranchAddress("mean",&ymean_buffer);
        typed->SetBranchAddress("sigma",&ysigma_buffer);
        for(Int_t i=0;i<41;i++){
            if(fee_id==0){
                txped->GetEntryWithIndex(id5[40-i]+1);
                typed->GetEntryWithIndex(id5[i]+1);
		
		xdy5ped_mean[i][0]=xmean_buffer;
		xdy5ped_sigma[i][0]=xsigma_buffer;
		ydy5ped_mean[i][0]=ymean_buffer;
		ydy5ped_sigma[i][0]=ysigma_buffer;
            }
            else{
                txped->GetEntryWithIndex(id5[i]+1);
                typed->GetEntryWithIndex(id5[40-i]+1);
		
		xdy5ped_mean[i][1]=xmean_buffer;
		xdy5ped_sigma[i][1]=xsigma_buffer;
		ydy5ped_mean[i][1]=ymean_buffer;
		ydy5ped_sigma[i][1]=ysigma_buffer;
            }   
        }
    }
    
    FILE *fp=0;
    for(int i=0;i<2;i++){
      
      fp=fopen(Form("%s/%s_ped.csv",outDir,label[i].Data()),"w");
      if(!fp){
	printf("can't open %s\n",Form("%s/%s_ped.csv",outDir,label[i].Data()));
	return -1;
      }
      fprintf(fp,"dy5_mean,dy5_sigma,dy8_mean,dy8_sigma\n");
      printf("fuck\n");
      for(int j=0;j<41;j++){
	
	fprintf(fp,"%.3f,%.3f,%.3f,%.3f\n",xdy5ped_mean[j][i],xdy5ped_sigma[j][i],
					   xdy8ped_mean[j][i],xdy8ped_sigma[j][i]);
      }
      fclose(fp);
    }
    
    for(int i=0;i<2;i++){
      fp=fopen(Form("%s/%s_ped.csv",outDir,label[i+2].Data()),"w");
      fprintf(fp,"dy5_mean,dy5_sigma,dy8_mean,dy8_sigma\n");
      for(int j=0;j<41;j++){
	fprintf(fp,"%.3f,%.3f,%.3f,%.3f\n",ydy5ped_mean[j][i],ydy5ped_sigma[j][i],
					   ydy8ped_mean[j][i],ydy8ped_sigma[j][i]);
      }
      fclose(fp);
    }
    
    
    delete fped;

    return 0;
}
