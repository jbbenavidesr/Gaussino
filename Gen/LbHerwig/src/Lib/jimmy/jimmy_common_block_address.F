C $Id: jimmy_common_block_address.F,v 1.1 2006-09-05 12:36:49 karl Exp $

C-----------------------------------------------------------------------
C
C  Function that returns address of Jimmy common blocki
C  specified by name
C  Based on analogous function for Herwig
C
C  Author: Karl Harrison
C
C-----------------------------------------------------------------------

C-----------------------------------------------------------------------
      FUNCTION JIMMY_COMMON_BLOCK_ADDRESS( COMMON_BLOCK_NAME )
C-----------------------------------------------------------------------
#include "jimmy.inc"
      CHARACTER*(*) COMMON_BLOCK_NAME
      INTEGER JIMMY_COMMON_BLOCK_ADDRESS
C
C-----------------------------------------------------------------------
      INTEGER ADRESSI
      INTEGER ADRESSF
      INTEGER ADRESSC
      INTEGER ADRESCX
      INTEGER COMMON_BLOCK_ADDRESS
C-----------------------------------------------------------------------
      COMMON_BLOCK_ADDRESS = 0
c-----------------------------------------------------------------------

      IF (COMMON_BLOCK_NAME.EQ.'JMPARM') THEN
        JIMMY_COMMON_BLOCK_ADDRESS = ADRESSF( PTJIM )
      ELSE  
        WRITE(6,*) "Unknown common-block name:  ", COMMON_BLOCK_NAME 
      ENDIF
 
      RETURN
      END
