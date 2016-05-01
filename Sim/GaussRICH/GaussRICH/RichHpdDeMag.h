#ifndef RichHpdDeMag_h
#define RichHpdDeMag_h 1

#include <vector>
#include "G4ThreeVector.hh"
#include "RichDet/Rich1DTabFunc.h"

// class for the hpd demagnification factor.

class RichHpdDeMag {

 public:

  RichHpdDeMag();
  virtual ~RichHpdDeMag();

  RichHpdDeMag(IDataProviderSvc* , int hpdnumb, int richdetnumb);

  double richHpdDemagLinearTerm()    const {return m_richHpdDemagLinearTerm; }
  double richHpdDemagQuadraticTerm() const {return  m_richHpdDemagQuadraticTerm;}
  std::vector<double> HpdDemagVect() const; 

  int curHpdNumb()     {return m_curHpdNumb; }
  int curRichDetNumb() {return m_curRichDetNumb; }
  void setCurrentHPDDemag(const std::vector<double> & demagVect);
  //following line modified to be compatible with recent change where a typedef is
  // removed for the RichDet method used here . Now using the full name.  01-05-2016 SE
  //  void setCurrentDemagnification(const Rich1DTabFunc*, const Rich1DTabFunc* );
  void setCurrentDemagnification(const Rich::TabulatedFunction1D *, const Rich::TabulatedFunction1D * );
  G4ThreeVector getPositionOnAnode( double , double ) ;

 private:

  int m_curHpdNumb;
  int m_curRichDetNumb;
  double m_richHpdDemagLinearTerm;
  double m_richHpdDemagQuadraticTerm;

  double m_RichHpdQWToSiMaxDist, m_Rcurv, m_activeRadius;

  //  const Rich1DTabFunc *m_Rdemag, *m_Phidemag;
  const Rich::TabulatedFunction1D *m_Rdemag, *m_Phidemag;

};
#endif
