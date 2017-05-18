// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "AmpGen/ParticlePropertiesList.h"
#include "AmpGen/MsgService.h"
#include "AmpGen/Utilities.h"

#include <string>
#include <fstream>
#include <cstdio>
#include <stdlib.h>

using namespace std;
using namespace AmpGen;

ParticlePropertiesList* ParticlePropertiesList::ptr=0;
std::string ParticlePropertiesList::_MintDalitzSpecialParticles="MintDalitzSpecialParticles.csv";

std::vector<std::string> ParticlePropertiesList::_dirList;


const ParticlePropertiesList* ParticlePropertiesList::getMe(){
  if(! ptr){
    ptr = new ParticlePropertiesList();
  }
  if(0 == ptr){
    ERROR(" Couldn't get ParticlePropertiesList (i.e. myself)");
    throw "No ParticlePropertiesList";
  }
  return ptr;
}

void ParticlePropertiesList::printCannotFindWarning(const std::string& where
    , const std::string& what){
  WARNING( "Got ParticlePropertiesList, but couldn't find " << what << " in " << where );
}
void ParticlePropertiesList::printCannotFindWarning(const std::string& where
    , int what){

  WARNING( "Got ParticlePropertiesList, but couldn't find " << what << " in " << where );
}

double ParticlePropertiesList::mass(const std::string& name) {
  const ParticleProperties* props = getMe()->get(name);
  if(0 == props){
    printCannotFindWarning("mass", name);
    return -9999;
  }    
  return props->mass();
}
double ParticlePropertiesList::mass(int pid) {
  const ParticleProperties* props = getMe()->get(pid);
  if(0 == props){
    printCannotFindWarning("mass", pid);
    return -9999;
  }    
  return props->mass();
}
double ParticlePropertiesList::width(const std::string& name) {
  const ParticleProperties* props = getMe()->get(name);
  if(0 == props){
    printCannotFindWarning("width", name);
    return -9999;
  }    
  return props->width();
}
double ParticlePropertiesList::width(int pid) {
  const ParticleProperties* props = getMe()->get(pid);
  if(0 == props){
    printCannotFindWarning("width", pid);
    return -9999;
  }    
  return props->width();
}


const std::vector<std::string>& ParticlePropertiesList::dirList(){
  if(_dirList.empty()) fillDirList();
  return _dirList;
}

void ParticlePropertiesList::fillDirList(){
  _dirList.clear();

  //   Get directory to AmpGenROOT

  std::string MintRoot(".");
  char * Mintenv(0);
  Mintenv = getenv ("AMPGENROOT");
  if (NULL != Mintenv){
    MintRoot = Mintenv;
  }

  _dirList.push_back("");
  _dirList.push_back( MintRoot + "/options/");
  _dirList.push_back("../");
}


FILE* ParticlePropertiesList::findFiles(){
  static int i=0;
  //  cout << " i = " << i << endl;
  FILE* f(0);
  if(0 == i){
    f = findThisFile(_fname);
    if(0 == f) findThisFile("mass_width.csv"); // making it backward compatible with old typo
    if(0 == f)i++;
  }
  if(1 == i){
    f = findThisFile(_MintDalitzSpecialParticles);
    if(0 == f) i++;
  }

  i = (i+1)%3;

  return f;
}
FILE* ParticlePropertiesList::findThisFile(const std::string& fname){
  bool verbose=true;
  for(std::vector<std::string>::const_iterator it = dirList().begin();
      it != dirList().end();
      it++){

    std::string tryThisDir = (*it);
    std::string fullFname = tryThisDir + fname;
    if(verbose) INFO( "Trying: " << fullFname ) ;
    FILE* f=fopen(fullFname.c_str(), "r");
    if(0 != f){
      INFO( "Using: " << fullFname ) ;
      return f;
    }

  }
  ERROR( "Cannot find file " << fname << " in any directory" );
  exit(1);
  return (FILE*) 0;
}

ParticlePropertiesList::ParticlePropertiesList(const std::string& fname_in){
  _fname=fname_in;
  if(! readFiles()){
    ERROR( "Problem reading file " << _fname );
  }
}

void ParticlePropertiesList::readLatexLabels(){
  std::string fname_in = getenv("AMPGENROOT")+std::string("/options/pdgID_to_latex.dat");
  auto lines = vectorFromFile( fname_in );
  m_latexLabels.clear();
  for( auto& line : lines ){
    auto tokens = split( line, ' ' );
    m_latexLabels[ stoi( tokens[0] ) ] = 
      std::make_pair( tokens[1], tokens[2] );
  }
}


bool ParticlePropertiesList::readFiles(){
  bool shoutIfTwice=true;
  int counter=0;
  readLatexLabels();
  while(FILE* f = findFiles()){
    INFO("Reading " << counter << "th file" );
    if(0==f) return false;

    static const int lineLength=800;
    char line[lineLength]={'\0'};

    while(fgets(line, lineLength, f)){
      ParticleProperties P(line);
      auto label = m_latexLabels.find( P.pdg_id() );
      if( label != m_latexLabels.end() )
        P.setLabel( label->second.first );
      if(! P.isValid()) continue;

      theList.push_back(P);
      if(P.hasDistinctAnti()){
        P.antiThis();
        if( label != m_latexLabels.end() ) 
          P.setLabel( label->second.second );
        theList.push_back(P);
      }
    }
    fclose(f);
  }
  // make maps for fast particle finding:
  for(auto& it : theList ){
    byName[it.name()] = &it;
    if(shoutIfTwice && it.pdg_id() != 0){
      auto found = byID.find(it.pdg_id());
      if(found != byID.end()){
        WARNING(
            "pdg_id " << it.pdg_id() << " used twice, here: " 
            << (found->second)->name()
            << ", and here: " << it.name() );
      }

    }
    byID[it.pdg_id()] = &it;
  }

  return true;
}

void ParticlePropertiesList::print(std::ostream& out) const{
  for(std::list<ParticleProperties>::const_iterator it= theList.begin();
      it != theList.end(); it++){
    it->print(out);
    out << "\n";
  }
}

const ParticleProperties* ParticlePropertiesList::get(const std::string& name) const{
  auto it = byName.find(name);
  if(it == byName.end()) return 0;
  return it->second;
}
const ParticleProperties* ParticlePropertiesList::get(int id) const{
  auto it = byID.find(id);
  if(it == byID.end()) return 0;
  return it->second;
}

std::ostream& operator<<(std::ostream& out, const ParticlePropertiesList& ppl){
  ppl.print(out);
  return out;
}

//
