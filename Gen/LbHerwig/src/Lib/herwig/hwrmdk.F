C $Id: hwrmdk.F,v 1.1 2007-04-25 12:45:14 karl Exp $

C-----------------------------------------------------------------------
C
C  Subroutine that removes from the event record the decays of particles
C  known to the decay tool
C
C  Particles are allowed to decay first in Herwig to obtain the right
C  multiplicities in Herwig's model for soft underlying event
C
C  Author: Karl Harrison
C
C-----------------------------------------------------------------------

      SUBROUTINE HWRMDK
#include "HERWIG65.INC"
      LOGICAL DKTL
      COMMON/HWDKTL/DKTL(0:NMXRES)
      INTEGER IDHERWIG,IHEP,IMAP(0:NMXHEP),J,NKEEP

C--Flag particles to be removed from event record
      DO 10 IHEP=1,NHEP
        IF (ISTHEP(IHEP).GE.190) THEN
C--Set status code to 1 if particle is known to decay tool
          IDHERWIG=-1
   20     CONTINUE
          IDHERWIG=IDHERWIG+1
          IF (IDHERWIG.GT.NMXRES) GOTO 30
          IF (IDHEP(IHEP).NE.IDPDG(IDHERWIG)) GOTO 20
            IF (DKTL(IDHERWIG)) ISTHEP(IHEP)=1
   30     CONTINUE
        ENDIF
C--Set status code to -1 for particles to be removed from event record
        IF (ISTHEP(IHEP).LE.1) THEN
          DO 40 J=MIN(JDAHEP(1,IHEP),JDAHEP(2,IHEP)),
     +      MAX(JDAHEP(1,IHEP),JDAHEP(2,IHEP))
            IF (J.GT.0.AND.J.LE.NMXHEP) THEN
              ISTHEP(J)=-1
            ENDIF
   40     CONTINUE 
C--Cast off daughters
          DO 45 J=1,2
            JDAHEP(J,IHEP)=0
   45     CONTINUE
        ENDIF
   10 CONTINUE

C--Update event record
      NKEEP=0
      IMAP(0)=0
      DO 50 IHEP=1,NHEP
        IF (ISTHEP(IHEP).GT.0) THEN
          NKEEP=NKEEP+1
          IMAP(IHEP)=NKEEP
          IF (IHEP.GT.NKEEP) THEN
            ISTHEP(NKEEP)=ISTHEP(IHEP)
            IDHEP(NKEEP)=IDHEP(IHEP)
            DO 60 J=1,5
              IF (J.LE.2) THEN
                JMOHEP(J,NKEEP)=JMOHEP(J,IHEP)
                JDAHEP(J,NKEEP)=JDAHEP(J,IHEP)
              ENDIF
              IF (J.LE.4) VHEP(J,NKEEP)=VHEP(J,IHEP)
              PHEP(J,NKEEP)=PHEP(J,IHEP)
   60       CONTINUE
          ENDIF
        ELSE
          IMAP(IHEP)=-1
        ENDIF
   50 CONTINUE
      NHEP=NKEEP

C--Update pointers to mothers and daughters
      DO 70 IHEP=1,NHEP
        DO 80 J=1,2
          IF (IMAP(JMOHEP(J,IHEP)).GE.0) THEN
            JMOHEP(J,IHEP)=IMAP(JMOHEP(J,IHEP))
          ELSE
            WRITE(6,101) IHEP,J
            WRITE(6,102)
            CALL HWWARN('HWRMDK',99)
          ENDIF
          IF (IMAP(JDAHEP(J,IHEP)).GE.0) THEN
            JDAHEP(J,IHEP)=IMAP(JDAHEP(J,IHEP))
          ELSE
            WRITE(6,103) IHEP,J
            WRITE(6,102)
            CALL HWWARN('HWRMDK',99)
          ENDIF
   80   CONTINUE
   70 CONTINUE
  101 FORMAT(1X,'HWRMDK: Particle ',I5,' - Mother ',I1)
  102 FORMAT(1X,'HWRMDK: Problems setting pointer')
  103 FORMAT(1X,'HWRMDK: Particle ',I5,' - Daughter ',I1)

      RETURN
      END


