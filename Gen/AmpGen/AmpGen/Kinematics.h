#include "AmpGen/EventList.h"
#include <array>
#include "TLorentzVector.h"

namespace AmpGen { 
  inline double Q2_Double( double s, double s1, double s2 ){
    return s/4. - ( s1 + s2)/2. + ( s1 - s2)*( s1 - s2) / (4*s);
  }

  TLorentzVector pFromEvent( const Event& evt, const unsigned int& ref){
    return TLorentzVector( evt.address( 4*ref ) );
  }

  double acoplanarity( const Event& evt ){

    TLorentzVector p0 = pFromEvent( evt, 0 ) ;
    TLorentzVector p1 = pFromEvent( evt, 1 ) ; 
    TLorentzVector p2 = pFromEvent( evt, 2 ) ;
    TLorentzVector p3 = pFromEvent( evt, 3 ) ;
    TLorentzVector pD = p0 + p1 + p2 + p3 ; 
    p0.Boost( - pD.BoostVector() );
    p1.Boost( - pD.BoostVector() );
    p2.Boost( - pD.BoostVector() );
    p3.Boost( - pD.BoostVector() );
    TVector3 t1 = ( p0.Vect().Cross( p1.Vect() ) ).Unit();
    TVector3 t2 = ( p2.Vect().Cross( p3.Vect() ) ).Unit();
    return acos( t1.Dot( t2 ) ) ;
  }

  inline double Product( const TLorentzVector& p1, const TLorentzVector& p2, const TLorentzVector& pX){
    return - p1.Dot( p2 ) + p1.Dot( pX ) * p2.Dot( pX ) /  pX.Dot(pX ) ;
  }

  inline double threeProduct( const std::vector<double>& a, const std::vector<double>& b ){
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
  } 

  struct MomentumTransfer {
    std::vector<unsigned int> p1;
    std::vector<unsigned int> p2;
    std::vector<unsigned int> s;

    MomentumTransfer( const std::vector<unsigned int>& _p1,
        const std::vector<unsigned int>& _p2 ) : p1(_p1), p2(_p2) 
    {
      for( auto& p : p1 ) s.push_back( p);
      for( auto& p : p2 ) s.push_back( p);
    }
    double operator()( const Event& evt ) const {
      double s0 = evt.s( s );
      double s1 = evt.s( p1 );
      double s2 = evt.s( p2 );
      return sqrt( Q2_Double( s0,s1,s2) )/1000.;
    }
  };

  struct HelicityCosine {
    unsigned int _i, _j;
    std::vector<unsigned int> _pR;
    HelicityCosine( const unsigned int& p1, 
        const unsigned int& p2, 
        const std::vector<unsigned int>& pR ) : _i(p1), _j(p2), _pR(pR) {}

    double operator() ( __gnu_cxx::__normal_iterator<const Event*, std::vector<Event, std::allocator<Event> > >& evt ) const { return (*this)(*evt);} 
    double operator() ( std::vector<Event>::iterator evt ) const { return (*this)(*evt); } 
    double operator() ( const Event& evt ) const {
      TLorentzVector PR;
      for( auto& i : _pR ) PR += pFromEvent( evt , i ) ;
      TLorentzVector pi = pFromEvent(evt, _i );
      TLorentzVector pj = pFromEvent(evt, _j );
      return Product( pi, pj, PR ) / sqrt( Product( pi,pi,PR) * Product( pj,pj,PR));
    }
  };

  double trihedralAngle( const Event& evt ){

    TLorentzVector p0 = pFromEvent( evt, 0 ) ;
    TLorentzVector p1 = pFromEvent( evt, 1 ) ;
    TLorentzVector p2 = pFromEvent( evt, 2 ) ;
    TLorentzVector p3 = pFromEvent( evt, 3 ) ;
    TLorentzVector pD = p0 + p1 + p2 + p3 ;
    p0.Boost( - pD.BoostVector() );
    p1.Boost( - pD.BoostVector() );
    p2.Boost( - pD.BoostVector() );
    p3.Boost( - pD.BoostVector() );
    return p0.Vect().Dot( p1.Vect().Cross( p2.Vect() ) )* ( 1./ (
          p1.Vect().Mag() * p0.Vect().Mag() * p2.Vect().Mag() ) );
  }

  double TripleProduct( const Event& evt ){

    TLorentzVector p0 = pFromEvent( evt, 0 ) ;
    TLorentzVector p1 = pFromEvent( evt, 1 ) ;
    TLorentzVector p2 = pFromEvent( evt, 2 ) ;
    TLorentzVector p3 = pFromEvent( evt, 3 ) ;
    TLorentzVector pD = p0 + p1 + p2 + p3 ;
    p0.Boost( - pD.BoostVector() );
    p1.Boost( - pD.BoostVector() );
    p2.Boost( - pD.BoostVector() );

    TVector3 P2 = p2.Vect().Unit();
    TVector3 DecayPlane =  p0.Vect().Cross(p1.Vect()) ;
    TVector3 f = DecayPlane.Unit();
    return P2.Dot(f);
  }

  /// boost event input along direction n 
  std::vector<double> boost( const std::vector<double>& input, const std::vector<double>& n, const double& v ){

    if( v > 1 ) ERROR(" Can't boost > c !");
    double gamma = 1. / sqrt( 1 - v*v );
    std::vector<double> returnValue(4,0);
    double norm = sqrt( n[0] * n[0] + n[1]*n[1] + n[2]*n[2] );

    returnValue[3] = gamma*( input[3] - v*threeProduct( input, n )/norm );
    for( unsigned int i = 0 ; i < 3 ; ++ i )
      returnValue[i] = input[i] + ( (gamma -1 )* threeProduct( input, n) /norm  - gamma*input[3]*v) * n[i] / norm;
    return returnValue; 
  }

  std::vector<double> rotate( const std::vector<double>& input, const std::vector<double>& n, const double& v ){
    std::vector<double> returnValue(4,0);
    double norm =sqrt( n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    returnValue[0] = cos(v)*input[0] + sin(v)*( input[2]*n[1] - input[1]*n[2] )/norm + (1 - cos(v))*threeProduct(input,n)*n[0] /(  norm*norm ) ;
    returnValue[1] = cos(v)*input[1] + sin(v)*( input[0]*n[2] - input[2]*n[0] )/norm + (1 - cos(v))*threeProduct(input,n)*n[1] / ( norm*norm ) ;
    returnValue[2] = cos(v)*input[2] + sin(v)*( input[1]*n[0] - input[0]*n[1] )/norm + (1 - cos(v))*threeProduct(input,n)*n[2] / ( norm*norm );
    returnValue[3] = input[3];
    return returnValue; 
  }
}
