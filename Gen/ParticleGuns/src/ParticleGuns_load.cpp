// $Id: ParticleGuns_load.cpp,v 1.2 2006-07-04 12:15:51 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(ParticleGuns) {

  DECLARE_ALGORITHM( SingleParticleGun );
  DECLARE_ALGORITHM( MaterialEvalGun );
//   DECLARE_ALGORITHM( ParticleGun );
  
}
