// $Id: epos.h, 2015-08-28 13:49:00 Laure Massacrier Exp $
#ifndef LBCRMC_EPOS_H 
#define LBCRMC_EPOS_H 1
// =====================================================================
// Include files
// =====================================================================
// STD & STL 
// =====================================================================
#include <string>
// =====================================================================
// LbCRMC 
// =====================================================================
#include "LbCRMC/Cevt.h"
#include "LbCRMC/C2evt.h"
#include "LbCRMC/Cptl.h"
#include "LbCRMC/Hadr5.h"
#include "LbCRMC/Accum.h"
#include "LbCRMC/Nucl1.h"
#include "LbCRMC/Othe1.h"
#include "LbCRMC/Appli.h"
#include "LbCRMC/Drop7.h"
#include "LbCRMC/Hadr25.h"

// =====================================================================

/** @class Epos epos.h LbCRMC/epos.h
 * 
 *  Utility class to access epos Fortran Functions 
 *
 *  @author Laure Massacrier
 *  @date   2015-08-28
 */
class Epos {
public:
  /// Access CEVT common block
  static Cevt & cevt() { return s_cevt ; }
  /// Access C2EVT common block
  static C2evt & c2evt() { return s_c2evt ; }
  /// Access CPTL common block
  static Cptl & cptl() { return s_cptl ; }
  /// Access HADR5 common block
  static Hadr5 & hadr5() { return s_hadr5 ; }
   /// Access ACCUM common block
  static Accum & accum() { return s_accum ; }
  /// Access NUCL1 common block
  static Nucl1 & nucl1() { return s_nucl1 ; }
  /// Access OTHE1 common block
  static Othe1 & othe1() { return s_othe1 ; }
  /// Access APPLI common block
  static Appli & appli() { return s_appli ; }
  /// Access DROP7 common block
  static Drop7 & drop7() { return s_drop7 ; }
    /// Access HADR25 common block
  static Hadr25 & hadr25() { return s_hadr25 ; }

protected:

private:
    static Cevt s_cevt ; ///< CEVT Common Block data
    static C2evt s_c2evt ; ///< C2EVT Common Block data
    static Cptl s_cptl; ///< CPTL Common Block data
    static Hadr5 s_hadr5; ///< HADR5 Common Block data
    static Accum s_accum; ///< ACCUM Common Block data
    static Nucl1 s_nucl1; ///< NUCL1 Common Block data
    static Othe1 s_othe1; ///< OTHE1 Common Block data
    static Appli s_appli; ///< APPLI Common Block data
    static Drop7 s_drop7; ///< DROP7 Common Block data
    static Hadr25 s_hadr25; ///< HADR25 Common Block data

};
#endif // LBCRMC_HADR25_H
