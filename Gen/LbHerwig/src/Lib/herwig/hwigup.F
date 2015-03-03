C $Id: hwigup.F,v 1.1 2006-09-05 12:36:49 karl Exp $

C-----------------------------------------------------------------------
C
C  Modified version of subroutine HWIGUP from Herwig 6.510
C
C  User routine called from HWIGUP changed from UPINIT to HWUPINIT
C  to avoid clash with routine of same name in LbPythia package
C
C  Modification: Karl Harrison
C
C-----------------------------------------------------------------------

CDECK  ID>, HWIGUP.
*CMZ :-        -15/07/02  16.42.23  by  Peter Richardson
*-- Author :    Peter Richardson
C----------------------------------------------------------------------
      SUBROUTINE HWIGUP
C----------------------------------------------------------------------
C     Use the GUPI (Generic User Process Interface) run common block
C     to initialise HERWIG
C----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
      CHARACTER *8 DUMMY,PDFNUC(9),PDFPI(9),PDFPHT(9)
      INTEGER I,IDB(2)
      SAVE PDFNUC,PDFPI ,PDFPHT
      DATA PDFNUC/ 'DO','DFLM','MRS','CTEQ','GRV','ABFOW','BM',
     &             '        ','         '/
      DATA PDFPI / 'OW-P','        ','SMRS-P','        ','GRV-P',
     &             'ABFKW-P','        ','        ','        '/
      DATA PDFPHT /'DO-G','DG-G','LAC-G','GS-G','GRV-G','ACG-G',
     &             '         ','WHIT-G','SaSph'/
C--call the user routine to do the initialisation
C--KH modification: start
C--Name of user routine changed from UPINIT to HWUPINIT
C--to avoid clash with routine of same name in LbPythia package
C--   CALL UPINIT
      CALL HWUPINIT
C--KH modification: end
C--setup the beam particles and momentum
      CALL HWUIDT(1,IDBMUP(1),IDB(1),DUMMY)
      PART1=DUMMY
      CALL HWUIDT(1,IDBMUP(2),IDB(2),DUMMY)
      PART2=DUMMY
      PBEAM1 = SQRT(EBMUP(1)**2-RMASS(IDB(1))**2)
      PBEAM2 = SQRT(EBMUP(2)**2-RMASS(IDB(2))**2)
C--set up for PDFLIB if need
      DO I=1,2
        IF(PDFGUP(I).NE.-1) THEN
          IF(PDFGUP(I).LT.1.OR.PDFGUP(I).GT.9) CALL HWWARN('HWIGUP',500)
          MODPDF(I) = PDFSUP(I)
C--proton/neutron beams
          IF(ABS(IDBMUP(I)).EQ.2212.OR.ABS(IDBMUP(I)).EQ.2112) THEN
            AUTPDF(I) = PDFNUC(PDFGUP(I))
C--photon beams
          ELSEIF(ABS(IDBMUP(I)).EQ.22) THEN
            AUTPDF(I) = PDFPHT(PDFGUP(I))
C--pion beams
          ELSEIF(ABS(IDBMUP(I)).EQ.211) THEN
            AUTPDF(I) = PDFPI(PDFGUP(I))
C--unknown beam type
          ELSE
            CALL HWWARN('HWIGUP',500)
          ENDIF
        ENDIF
      ENDDO
C--decide what to do about the weights
      IF(ABS(IDWTUP).EQ.1) THEN
        WGTMAX = ZERO
        AVWGT  = ONE
        AVABW  = ONE
        NOWGT  = .TRUE.
C--sum up the magnitudes of the maximum weight
        LHMXSM = ZERO
        DO I=1,NPRUP
          LHXMAX(I) = XMAXUP(I)*1.0D-3
          LHMXSM    = LHMXSM+ABS(LHXMAX(I))
        ENDDO
        ITYPLH = 0
      ELSEIF(ABS(IDWTUP).EQ.2) THEN
        WGTMAX = ZERO
        AVWGT  = ONE
        AVABW  = ONE
        NOWGT = .TRUE.
C--sum the cross sections and obtain the total
        LHMXSM = ZERO
        DO I=1,NPRUP
          LHXSCT(I) = XSECUP(I)*1.0D-3
          LHXMAX(I) = XMAXUP(I)*1.0D-3
          LHMXSM = LHMXSM+ABS(LHXSCT(I))
        ENDDO
        ITYPLH = 0
      ELSEIF(ABS(IDWTUP).EQ.3) THEN
        WGTMAX = ONE
        AVWGT  = ONE
        AVABW  = ONE
        NOWGT = .TRUE.
      ELSEIF(ABS(IDWTUP).EQ.4) THEN
        WGTMAX = ONE
        AVWGT  = ONE
        NOWGT = .FALSE.
      ENDIF
      IF(IDWTUP.LT.0) NEGWTS = .TRUE.
C--zero the weight
      DO I=1,NPRUP
        LHWGT (I) = ZERO
        LHWGTS(I) = ZERO
        LHIWGT(I) = 0
        LHNEVT(I) = 0
      ENDDO
      END
