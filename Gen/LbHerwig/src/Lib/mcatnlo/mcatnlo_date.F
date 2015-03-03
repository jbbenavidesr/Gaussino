C $Id: mcatnlo_date.F,v 1.1 2007-05-07 14:01:59 karl Exp $

C-----------------------------------------------------------------------
C
C  This file contains rewrites of the subroutines BSDATE and BSTIME
C  implemented in mcatnlo_date.f from MC@NLO 3.2
C
C  Author: Karl Harrison
C
C-----------------------------------------------------------------------

C***********************************************************************
C*=======================                                              *
       SUBROUTINE BSDATE
C*=======================                                              *
C* Subroutine to extract information for time stamp
C*
C* When the subroutine returns, the vectors IDATE and ITIME
C* in the common block /BDATE/ contain the following information:
C*
C*    IDATE(1) : year
C*    IDATE(2) : month
C*    IDATE(3) : day
C*
C*    ITIME(1) : hour
C*    ITIME(2) : minute
C*
C***********************************************************************
       COMMON /BDATE/ IDATE(3), ITIME(2)
       CHARACTER*8 DATE
       CHARACTER*10 TIME
       CHARACTER*5 ZONE
       INTEGER TVALUE(8)
       CALL DATE_AND_TIME( DATE, TIME, ZONE, TVALUE ) 
       IDATE(1) = MOD( TVALUE(1), 100 )
       IDATE(2) = TVALUE(2)
       IDATE(3) = TVALUE(3)
       ITIME(1) = TVALUE(5)
       ITIME(2) = TVALUE(6)
       RETURN
       END

C***********************************************************************
C*=======================                                              *
      SUBROUTINE BSTIME( TIME, IFLG )
C*=======================                                              *
C* Subroutine to determine elapsed CPU time
C*
C* Input:
C*    IFLG : Flag
C*    IFLG = 0 : Initialise timing
C*    IFLG = 1 : Determine elapsed CPU time
C*
C* Output:
C*    TIME : Elapsed CPU time in seconds
C*
C***********************************************************************
      REAL TIME0, TIME1
      SAVE TIME0
      IF ( IFLG .NE. 0 ) THEN
         CALL CPU_TIME( TIME1 )
         TIME = TIME1 - TIME0
      ELSE
         CALL CPU_TIME( TIME0 )
         TIME=0.
      ENDIF
      RETURN 
      END
