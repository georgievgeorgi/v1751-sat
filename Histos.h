#ifndef _Histos_h_
#define _Histos_h_ 0

#include<sstream>
#include<string>
#include<map>

#include"TH1F.h"
#include"TH2F.h"
#include"TGraph.h"
#include"TFile.h"
#include"TDirectory.h"
#include"TTree.h"

class Histos{
  private:
    const static int fNChanMax=8;
    std::string fOutRootFileName;

  public:
    TH2F* hist2f_OscCum      [fNChanMax];
    TH1F* hist1f_Pedestals   [fNChanMax];
    TH1F* hist1f_Spectrum    [fNChanMax];
    TH1F* hist1f_TimeBTwTwoEvt [fNChanMax];
    TH2F* hist2f_SignalStudy [fNChanMax];

    // Declare your histograms here



    void CreateHistos(){
      std::string name, title;
      TObject* tmptobj;

      // Create your histograms here

      for(int ii=0;ii<fNChanMax;++ii){
        std::stringstream chan_i_ss;
        chan_i_ss<<ii;
        name="CumulativeOscillogram_ch_"+chan_i_ss.str();  title=name+" ";
        tmptobj=hist2f_OscCum[ii]=new TH2F(name.c_str(),title.c_str(),1024,-.5,1023.5, 2048,-1,1) ;
        fAllObj.emplace("SigCumOsc",tmptobj);



        name="Pedestals_ch_"+chan_i_ss.str();  title=name+" ";
        tmptobj=hist1f_Pedestals[ii]=new TH1F(name.c_str(),title.c_str(),500,3500-.5,4000-.5);
        fAllObj.emplace("SigPedestals",tmptobj);

        name="Spectrum_ch_"+chan_i_ss.str();  title=name+" ";
        tmptobj=hist1f_Spectrum[ii]=new TH1F(name.c_str(),title.c_str(),2000,-1,30);
        fAllObj.emplace("SigSpectrum",tmptobj);


        name="TimeBTwTwoEvt_ch_"+chan_i_ss.str();  title=name+" ";
        tmptobj=hist1f_TimeBTwTwoEvt[ii]=new TH1F(name.c_str(),title.c_str(),10000,0,1);
        fAllObj.emplace("TimeBTwTwoEvt",tmptobj);


        name ="SignalStudy_ch_"+chan_i_ss.str();  title=name;
        tmptobj=hist2f_SignalStudy[ii]=new TH2F(name.c_str(),title.c_str(),1024,0-.5,1024-.5,1000,-.5,100-.5);
        fAllObj.emplace("StudySignal",tmptobj);



      }// end for ii<fNChanMax

    }





  public:
    static Histos& GetInstance();

    void SetFileName(const std::string& fn){
      fOutRootFileName = fn;
      CreateHistos();
    }
    void SaveDelete(){
      WriteHistos();
      DeleteHistos();
    }

  private:
    std::multimap<std::string,TObject*>fAllObj;

  private:
    Histos(){
      TFile* a=new TFile("tmp.root");a->cd();}


    ~Histos(){
      //SaveDelete();
    }

    //void AddToFile(const std::string& dirname, TObject* obj){fAllObj.emplace(dirname,obj);}
    void WriteHistos(){
      TFile* outRootFileP=new TFile(fOutRootFileName.c_str(),"recreate");
      TDirectory* fileDir=outRootFileP;
      std::stringstream chan_i_ss;
      fileDir->cd();
      std::string curdirname;
      for(auto obj_it=fAllObj.begin();obj_it!=fAllObj.end();++obj_it){
        if(curdirname!=obj_it->first){
          curdirname=obj_it->first;
          fileDir->mkdir(curdirname.c_str())->cd();
        }
        if(curdirname.size()==0)fileDir->cd();
        TH1*hist=dynamic_cast<TH1*>(obj_it->second);
        if(hist!=nullptr&&hist->GetEntries()==0)continue;
        obj_it->second->Write();
      }
      outRootFileP->Write();
      outRootFileP->Close();
      delete outRootFileP;
    }

    void DeleteHistos(){
      for(auto obj_it=fAllObj.begin();obj_it!=fAllObj.end();++obj_it){
        delete obj_it->second;
      }
    }
};
#endif
