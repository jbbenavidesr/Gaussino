C $Id: hwrgen.F,v 1.1 2006-09-05 12:36:49 karl Exp $

C-----------------------------------------------------------------------
C
C  Reimplementation of function HWRGEN from Herwig 6.510
C  for random-number generation
C
C  This version uses Gaudi RndmNumberSvc
C
C  Author: Karl Harrison
C
C-----------------------------------------------------------------------

      FUNCTION HWRGEN( IDUMMY )
      IMPLICIT DOUBLE PRECISION( A-H, O-Z )
      HWRGEN = CPYR( IDUMMY )
      RETURN
      END
