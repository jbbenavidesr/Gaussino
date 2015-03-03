C $Id: hwusta.F,v 1.1 2006-09-05 12:36:49 karl Exp $

C-----------------------------------------------------------------------
C
C  Modified version of subroutine HWUSTA from Herwig 6.510
C  Disabled printing of information for particles set stable,
C  to have less verbose output
C
C  Modification: Karl Harrison
C
C-----------------------------------------------------------------------

CDECK  ID>, HWUSTA.
*CMZ :-        -26/04/91  10.18.58  by  Bryan Webber
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      SUBROUTINE HWUSTA(NAME)
C-----------------------------------------------------------------------
C     MAKES PARTICLE TYPE 'NAME' STABLE
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER IPDG,IWIG
      CHARACTER*8 NAME
      CALL HWUIDT(3,IPDG,IWIG,NAME)
      IF (IWIG.EQ.20) CALL HWWARN('HWUSTA',500)
      RSTAB(IWIG)=.TRUE.
C--KH modification: start
C     WRITE (6,10) IWIG,NAME
C--KH modification: end
   10 FORMAT(/10X,'PARTICLE TYPE',I4,'=',A8,' SET STABLE')
      END
