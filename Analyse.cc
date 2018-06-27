#include"Analyse.h"
#include <TF1.h>

  void
Analyse::Initialise()
{
}
  void
Analyse::Process(RawEvent& evt)
{
  double pedestals[8]={0};
  for(int chan_i=0;chan_i<1;++chan_i){
    int pedestalsamples=0;
    for(int samp_i=0;samp_i<evt.GetChannel(chan_i).GetNSamp();++samp_i){
      Histos::GetInstance().hist2f_OscCum[chan_i]->Fill(samp_i,evt.GetChannel(chan_i).GetSamp(samp_i)/4096.);
      if(samp_i>20&&samp_i<180){
        pedestalsamples++;
        pedestals[chan_i]+=evt.GetChannel(chan_i).GetSamp(samp_i)/4096.;
      }
    }
    pedestals[chan_i]/=pedestalsamples;
  }


  static int nfilled=0;
  nfilled++;
  double energyDeposit[8]={0};
  for(int chan_i=0;chan_i<1;++chan_i){
    for(int samp_i=0;samp_i<evt.GetChannel(chan_i).GetNSamp();++samp_i){
      if(samp_i>320&&samp_i<420){
      energyDeposit[chan_i]+=evt.GetChannel(chan_i).GetSamp(samp_i)/4096.-pedestals[chan_i];
      }


      if(nfilled<1000){
        Histos::GetInstance().hist2f_SignalStudy       [chan_i]->SetBinContent(samp_i+1,nfilled,pedestals[chan_i]-evt.GetChannel(chan_i).GetSamp(samp_i)/4096.);
      }

      Histos::GetInstance().hist1f_Spectrum[chan_i]->Fill(-energyDeposit[chan_i]);
    }


    static uint32_t prevEvtTimeStamp=0;
    uint32_t currEvtTimeStamp=evt.fHeader.GetEventTimeTag();
    Histos::GetInstance().hist1f_TimeBTwTwoEvt       [chan_i]->Fill(double(currEvtTimeStamp-prevEvtTimeStamp)/125e6);
    prevEvtTimeStamp=currEvtTimeStamp;
  }
}


  void
Analyse::Finalise()
{
  TF1* fit=new TF1("expcustom","([0]*exp(-[1]*x))",0.00061,1e7);
  fit->SetParName(0,"Constant");
  fit->SetParName(1,"Slope");
  Histos::GetInstance().hist1f_TimeBTwTwoEvt [0]->Fit("expcustom","RQ");
  double para =fit->GetParameter(0);
  double parb =fit->GetParameter(1);
  double dpara=fit->GetParError(0);
  double dparb=fit->GetParError(1);

  //std::cout <<para     <<"   " <<dpara    <<"   " <<parb     <<"   " <<dparb    <<std::endl;

  std::cout<<para/parb*1e4<<"  "<<(para*dparb+parb*dpara)/parb/parb*1e4<< " entr "<<Histos::GetInstance().hist1f_TimeBTwTwoEvt [0]->GetEntries()<<std::endl;
  delete fit;



}
