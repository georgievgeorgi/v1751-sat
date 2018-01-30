#include"Analyse.h"

  void
Analyse::Initialise()
{
}
  void
Analyse::Process(RawEvent& evt)
{
  for(int chan_i=0;chan_i<2;++chan_i){
    for(int samp_i=0;samp_i<evt.GetChannel(chan_i).GetNSamp();++samp_i){
      Histos::GetInstance().hist2f_OscCum[chan_i]->Fill(samp_i,evt.GetChannel(chan_i).GetSamp(samp_i)/4096.);
    }
  }
}
  void
Analyse::Finalise()
{
}
