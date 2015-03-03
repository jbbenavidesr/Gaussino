C $Id: jmwjmx.F,v 1.1 2006-09-05 12:36:49 karl Exp $

C-----------------------------------------------------------------------
C
C Subroutine JMWJMX
C Code adapted from section of subroutine HWHSCT from Herwig 6.510,
C to allow maximum weight to be caltulated at initialisation time
C
C Author: Karl Harrison
C 
C-----------------------------------------------------------------------

C-----------------------------------------------------------------------
      SUBROUTINE JMWJMX
C-----------------------------------------------------------------------
#include "HERWIG65.INC"
#include "jimmy.inc"
      DOUBLE PRECISION WGT,WJMAX
      COMMON/JMWGTMX/WJMAX
      INTEGER I
C---PERFORM A SEARCH FOR THE MAXIMUM WEIGHT
      WJMAX=0
      GENEV=.FALSE.
      DO I=1,IBSH
        CALL HWHSCU(WGT,PTJIM)
        WJMAX=MAX(WJMAX,WGT)
      ENDDO
      WRITE (6,'(A,G12.4)') ' Jimmy search for maximum weight=',WJMAX
C---BECAUSE OF THE ENERGY DEPENDENCE, LEAVE LOTS OF SAFETY MARGIN
      WJMAX=WJMAX*2
      RETURN
      END
