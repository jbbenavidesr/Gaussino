#include "AmpGen/Particle.h"
#include "AmpGen/IVertex.h"
#include "AmpGen/Utilities.h"
#include <bitset>
#include <fstream>

using namespace AmpGen; 

Particle::Particle( const std::string& name, 
    const Particle& p1 , 
    const Particle& p2 ) : 
  m_props( ParticlePropertiesList::getMe()->get(name) ),
  m_name(name), 
  m_index(999), 
  m_orbital(0), 
  m_spinConfigurationNumber(0),
  m_istop(true),
  m_lineshape("BW"),
  m_conjThis(false),
  m_isStateGood(true)
{
  m_daughters.push_back( std::make_shared<Particle>(p1) );
  m_daughters.push_back( std::make_shared<Particle>(p2) );
  pdg_lookup();
  sortDaughters();
  for( auto& d : m_daughters ) d->setTop(false);
  m_uniqueString = makeUniqueString();
}

Particle::Particle( const int& pdg_id, const Particle& p1, const Particle& p2 ) :
  m_props( ParticlePropertiesList::getMe()->get(pdg_id) ), 
  m_index(999), 
  m_orbital(0),
  m_spinConfigurationNumber(0),
  m_istop(true),
  m_lineshape("BW"),
  m_conjThis(false),
  m_isStateGood(true)
{
  m_daughters.push_back( std::make_shared<Particle>(p1) );
  m_daughters.push_back( std::make_shared<Particle>(p2) );
  if( m_props != 0 ) m_name = m_props->name() ;
  pdg_lookup();
  sortDaughters();
  for( auto& d : m_daughters ) d->setTop(false);
  m_uniqueString = makeUniqueString();

}

void Particle::parseModifier( const std::string& mod ){

  DEBUG("Modifier = " << mod );
  if( mod == "S" ) m_orbital = 0;
  else if( mod == "P" ) m_orbital = 1;
  else if( mod == "D" ) m_orbital = 2;
  else if( mod == "F" ) m_orbital = 3; 
  else if( mod == "0" ) m_spinConfigurationNumber=0;
  else if( mod == "1" ) m_spinConfigurationNumber=1;
  else if( mod == "2" ) m_spinConfigurationNumber=2;
  else if ( LineshapeFactory::isLineshape(mod)) m_lineshape = mod ; 
}


Particle::Particle( const std::string& decayString , 
    std::vector<std::string>& finalStates ) : 
  m_index(999), 
  m_orbital(0),
  m_spinConfigurationNumber(0),
  m_istop(true),
  m_lineshape("BW"),
  m_conjThis(false),
  m_isStateGood(true)
{
  DEBUG("Building particle from decay string = " << decayString );
  auto items = getItems(decayString);
  if( items.size() == 0 ){
    ERROR("Failed to parse string" << decayString );
    m_isStateGood = false ;
    return;
  } 
  m_modifiers = getItems( items[0], {"[","]"}, ";" );
  m_name = m_modifiers[0]; 
  m_modifiers.erase( m_modifiers.begin() );
  for(auto& modifier : m_modifiers )
    parseModifier( modifier );
  if ( items.size() == 1 ){
    for( unsigned int i = 0 ; i < finalStates.size(); ++i ){
      if( finalStates[i] != m_name ) continue;
      m_index = i;
      finalStates[i] = "###";
      break;        
    }
  }
  else {
    for( unsigned int it=1;it!=items.size(); ++it)
      m_daughters.push_back( std::make_shared<Particle>( items[it] , finalStates ) );
  }
  m_props = ParticlePropertiesList::getMe()->get(m_name);
  pdg_lookup();
  sortDaughters();
  for( auto& d : m_daughters) m_isStateGood &= d->isStateGood() ;
  if( ! isStateGood() ){
    ERROR("Amplitude " << decayString << " not configured correctly" );
  }
  m_uniqueString = makeUniqueString();
}

Particle::Particle( const std::string& name, const unsigned int& index ) : 
  m_props( ParticlePropertiesList::getMe()->get(name) ),
  m_name(name),
  m_index(index),
  m_orbital(0),
  m_spinConfigurationNumber(0),
  m_istop(true),
  m_lineshape("BW"),
  m_conjThis(false),
  m_isStateGood(true)
{
  pdg_lookup();
  for( auto& d : m_daughters ) d->setTop(false);
  m_uniqueString = makeUniqueString();
}

unsigned int Particle::spin() const { return m_spin;}

unsigned int Particle::TwoParticleHash() const {
  if ( m_daughters.size() != 2 ) return 999;
  return pid( m_spin, m_daughters[0]->spin(), m_daughters[1]->spin(), m_orbital, m_spinConfigurationNumber );
}


void Particle::pdg_lookup(){
  if( m_props != 0 ){
    if( m_lineshape == "BW" && m_name.find("NonRes") != std::string::npos ) m_lineshape = "NonRes";
    m_parity = m_props->P(); 
    if( ! isdigit( m_props->J()[0] ) ){
      ERROR("Spin not recognised! : " << m_name << " J = " << m_props->J() );
    }
    m_spin = stoi(m_props->J());
    bool isStrong = quarks() == daughterQuarks();

    if( m_name.find("NonRes") != std::string::npos ) isStrong = true;
    m_minL = m_daughters.size() == 2 ? lRange(isStrong).first : 0;
    if( m_daughters.size() == 2 ){
      DEBUG( "IsStrong ? " << isStrong 
          << " orbital =  " << m_orbital 
          << " minL   =  " << m_minL 
          << " name   =  " << m_name 
          << " d0     =  " << m_daughters[0]->name() 
          << " d1     =  " << m_daughters[1]->name( ) );
    }
    if( m_orbital == 0 ) m_orbital = m_minL; //// define in ground state unless specified ///
    m_parity_factor = m_props->pdg_id()>0?1:-1;

    for( auto& d : m_daughters ){
      d->setTop(false);
      d->setParityFactor(m_parity_factor);
    }
  }
  else {
    auto particleNames = ParticlePropertiesList::getMe()->getParticleNames(); 
    unsigned int minDistance = 9999;
    std::string suggestion = "";
    for( auto& particle : particleNames ){
      unsigned int distance = edit_distance( particle, m_name );
      if( distance < minDistance ){
        suggestion = particle; 
        minDistance = distance ;
      }
    }
    ERROR( "Particle: " << m_name << " not in PDG. Did you mean " << suggestion << "?" );   
    m_isStateGood = false;
  }
}

void Particle::CPConjugateThis(){
  int pid = ParticlePropertiesList::getMe()->get(m_name)->pdg_id();
  int new_pid;
  if(!ParticlePropertiesList::getMe()->get(m_name)->hasDistinctAnti()) new_pid = pid;
  else new_pid = -pid;
  m_props = ParticlePropertiesList::getMe()->get(new_pid);
  m_name = m_props->name();
  pdg_lookup();
  sortDaughters();
  for( auto& d : m_daughters ) d->CPConjugateThis();
  m_uniqueString = makeUniqueString();
}

Tensor Particle::P() const {

  Tensor momentum( std::vector<double>({0.,0.,0.,0.}),std::vector<unsigned int>({4}));

  if( isStable() ){
    if( m_index != 999 ){
      const std::string index=std::to_string(m_index);
      return Tensor( {
          (m_parity_factor) * Parameter( index+"_Px",0, false,true), 
          (m_parity_factor) * Parameter( index+"_Py",0, false,true),
          (m_parity_factor) * Parameter( index+"_Pz",0, false,true),
          Parameter( index+"_E" ,0, false,true) } , {4} ) ;
    }
    else ERROR( "Stable particle " <<  m_index << "is unindexed!" );
  } 
  else {
    for(auto& d : m_daughters) momentum = momentum + d->P();
  }
  if( momentum.nElements() != 4 ){
    ERROR("Momentum is not a four vector");
  }
  return momentum; 
}

Tensor Particle::Q() const {
  if( m_daughters.size() != 2 ){
    ERROR(" Q is only well defined for 2 body decay - check particle logic");
    return Tensor();
  }
  Tensor qV =  m_daughters[0]->P() - m_daughters[1]->P() ; 
  if( qV.nElements() != 4 )
    ERROR("Momentum transfer ill formed, somehow ..");
  return qV;
}

std::shared_ptr<Particle> Particle::daughter( const unsigned int& index ){ return (m_daughters[index]) ; }
std::shared_ptr<Particle> Particle::daughter( const unsigned int& index ) const { return m_daughters[index];}

std::string Particle::orbitalString() const {
  if( m_orbital == 0 ) return "S";
  else if( m_orbital == 1 ) return "P";
  else if( m_orbital == 2 ) return "D";
  else if( m_orbital == 3 ) return "F";
  else return "X";
}

bool Particle::hasModifier( const std::string& modifier ) const {
  return std::find( m_modifiers.begin(), m_modifiers.end(), modifier ) != m_modifiers.end() ;
}

std::string Particle::modifierString() const {
  //  if( m_modifiers.size() == 0 ) return "";
  std::string modString="[";
  if( m_orbital != m_minL ) modString += orbitalString(); 
  if( m_lineshape != "BW" && m_lineshape != "NonRes" ) 
    modString += ( modString == "["  ? m_lineshape : ";" + m_lineshape ); 

  std::vector<std::string> otherModifiers = {"BgSpin0","Inco","NoSym"};
  for( auto& mod : otherModifiers ){
    if( hasModifier( mod ) ) 
      modString += ( modString == "["  ? "" : ";" ) + mod;
  }
  modString += "]";
  return modString; 
}

std::string Particle::makeUniqueString() { 
  if( m_daughters.size() != 0 ){
    std::string modifier = modifierString();
    std::string val = m_name + ( modifier == "[]" ? "{" : modifier + "{" );
    for( unsigned int i = 0 ; i < m_daughters.size() -1; ++i  ) 
      val+= m_daughters[i]->uniqueString() + ",";
    val+= m_daughters[ m_daughters.size() - 1 ]->uniqueString() + "}" ;
    return val; 
  }
  else return m_name;
}

std::vector<std::shared_ptr<Particle>> Particle::getFinalStateParticles(){
  /// tree-traversal///
  std::vector<std::shared_ptr<Particle>> ffs;

  for( auto daughter : m_daughters ){
    if( daughter->isStable() ) ffs.push_back( daughter );
    else {
      auto daughter_states = daughter->getFinalStateParticles();
      for( auto& s : daughter_states ) ffs.push_back(s);
    }
  }
  return ffs;
}

bool Particle::isStable() const { return m_daughters.size() == 0 ; }

Expression Particle::Lineshape(std::vector<DBSYMBOL>* db) const{

  if( db!= 0 && ! isStable() ) db->push_back( DBSYMBOL( uniqueString(), Parameter("NULL", 0,true) ) );
  Expression total(1.);

  if( m_istop && m_daughters.size() == 2 ){   
    std::string topShape =  m_lineshape == "BW" ? "FormFactor" : m_lineshape;                              
    const Expression s = mass() * mass();
    DEBUG("Getting lineshape for " << m_name << " = " << topShape );
    total = total* LineshapeFactory::getLineshape( topShape , 
        s, daughter(0)->massSq(), daughter(1)->massSq(), 
        m_name, m_orbital, db) ;
  }
  if( m_daughters.size() == 0 ) return total;
  if( !m_istop && m_daughters.size() == 2 ){
    auto propagator = LineshapeFactory::getLineshape( m_lineshape, 
        massSq(), daughter(0)->massSq(), daughter(1)->massSq(), 
        m_name, m_orbital, db );
    auto daughter0_shape = daughter(0)->Lineshape(db);
    auto daughter1_shape = daughter(1)->Lineshape(db);
    return propagator*daughter0_shape*daughter1_shape;
  }
  else if ( m_daughters.size() != 0 ){
    for( auto& d : m_daughters ) total = total*d->Lineshape(db);
  }
  Expression shape = total; 
  if( db!=0) db->push_back( DBSYMBOL( uniqueString()+"_total" , total ) );
  return total ;
}

Expression Particle::getExpression(std::vector<DBSYMBOL>* db , const unsigned int& index) {

  Expression total(0);
  auto finalStateParticles = getFinalStateParticles();
  std::sort( finalStateParticles.begin(), finalStateParticles.end(), 
      []( const std::shared_ptr<Particle>& p1, const std::shared_ptr<Particle>& p2){ 
      return p1->index() < p2->index() ; 
      } ); /// get the final state particles ordered by index /// 
  std::vector<std::string> permutation0_names ; 
  std::vector<unsigned int> indices( finalStateParticles.size() ) ;
  std::iota( indices.begin() , indices.end() , 0 );
  for( auto& particle : finalStateParticles ) 
    permutation0_names.push_back( particle->name() );
  double nPermutations=0;
  bool doBoseSymmetrisation = ! hasModifier("NoSym");
  bool sumAmplitudes = ! hasModifier("Inco");
  do {
    DEBUG("Getting first permutation of " << uniqueString() );
    bool isSameAsP0 = true;
    for( unsigned int i = 0 ; i < indices.size(); ++i)
      if( permutation0_names[i] != finalStateParticles[ indices[i] ]->name() ) 
        isSameAsP0 = false;
    if( isSameAsP0 ){
      for( unsigned int i = 0 ; i < indices.size() ; ++i ){
        finalStateParticles[i]->setIndex( indices[i] );
      }
      const Expression spinPart = hasModifier("BgSpin0") ? Constant(1) : SpinTensor(db).get(index);
      DEBUG("Got spin matrix element -> calculating lineshape product");
      if( sumAmplitudes ) total = total + Lineshape(db) * spinPart;
      else {
        Expression ls = Lineshape(db) * spinPart; 
        Expression conjLs = ls.conjugate();
        Expression prod = ls*conjLs;
        if( db != 0 ){
          add_debug ( ls, db );
          add_debug( conjLs , db );
          add_debug( prod, db );
        }
        total = total + prod ; 
      }
      nPermutations++;
    }

  } while( doBoseSymmetrisation && std::next_permutation( indices.begin(), indices.end() ) );
  if( db != 0 ) add_debug( total, db );

  if( sumAmplitudes ) return total / Sqrt( nPermutations );
  else {
    Expression sqrted = Sqrt( total / nPermutations );
    if( db != 0 ) add_debug( sqrted , db );
    return sqrted; 
  }
}

Tensor Particle::SpinTensor( std::vector<DBSYMBOL>* db) {

  if( m_daughters.size() == 0 ) return FinalStateSpinTensor();
  else if( m_daughters.size() == 2  ){
    DEBUG("Getting spin tensor for " << TwoParticleHash() );
    Tensor value = 
      VertexFactory::getSpinFactor( P(), Q(), 
          daughter(0)->SpinTensor(db), daughter(1)->SpinTensor(db),  
          TwoParticleHash(), db );
    if( m_istop && db != 0 ) add_debug( value[0] , db ) ; 
    DEBUG("Returning spin tensor");
    return value; 
  } 
  return Tensor( std::vector<double>({1.} ),{0} ) ;
}

Tensor Particle::FinalStateSpinTensor() const {
  if( spin() == 0 ) return Tensor( std::vector<double>({1.}), std::vector<unsigned int>({1}) );
  else if( spin() == 1 ) {
    Tensor p = this->P();
    Expression pX = p.get(0);
    Expression pY = p.get(1);
    Expression pZ = p.get(2);
    Expression pE = p.get(3);
    auto m = Sqrt( dot(p,p) );
    Expression norm = 1 / ( m *Sqrt(2) ); 
    Tensor pol_plus = -norm * Tensor( {
        Complex(m+pX*pX/(pE+m),-pX*pY/(pE+m)),
        Complex(pY*pX/(pE+m), -pY*pY/(pE+m) - m),
        Complex(pZ*pX/(pE+m), -pZ*pY/(pE+m)),
        Complex(pX,-pY) }  ); 
    Tensor pol_zero = Tensor( {
        Complex(pX*pZ/(pE+m)/m,0.),
        Complex(pY*pZ/(pE+m)/m, 0.),
        Complex(1.+pZ*pZ/(pE+m)/m,0.),
        Complex(pZ/m,0.) } ); 
    Tensor pol_minus = norm * Tensor( {
        Complex(m+pX*pX/(pE+m),pX*pY/(pE+m)),
        Complex(pY*pX/(pE+m), m + pY*pY/(pE+m)),
        Complex(pZ*pX/(pE+m),pZ*pY/(pE+m)),
        Complex(pX,pY) } ); 
    return pol_plus + pol_zero + pol_minus;

  }  
  else return Tensor( std::vector<double>({1.}), std::vector<unsigned int>({0}) ) ; 
}

bool Particle::checkExists(){
  bool success = true;
  if( m_daughters.size() == 2 ){
    success &= VertexFactory::isVertex(TwoParticleHash()) ;
    if( !success ){
      ERROR( uniqueString() );
      ERROR( "Spin configuration not found J = " << spin() 
          << " L  =  "  << m_orbital 
          << " l' = " << m_spinConfigurationNumber 
          << " s1 = " << m_daughters[0]->spin() 
          << " s2 = " << m_daughters[1]->spin() 
          << " hash =   " << TwoParticleHash()  
          << " bits = " << std::bitset<20>(TwoParticleHash() ) );
    }
    success &= daughter(0)->checkExists() & daughter(1)->checkExists();

  }
  if( success == false ) 
    ERROR( uniqueString() << " is not described in IVertex" );
  return success;
}

std::pair<unsigned int , unsigned int> Particle::lRange(bool conserveParity) const {
  if( m_daughters.size() == 0 ) return {0,0};
  if( m_daughters.size() != 2 ){
    ERROR( "L not well defined for D == " << m_daughters.size() ) ; 
    return {999,998};
  }
  const int S = spin();
  const int s1 = daughter(0)->spin();
  const int s2 = daughter(1)->spin();

  unsigned int min = abs(S-s1-s2);
  if( abs(S+s1-s2) < min ) min = abs(S+s1-s2);
  if( abs(S-s1+s2) < min ) min = abs(S-s1+s2);
  const unsigned int max = S + s1 + s2;

  if( conserveParity == false ) return {min,max};
  unsigned int l = min;
  for(; l < max + 1; ++l ) if( conservesParity(l)) break;
  if( l == max + 1 ) return {999,998};
  std::pair<unsigned int, unsigned int> lLimit = {l,l};
  l=max;
  for(;l != min -1; --l ) if( conservesParity(l)) break;
  lLimit.second=l;
  return lLimit;
}

std::string Particle::getTeX(bool isRoot) const {

  std::string marker = isRoot ? "#" : "\\";
  const std::string leftBrace = isRoot ? "(" : "\\left["; 
  const std::string rightBrace = isRoot ? ")" : "\\right]";
  std::string name = m_name ;
  if( ! m_istop && m_conjThis ){ 
    name = m_props->anti().name();
  }
  if( m_daughters.size() != 0 ){
    std::string val = m_istop ? "" : m_props->texName() + leftBrace;
    if( m_istop && m_orbital != m_minL ) val =  leftBrace;
    for( unsigned int i = 0 ; i < m_daughters.size(); ++i  )
      val+= m_daughters[i]->getTeX(isRoot);
    val+= m_istop ? "" :  rightBrace;
    if( m_istop && m_orbital != m_minL ) val += rightBrace;
    if( m_orbital != m_minL ) val += "^{" + orbitalString() + "}";
    return val;
  }
  else return m_props->texName();
}

