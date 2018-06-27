#ifndef _v1751raw_h_
#define _v1751raw_h_

#include<string>
#include<iostream>
#include<iomanip>
#include<map>
#include<iostream>
#include<fstream>
#include<exception>

class Header{
  public:
    typedef union{
      uint32_t rows[4];
      struct __attribute__((packed)){
        uint32_t row1;
        uint32_t row2;
        uint32_t row3;
        uint32_t row4;
      };

      struct __attribute__((packed)){
        // Row1
        uint32_t eventSize   :28;
        uint32_t header1010  : 4;

        // Row2
        union __attribute__((packed)){
          struct __attribute__((packed)){
            unsigned int ch0:1;
            unsigned int ch1:1;
            unsigned int ch2:1;
            unsigned int ch3:1;
            unsigned int ch4:1;
            unsigned int ch5:1;
            unsigned int ch6:1;
            unsigned int ch7:1;
          };
          unsigned int channelMask:8 ;
        };
        uint32_t patternLVDS   :16;
        uint32_t zero          : 1;
        uint32_t res           : 2;
        uint32_t boardID       :5;

        // Row3
        uint32_t eventCounter  :24;
        uint32_t reserved3     : 8;
        // Row4
        uint32_t triggerTimeTag; // :32
      };
    } __attribute__((packed)) header_t;

  public:
    Header():fHeaderHandler((char*)(&fHeader.rows[0])){}
    ~Header(){}

    uint32_t     GetEventSize         ()        const{return fEventSize;}
    uint32_t     GetChannelSize       ()        const{return fChannelSize;}
    unsigned int GetChannelMask       ()        const{return fChannelMask;}
    uint32_t     GetEventId           ()        const{return fEventCounter;}
    uint32_t     GetEventTimeTag      ()        const{return fTriggerTimeTag;}
    unsigned int GetNChannels         ()        const{return fNChannels;}
    bool         HasChannel           (int chID)const{return fHasCh[chID];}
    void Info(){
      std::cout<<std::hex
        <<"0x"<< std::setfill('0') << std::setw(8)<<fHeader.row1       <<"   "
        <<"0x"<< std::setfill('0') << std::setw(8)<<fHeader.row2       <<"   "
        <<"0x"<< std::setfill('0') << std::setw(8)<<fHeader.row3       <<"   "
        <<"0x"<< std::setfill('0') << std::setw(8)<<fHeader.row4       <<std::endl
        <<"eventSize      " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.eventSize       <<std::endl
        <<"channelSize      " << std::setfill('0') << std::setw(8)<<std::hex<<fChannelSize       <<std::endl
        <<"header1010     " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.header1010      <<std::endl
        <<"channelMask    " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.channelMask   <<"  "<<GetNChannels()<<"  bits set"<<std::endl
        //<<"reserved1      " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.reserved1       <<std::endl
        <<"zero           " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.zero            <<std::endl
        //<<"reserved2      " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.RES       <<std::endl
        <<"eventCounter   " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.eventCounter    <<std::endl
        <<"reserved3      " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.reserved3       <<std::endl
        <<"triggerTimeTag " << std::setfill('0') << std::setw(8)<<std::hex<<fHeader.triggerTimeTag  <<std::endl
        <<std::endl;
      return;
    }
  protected:
    uint32_t     fEventSize;
    unsigned int fChannelMask;
    uint32_t     fEventCounter;
    uint32_t     fTriggerTimeTag;
    unsigned int fNChannels;
    uint32_t     fChannelSize;
    bool         fHasCh[8];
  public:
    void PostProc(){
      fNChannels=0;
      for(int i=0;i<8;i++){
        fHasCh[i]=(fHeader.channelMask>>i&0x1); //checks if i-th bit is set
        //std::cout<<__FILE__<<":"<<__LINE__<<"  ch "<<i<<"  "<<fHasCh[i]<<std::endl;
        if(fHeader.channelMask>>i&0x1) ++fNChannels; //counts enabled channels
      }
      fEventSize=fHeader.eventSize-4; // the real event size is w/o the header
      fChannelSize=fEventSize/fNChannels; // evt size by nChannels = channel size [words]

      fChannelMask   = fHeader.channelMask;
      fEventCounter  = fHeader.eventCounter;
      fTriggerTimeTag= fHeader.triggerTimeTag;
    }
  public:
    header_t fHeader;
    char* fHeaderHandler;
};

class ChannelSamples{
  public:
    ChannelSamples() :fSamplesHandler((char*)(&fRawDat)){}
    typedef union{
      struct __attribute__((packed)){
        uint32_t event0   :10;
        uint32_t event1   :10;
        uint32_t event2   :10;
        uint32_t nEvents  : 2;
      };
      uint32_t word;
    }__attribute__((packed)) sample_word_t;


  private:
    int fNSamp;
    int fNWords;
    sample_word_t fRawDat[350];
    int fSamp[1024];
  public:
    char* fSamplesHandler;
    void SetNWords(int nw){fNWords=nw;Process();}
    int GetNWords() const {return fNWords;}
    const int * GetSamples(){return fSamp;}
    const int GetSamp(int i)const {return fSamp[i];}
    int GetNSamp()const{return fNSamp;}
    void Process(){
      fNSamp=0;
      for(int word_i=0;word_i<fNWords;++word_i){
        if(fRawDat[word_i>0?word_i-1:0].nEvents!=3||
            fRawDat[word_i].nEvents<1||
            fRawDat[word_i].nEvents>3
          ){
          std::cerr<<"something went wrong  "<<__FILE__<<":"<<__LINE__<<std::endl;
          exit(99);
        }
        switch (fRawDat[word_i].nEvents){
          case 3:
            fSamp[fNSamp++]=fRawDat[word_i].event0;
            fSamp[fNSamp++]=fRawDat[word_i].event1;
            fSamp[fNSamp++]=fRawDat[word_i].event2;
            break;
          case 2:
            fSamp[fNSamp++]=fRawDat[word_i].event0;
            fSamp[fNSamp++]=fRawDat[word_i].event1;
            break;
          case 1:
            fSamp[fNSamp++]=fRawDat[word_i].event0;
            break;
        }
      }
    }

};

class RawEvent{
  public:
    class ChannelNotFound:std::exception{};
  public:
    RawEvent(){ }

    const ChannelSamples& GetChannel(int ch){
      if(!fHeader.HasChannel(ch))throw ChannelNotFound();
      return fChans[ch];
    }

  public:
    Header fHeader;
    std::map<int,ChannelSamples> fChans;
};

class RawFile{
  public:
    RawFile(const std::string& fn):
      fEventCounter(0){
        fRawFile.open (fn.c_str(), std::ios::binary );
      }
      ~RawFile(){fRawFile.close();}


  private:
    RawEvent fEvent;
  private:
    RawFile();
  public:
    RawEvent& GetEvent(){return fEvent;}
    bool IsOpened(){return fRawFile.is_open();}
    bool GetNextEvt(){
      fEventCounter++;
      fRawFile.read(fEvent.fHeader.fHeaderHandler, 16);
      fEvent.fHeader.PostProc();
      if(fRawFile.eof())return false;
      //fEvent.fHeader.Info();
      for(int ch_i=0;ch_i<8;++ch_i){
        if(fEvent.fHeader.HasChannel(ch_i)){
          fEvent.fChans[ch_i].SetNWords(fRawFile.read(fEvent.fChans[ch_i].fSamplesHandler,fEvent.fHeader.GetChannelSize()*4).gcount()/4);
          if(fEvent.fChans[ch_i].GetNWords()!=fEvent.fHeader.GetChannelSize()){
            std::cout<<__FILE__<<":"<<__LINE__<<" ch_i="<<ch_i<<"   "<<"   "<<fEvent.fChans[ch_i].GetNWords()<<"   "<<fEvent.fHeader.GetChannelSize()<<std::endl;
          }
        }
        if(fRawFile.eof())return false;
      }
      return true;
    }
  private:
    std::ifstream fRawFile;
    int fEventCounter;

};

#endif
