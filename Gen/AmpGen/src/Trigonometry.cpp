#include "AmpGen/Trigonometry.h"

#include <complex>

using namespace AmpGen;

Expression Cos::d( const Parameter& div){
  return Constant(-1)*Sin( m_expression ) * m_expression.d(div);
} 

Expression Sin::d( const Parameter& div){
  return Cos( m_expression ) * m_expression.d(div);
}

Expression Tan::d( const Parameter& div){
  return m_expression.d(div) / ( Cos( m_expression ) * Cos(m_expression) );
}

Expression aCos::d( const Parameter& div){
  return m_expression.d(div) / Sqrt( 1 - m_expression * m_expression );
}
Expression aSin::d( const Parameter& div){
  return - m_expression.d(div) / Sqrt( 1 - m_expression * m_expression );
}
Expression aTan::d( const Parameter& div){
  return m_expression.d( div )  / ( 1 + m_expression * m_expression );
}

