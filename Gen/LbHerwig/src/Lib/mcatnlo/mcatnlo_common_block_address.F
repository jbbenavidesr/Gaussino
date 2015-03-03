C $Id: mcatnlo_common_block_address.F,v 1.1 2006-09-05 12:36:49 karl Exp $

C-----------------------------------------------------------------------
C
C  Function that returns address of MC@NLO common blocki
C  specified by name
C  Based on analogous function for Herwig
C
C  Author: Karl Harrison
C
C-----------------------------------------------------------------------

C-----------------------------------------------------------------------
      FUNCTION MCATNLO_COMMON_BLOCK_ADDRESS( COMMON_BLOCK_NAME )
C-----------------------------------------------------------------------
      include 'mcatnlo_qq.inc'
      CHARACTER*50 QQIN
      COMMON/VVJIN/QQIN
      CHARACTER*(*) COMMON_BLOCK_NAME
      INTEGER MCATNLO_COMMON_BLOCK_ADDRESS
C
C-----------------------------------------------------------------------
      INTEGER ADRESSI
      INTEGER ADRESSF
      INTEGER ADRESSC
      INTEGER ADRESCX
      INTEGER COMMON_BLOCK_ADDRESS
C-----------------------------------------------------------------------
      COMMON_BLOCK_ADDRESS = 0
C-----------------------------------------------------------------------
      IF (COMMON_BLOCK_NAME.EQ.'CETACUT') THEN
        MCATNLO_COMMON_BLOCK_ADDRESS = ADRESSF( ETACUT )

      ELSE IF (COMMON_BLOCK_NAME.EQ.'CIWGTNORM') THEN
        MCATNLO_COMMON_BLOCK_ADDRESS = ADRESSI( IWGTNORM )

      ELSE IF (COMMON_BLOCK_NAME.EQ.'FIXVAR') THEN
        MCATNLO_COMMON_BLOCK_ADDRESS = ADRESSF( XM2 )

      ELSE IF (COMMON_BLOCK_NAME.EQ.'MCNLOPAR') THEN
        MCATNLO_COMMON_BLOCK_ADDRESS = ADRESSF( AC1 )

      ELSE IF (COMMON_BLOCK_NAME.EQ.'PARMASS') THEN
        MCATNLO_COMMON_BLOCK_ADDRESS = ADRESSF( XMASS( -5 ) )

      ELSE IF (COMMON_BLOCK_NAME.EQ.'VVJIN') THEN
        MCATNLO_COMMON_BLOCK_ADDRESS = ADRESSC( QQIN )

      ELSE  
        WRITE(6,*) " Unknown common-block name:  " , COMMON_BLOCK_NAME 

      ENDIF
 
      RETURN
      END
