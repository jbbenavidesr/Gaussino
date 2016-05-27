// Include files 



// local
#include "GaussRICH/RichScintilParamAdmin.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichScintilParamAdmin
//
// 2016-05-26 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

RichScintilParamAdmin* RichScintilParamAdmin::RichScintilParamAdminInstance=0;

RichScintilParamAdmin::RichScintilParamAdmin()
  :m_RichScintilScaleFactor(0.285066),
   m_RichScintilRawFraction(0.6),
   m_RichScintilEffectiveFraction(0.17104),
   m_RichScintilAuxScaleFactor(1.0)  { }

//=============================================================================
// Destructor
//=============================================================================
RichScintilParamAdmin::~RichScintilParamAdmin() {} 

//=============================================================================



void RichScintilParamAdmin::setRichScintilEffectiveFraction() {
  
  m_RichScintilEffectiveFraction = 
         m_RichScintilRawFraction * m_RichScintilScaleFactor * m_RichScintilAuxScaleFactor ;

}



RichScintilParamAdmin* RichScintilParamAdmin::getRichScintilParamAdminInstance(){

  if(RichScintilParamAdminInstance == 0 ) {
    RichScintilParamAdminInstance  = new RichScintilParamAdmin();
    

  }
  return RichScintilParamAdminInstance;
}
