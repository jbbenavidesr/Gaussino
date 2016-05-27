#ifndef GAUSSRICH_RICHSCINTILPARAMADMIN_H 
#define GAUSSRICH_RICHSCINTILPARAMADMIN_H 1

// Include files
#include <vector>
#include <string>

/** @class RichScintilParamAdmin RichScintilParamAdmin.h GaussRICH/RichScintilParamAdmin.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2016-05-26
 */
class RichScintilParamAdmin {
public: 

  virtual ~RichScintilParamAdmin( ); ///< Destructor



  void setRichScintilScaleFactor( double aFactor) {
    m_RichScintilScaleFactor = aFactor; 
    setRichScintilEffectiveFraction();
  }

  double RichScintilEffectiveFraction() 
  {  return m_RichScintilEffectiveFraction; }
  

  void setRichScintilEffectiveFraction();

  void setRichScintilRawFraction( double aFrac){
    m_RichScintilRawFraction= aFrac;
    setRichScintilEffectiveFraction();
  }
  void setRichScintilAuxScaleFactor( double aAuxS){ 
    m_RichScintilAuxScaleFactor =  aAuxS ;
    setRichScintilEffectiveFraction();
  }
  double RichScintilAuxScaleFactor(){
    return m_RichScintilAuxScaleFactor;
  }
  double RichScintilScaleFactor() 
  {return  m_RichScintilScaleFactor; }
  
    

  static  RichScintilParamAdmin*  getRichScintilParamAdminInstance();


protected:

private:
 
  /// Standard constructor  kept private
  RichScintilParamAdmin( ); 
  static  RichScintilParamAdmin*   RichScintilParamAdminInstance;



  double m_RichScintilScaleFactor;
  double m_RichScintilRawFraction;
  double m_RichScintilEffectiveFraction;
  double m_RichScintilAuxScaleFactor;
  
  

};
#endif // GAUSSRICH_RICHSCINTILPARAMADMIN_H
