#include "AmpGen/CompiledExpression.h"
#include "AmpGen/MinuitParameter.h"
#include "TMatrixD.h"
#include "TVectorD.h"


namespace AmpGen { 
  class CacheTransfer {
    protected: 
      CompiledExpression<std::complex<double>>* m_destination;
      unsigned int m_address;
      AmpGen::MinuitParameter* m_source;    
    public:
      virtual void transfer() { 
        m_destination->setExternal( m_source->mean(), m_address  );
      };
      CacheTransfer(){};
      CacheTransfer( AmpGen::MinuitParameter* source,
          CompiledExpression<std::complex<double>>* sink,
          const unsigned int& address ) :
        m_destination( sink ),
        m_address(address),
        m_source(source) {};

  };


  class SplineTransfer : public CacheTransfer {
    private:

      CompiledExpression<std::complex<double>>* m_destination;

      TMatrixD m_transferMatrix;
      std::vector<AmpGen::MinuitParameter*> m_y_parameters;

      double m_min;
      double m_max;
      unsigned int m_y_address;
      unsigned int m_f_address;

    public:
      SplineTransfer( const SplineTransfer& other ) : CacheTransfer(),
      m_transferMatrix( other.m_transferMatrix ),
      m_y_parameters( other.m_y_parameters ),
      m_min( other.m_min ),
      m_max( other.m_max ),
      m_y_address( other.m_y_address ),
      m_f_address( other.m_f_address ) {} 

      SplineTransfer() {};
      bool isConfigured(){
        for( auto& x : m_y_parameters) if( x == NULL ) return false;
        return true;
      };

      void set( const unsigned int & N, AmpGen::MinuitParameter* f ){ 
        // DEBUG("Setting param " << N << " to " << f->name() );
        m_y_parameters[N] = f; } 
      void setAddress( const unsigned int& address ){ m_y_address = (address); } 
      void setCurveAddress( const unsigned int& address){ m_f_address = (address); } 
      //   SplineTransfer( const SplineTransfer& other ) : 
      SplineTransfer(
          CompiledExpression<std::complex<double>>* sink,
          const unsigned int& N,
          const double& min,
          const double& max  ) :
        m_destination(sink ), 
        m_transferMatrix(TMatrixD(N-2,N-2)), 
        m_y_parameters( N , NULL ), m_min(min), m_max(max), m_y_address(0), m_f_address(0)  {

          unsigned int size = N - 2;
          TMatrixD M(N-2,N-2);
          for( unsigned int i = 0 ; i < size; ++i ){
            M[i][i] = 4;
            if( i != size -1 ){
              M[i][i+1] = 1;
              M[i+1][i] = 1;
            }
          }
          DEBUG( m_transferMatrix.GetNrows() << " x " << m_transferMatrix.GetNcols() << "  ; " << 
              M.GetNrows() << " x " << M.GetNcols() );

          m_transferMatrix = M.Invert();
          DEBUG("Leaving constructor ...");  
        }
      void transfer(){
        DEBUG("Here in transfer ... ");
        unsigned int size = m_y_parameters.size() -2 ;
        TVectorD L( size );
        for( unsigned int i = 0 ; i < size; ++i){
          L[i] = m_y_parameters[i+2]->mean() - 2*m_y_parameters[i+1]->mean() + m_y_parameters[i]->mean();
        }
        DEBUG( L.GetNrows() << "   " << m_transferMatrix.GetNrows() << "   " << m_transferMatrix.GetNcols() );
        auto mtv = m_transferMatrix*L;
        std::vector<double> mvectors( m_y_parameters.size() , 0 );
        double spacing = (m_max-m_min)/double(m_y_parameters.size()); 

        for( int i = 0 ; i  < mtv.GetNrows(); ++i)
          mvectors[i+1] =  6*mtv[i]/(spacing*spacing);

        for( unsigned int i = 0 ; i < m_y_parameters.size() ;++i){
          DEBUG("Transferring " << m_y_parameters[i]->mean() << ", " << mvectors[i] << " to " << m_y_address + i << " , " << m_f_address +i );
          m_destination->setExternal( m_y_parameters[i]->mean(), m_y_address + i );
          DEBUG("Setting other external...");
          m_destination->setExternal( mvectors[i], m_f_address + i );
        };
        DEBUG("Closing the transfer");
      }; 

  }; } 


/*
   static std::map< std::string, std::shared_ptr<SplineTransfer> > 
   getSplineParameterMapping( CompiledExpression<std::complex<double>>& sink, 
   const std::map<std::string, std::pair< unsigned int, double> >& splineParameters, const std::map<std::string, AmpGen::MinuitParameter*>& otherParameters ){

   std::map<std::string, std::shared_ptr<SplineTransfer> > splines; 

   for( auto param : splineParameters ){

   auto tokens = split( param.first , ':' );
   const std::string particleName = tokens[0];
   const std::string splineName = tokens[0] + "::"+tokens[1]+"::"+tokens[2];
   DEBUG("Spline parameter for " << param.first << " configuring");
   auto
   thisSpline = splines.find(splineName);
   if( thisSpline == splines.end() ){
   double min =
   AmpGen::NamedParameter<double>(particleName+"::Spline::Min",0.).getVal();
   double max =
   AmpGen::NamedParameter<double>(particleName+"::Spline::Max",1800*1800).getVal();
   unsigned int nBins =
   AmpGen::NamedParameter<unsigned int>(particleName+"::Spline::N",10).getVal();
   splines[splineName] =
   std::make_shared<SplineTransfer>( &sink, nBins , min, max ) ;
   thisSpline = splines.find( splineName );
   }
   auto it = otherParameters.find( param.first );
   unsigned int index = stoi((*tokens.rbegin()));
   if ( it != otherParameters.end() ){
   DEBUG(" -> to " << (*it).second <<"    " <<  param.first << "    " << (*it).second->mean() );
   thisSpline->second->set( index , (*it).second );
   if( index == 0 )
   thisSpline->second->setAddress( param.second.first );
   }

   else if( *(tokens.rbegin()+1) == "C"  ){
   if( index == 0 ) thisSpline->second->setCurveAddress( param.second.first );
   }
   else
   ERROR( param.first << " spline parameter not understood");
   }
   return splines; 
   }
   */
