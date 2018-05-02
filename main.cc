#include"main.h"
#include"Histos.h"
#include"v1751raw.h"
#include"Analyse.h"

#include<vector>

#include<algorithm>
#include<string>
#include<iostream>
#include<cctype>
#include<getopt.h>



int ndegra::gDebug;
unsigned int ndegra::gNBursts;
std::string ndegra::gOutputRootFile;

int main(int argc, char* argv[]) {
  int opt;
  std::vector<std::string> inputFiles,inputLists;
  std::string tmpstring;

  ndegra::gDebug=1; // default value
  ndegra::gNBursts=-1;


  const struct option longopts[] =
  {
    {"input",   required_argument,        0, 'i'},
    {"list" ,   required_argument,        0, 'l'},
    {0,0,0,0},
  };

  int index;
  int iarg=0;

  while ((opt = getopt_long(argc, argv, "i:l:o:d:b:", longopts, &index)) != -1){
    switch (opt){
      case 'i': tmpstring=optarg;inputFiles.push_back(tmpstring);break;
      case 'l': tmpstring=optarg;inputLists.push_back(tmpstring);break;
      case 'o': ndegra::gOutputRootFile=optarg;break;
      case 'd': ndegra::gDebug=atoi(optarg); break;
      case 'b': ndegra::gNBursts=atoi(optarg);break;
      default:
                fprintf(
                    stderr,
                    "SYNOPSIS: %s [-i input file] [-l list file] [-o root_file]\n",argv[0]);
                return 1;break;
    }
  }

  if(
      ndegra::gOutputRootFile.size()==0||
      (inputLists.size()==0&&inputFiles.size()==0))
  {
    fprintf(stderr,"SYNOPSIS: %s [-i input file] [-l list file] [-o root_file]\n",argv[0]);
    return 1;}


  char tmpchar[500];

  Histos::GetInstance().SetFileName(ndegra::gOutputRootFile);

  for(unsigned int i=0;i<inputLists.size();++i){
    std::ifstream list(inputLists[i].c_str(),std::ifstream::in);
    if(!list.is_open())continue;
    std::cerr<<"proc list "<<inputLists[i]<<std::endl;
    while(!list.eof()){
      //list>>tmpstring;
      list.getline((char*)tmpchar,500,'\n');
      tmpstring=tmpchar;
      tmpstring.erase(std::remove(tmpstring.begin(),tmpstring.end(),' '),tmpstring.end());
      if(tmpstring.find_first_of('#')!=std::string::npos)
        tmpstring.erase(tmpstring.find_first_of('#'),tmpstring.length()); // # comment char
      if(tmpstring.size()>0)
        inputFiles.push_back(tmpstring);
    }
  }

  Analyse ana;
  ana.Initialise();

  unsigned int burst_i=0;
  int event_i=0;
  for(std::vector<std::string>::iterator if_it=inputFiles.begin();
      if_it!=inputFiles.end();
      ++if_it){
    std::string& filename=*if_it;
    std::cerr<<"Processing \""<<filename<<"\""<<std::endl;
    if(burst_i>ndegra::gNBursts)break;
    burst_i++;



    RawFile raw(filename);
    if(!raw.IsOpened())continue;
    while(raw.GetNextEvt()){
      if((event_i)%1000==0)std::cerr<<event_i++<<" events processed"<<std::endl;
      ana.Process(raw.GetEvent());
    }
  }
  ana.Finalise();





  return 0;
}




// Singleton class
Histos& Histos::GetInstance()
{
  static Histos h;
  return h;
}
