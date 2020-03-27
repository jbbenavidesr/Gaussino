#include "Event/MCVertex.h"
#include "GiGaMTCoreTruth/LinkedParticle.h"
#include "GaudiKernel/MsgStream.h"

namespace Gaussino {
  LHCb::MCVertex::MCVertexType vertexType( int id );
  LHCb::MCVertex::MCVertexType GetLinkedVertexType( LinkedVertex* lv, MsgStream* msgStream=nullptr);
}
