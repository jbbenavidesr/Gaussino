c-----------------------------------------------------------------------
      function rangen()
c-----------------------------------------------------------------------
c     generates a random number
c-----------------------------------------------------------------------
      REAL rangen 
      double precision cpyr
      INTEGER IDUMMY

      IDUMMY = 0
      rangen=REAL( cpyr(IDUMMY) )

      return
      end

C-----------
      function drangen( dummy )
      double precision drangen, dummy, cpyr
      integer IDUMMY
      IDUMMY = 0
      
      IDUMMY = 0
      drangen = cpyr( IDUMMY )
      return
      end
