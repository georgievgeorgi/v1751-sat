#ifndef _Analyse_h_
#define _Analyse_h_ 1


#include"Histos.h"
#include"main.h"
#include"v1751raw.h"



class Analyse{


  public:
    Analyse(){}
    ~Analyse(){}
    void Initialise();
    void Process(RawEvent& evt);
    void Finalise();



};


#endif
