// Local.
#include "MCInterfaces/IFlagSignalChain.h"

// Gaudi.
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Kernel.
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include "range/v3/all.hpp"

#include "Defaults/HepMCAttributes.h"
#include "HepMCUser/Status.h"
#include "HepMCUser/VertexAttribute.h"

#include "MCTruthToEDM/VertexType.h"

#include<set>

// Gaudi.
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/Transformer.h"
#include "GaudiKernel/Vector4DTypes.h"

// Event.
#include "Event/MCHeader.h"
#include "Event/GenHeader.h"

// Event.
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "MCTruthToEDM/LinkedParticleMCParticleLink.h"

#include "GiGaMTCoreTruth/MCTruthConverter.h"

#include "Defaults/Locations.h"
#include "GiGaMTReDecay/typedefs.h"

/** @class MCTruthToEDM MCTruthToEDM.h
 *  Algorithm to move create the MCParticle and MCVertex structure
 *  from the filled MCTruth objects. Based loosely on the
 *  MCTruthToEDM algorithm in Gauss
 *
 *  Also returns an additional structure which maps LinkedParticle to the
 *  converted MCParticle. This will be useful for later assigning the MCParticle
 *  to the MCHit via the LinkedParticle stored in the G4VHit
 *
 *  @author Dominik Muller
 *  @date 2018-04-09
 */

class ReDecayMCTruthToEDM : public Gaudi::Functional::MultiTransformer<
                         std::tuple<LHCb::MCParticles, LHCb::MCVertices, LHCb::MCHeader, LinkedParticleMCParticleLinks>(
                             const Gaussino::MCTruthPtrs&, const LHCb::GenHeader &, const Gaussino::ReDecay::SignalTruths & )>
{
public:
  /// Standard constructor.
  ReDecayMCTruthToEDM( const std::string& name, ISvcLocator* pSvcLocator )
      : MultiTransformer(
            name, pSvcLocator, {{KeyValue{"MCTruthLocation", Gaussino::MCTruthsLocation::Default},
                KeyValue{"GenHeaderLocation", Gaussino::GenHeaderLocation::Default},
                KeyValue{"OutputSignalTruths", Gaussino::MCTruthsLocation::SignalTruthsMap}}},
            {{
                KeyValue{"Particles", Gaussino::MCParticleLocation::Default},
                KeyValue{"Vertices", Gaussino::MCVertexLocation::Default},
                KeyValue{"MCHeader", LHCb::MCHeaderLocation::Default},
                KeyValue{"LinkedParticleMCParticleLinks", Gaussino::LinkedParticleMCParticleLinksLocation::Default},
            }} ){};
  virtual ~ReDecayMCTruthToEDM() = default; ///< Destructor.
  virtual std::tuple<LHCb::MCParticles, LHCb::MCVertices, LHCb::MCHeader, LinkedParticleMCParticleLinks>
  operator()( const Gaussino::MCTruthPtrs&, const LHCb::GenHeader&, const Gaussino::ReDecay::SignalTruths &  ) const override;

private:
  typedef std::set<std::pair<LHCb::MCVertex*, const HepMC3::GenVertex*>> VertexSet;
  /// Determine the primary vertex of the interaction.
  LHCb::MCVertex* FindPV( LinkedParticle* lp, VertexSet& pvs ) const;

  /// Convert a GenParticle either into a MCParticle or G4PrimaryParticle.
  class Converter
  {
  public:
    Converter( LHCb::MCParticles& mcparticles, LHCb::MCVertices& mcvertices, LinkedParticleMCParticleLinks& lpmcp_links,
               MsgStream& stm, const Gaussino::ReDecay::SignalTruths & struths)
        : msgStream( stm ), m_particles( mcparticles ), m_vertices( mcvertices ), m_links( lpmcp_links ), m_signal_truths(struths)
    {
    }
    void convert( LinkedParticle* particle, LHCb::MCVertex* originVertex );

    LHCb::MCParticle* makeMCParticle( LinkedParticle* particle );
    // Simple helper to create a vertex at the location and add
    // it to the containers
    LHCb::MCVertex* createVertex( const HepMC3::FourVector& fm );
    // Converts a LinkedVertex to MCVertex. Figures out a sensible
    // vertex type and sets it based on the MCTruth structure.
    LHCb::MCVertex* createVertex( LinkedVertex* lv );

  private:
    MsgStream& msgStream;
    LHCb::MCParticles& m_particles;
    LHCb::MCVertices& m_vertices;
    LinkedParticleMCParticleLinks& m_links;
    std::set<LinkedVertex*> already_converted{};
    const Gaussino::ReDecay::SignalTruths & m_signal_truths;
  };
};

DECLARE_COMPONENT( ReDecayMCTruthToEDM)

std::tuple<LHCb::MCParticles, LHCb::MCVertices, LHCb::MCHeader, LinkedParticleMCParticleLinks> ReDecayMCTruthToEDM::
operator()( const Gaussino::MCTruthPtrs& mctruths, const LHCb::GenHeader &genHeader, const Gaussino::ReDecay::SignalTruths &  signaltruths ) const
{
  // Create containers in TES for MCParticles and MCVertices.
  LHCb::MCParticles m_particleContainer;
  LHCb::MCVertices m_vertexContainer;
  LinkedParticleMCParticleLinks particle_links;
  size_t n_LinkedParticles{0};
  size_t n_LinkedVertices{0};
  for ( auto& mctruth : mctruths ) {
    n_LinkedParticles += mctruth->GetNParticles();
    n_LinkedVertices += mctruth->GetNVertices();
  }

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Reserving space for " << n_LinkedParticles << "(" << n_LinkedVertices << ") MCParticles(MCVertices)"
            << endmsg;
  }

  m_particleContainer.reserve( n_LinkedParticles );
  m_vertexContainer.reserve( n_LinkedVertices );
  Converter converter{m_particleContainer, m_vertexContainer, particle_links, msgStream(), signaltruths};

  // Create some MCHeader.
  LHCb::MCHeader mcHeader;
  mcHeader.setEvtNumber(genHeader.evtNumber());
  mcHeader.setRunNumber(genHeader.runNumber());
  // Create a set to store a pointer to createed primary vertices.
  // This will be searched for every root particle to assign them to the
  // same vertex if necessary
  VertexSet pvs;

  // Loop over the MCTruth objects
  for ( auto& mt : mctruths ) {

    // Getting root particles including those from any slave MCTruth objects contained within
    // FIXME: Introduce global setting server to only activate the special event processing
    // when needed? Might be slight performance improvement
    for ( auto rp : mt->GetRootParticlesIncludingSlaves() ) {
      if(rp->GetType() == Gaussino::ConversionType::REDECAY ){
        if(signaltruths.find(rp) == std::end(signaltruths)){
          throw GaudiException( "Particle not in truth map.", "Gaussino::ReDecay", StatusCode::FAILURE );
        }
        auto truth = signaltruths.at(rp);
        auto roots = truth->GetRootParticlesIncludingSlaves();
        if(roots.size() != 1){
          throw GaudiException( "SignalTruth does not have exactly one child.", "Gaussino::ReDecay", StatusCode::FAILURE );
        }
        rp = *std::begin(roots);
      }
      LHCb::MCVertex* primary{nullptr};
      auto foundpv = FindPV( rp, pvs );
      // Attach to the found vertex, if not create a new one
      if ( foundpv ) {
        primary = foundpv;
      } else {
        primary = converter.createVertex( rp->GetOriginPosition() );
        primary->setType( LHCb::MCVertex::ppCollision );
        mcHeader.addToPrimaryVertices( primary );
        // Lastly, add the vertex to the set to be found later
        pvs.insert( std::make_pair( primary, rp->HepMC() ? rp->HepMC()->production_vertex().get() : nullptr ) );
      }
      // Now recursively convert everything
      converter.convert( rp, primary );
    }
  }

  return std::make_tuple( std::move( m_particleContainer ), std::move( m_vertexContainer ), mcHeader,
                          std::move( particle_links ) );
}

//=============================================================================
// Convert a decay tree into MCParticle or to G4PrimaryParticle.
//=============================================================================

LHCb::MCVertex* ReDecayMCTruthToEDM::FindPV( LinkedParticle* lp, VertexSet& pvs ) const
{

  auto essentiallyEqual = []( float a, float b, float epsilon = 0.00001 ) {
    return fabs( a - b ) <= ( ( fabs( a ) > fabs( b ) ? fabs( b ) : fabs( a ) ) * epsilon );
  };

  // Vertex finder for the PV of a root particle
  auto finder_hepmc = [&]( VertexSet::key_type p ) {
    if ( lp->HepMC() && lp->HepMC()->production_vertex().get() == p.second ) {
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Found PV for " << *lp << " via HepMC production vertex" << endmsg;
      }
      return true;
    }
    return false;
  };
  auto finder_position = [&]( VertexSet::key_type p ) {
    if ( essentiallyEqual( p.first->position().x(), lp->GetOriginPosition().x() ) &&
         essentiallyEqual( p.first->position().y(), lp->GetOriginPosition().y() ) &&
         essentiallyEqual( p.first->position().z(), lp->GetOriginPosition().z() ) &&
         essentiallyEqual( p.first->time(), lp->GetOriginPosition().t() ) ) {
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Found PV for " << *lp << " via position comparison vertex" << endmsg;
      }
      return true;
    }
    return false;
  };
  auto found = std::find_if( std::begin( pvs ), std::end( pvs ), finder_hepmc );
  if ( found != std::end( pvs ) ) {
    return found->first;
  }
  found = std::find_if( std::begin( pvs ), std::end( pvs ), finder_position );
  if ( found != std::end( pvs ) ) {
    return found->first;
  }
  return nullptr;
}

LHCb::MCVertex* ReDecayMCTruthToEDM::Converter::createVertex( const HepMC3::FourVector& fm )
{
  auto ret = new LHCb::MCVertex{};
  ret->setPosition( Gaudi::XYZPoint{fm.x(), fm.y(), fm.z()} );
  ret->setTime( fm.t() );
  m_vertices.insert( ret );

  return ret;
}

LHCb::MCVertex* ReDecayMCTruthToEDM::Converter::createVertex( LinkedVertex* lv )
{
  auto fm  = lv->GetPosition();
  auto ret = createVertex( fm );
  already_converted.insert( lv );
  ret->setType(Gaussino::GetLinkedVertexType(lv, &msgStream));
  return ret;
}

void ReDecayMCTruthToEDM::Converter::convert( LinkedParticle* particle, LHCb::MCVertex* originVertex )
{
  auto mcp = makeMCParticle( particle );
  mcp->setOriginVertex( originVertex );
  originVertex->addToProducts( mcp );
  for ( auto& ev : particle->GetEndVtxs() ) {
    if ( already_converted.count( ev.get() ) > 0 ) {
      // The endvertex has already been converted. This happens if a LinkedVertex
      // has two ingoing particles, usually from the hard collision. In this case
      // we leave the second particle unconnected
      // FIXME: Needs to be revisited
      continue;
    }
    auto endVertex = createVertex( ev.get() );
    endVertex->setMother( mcp );
    mcp->addToEndVertices( endVertex );
    for ( auto child : ev->outgoing_particles ) {
      if(child->GetType() == Gaussino::ConversionType::REDECAY ){
        if(m_signal_truths.find(child) == std::end(m_signal_truths)){
          throw GaudiException( "Particle not in truth map.", "Gaussino::ReDecay", StatusCode::FAILURE );
        }
        auto truth = m_signal_truths.at(child);
        auto roots = truth->GetRootParticlesIncludingSlaves();
        if(roots.size() != 1){
          throw GaudiException( "SignalTruth does not have exactly one child.", "Gaussino::ReDecay", StatusCode::FAILURE );
        }
        child = *std::begin(roots);
      }
      convert( child, endVertex );
    }
    // Now convert all outgoing mctruth objects from this vertex.
    // Additional MCTruth objects that exist on root level are handled in the
    // main loop over all root particles
    for(auto & slavetruth: ev->outgoing_mctruths){
      if(msgStream.currentLevel() <= MSG::DEBUG){
        msgStream << MSG::DEBUG << "Adding outgoing MCTruth to record." << endmsg;
      };
      for ( auto& child : slavetruth->GetRootParticlesIncludingSlaves() ) {
        convert( child, endVertex );
      }
    }
  }
}

LHCb::MCParticle* ReDecayMCTruthToEDM::Converter::makeMCParticle( LinkedParticle* particle )
{
  // Create and insert into TES.
  // LHCb::MCParticle* mcp = new LHCb::MCParticle();
  auto mcp = new LHCb::MCParticle{};
  m_particles.insert( mcp );
  m_links[particle] = mcp;

  // Set properties.
  Gaudi::LorentzVector mom( particle->GetMomentum() );
  LHCb::ParticleID pid( particle->GetPDG() );
  mcp->setMomentum( mom );
  mcp->setParticleID( pid );

  //  Set the fromSignal flag
  if ( particle->HepMC() && ( HepMC3::Status::SignalInLabFrame == ( particle->HepMC()->status() ) ) ) {
    mcp->setFromSignal( true );
  }

  return mcp;
}
