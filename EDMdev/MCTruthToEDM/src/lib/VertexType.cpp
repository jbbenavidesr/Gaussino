#include "MCTruthToEDM/VertexType.h"
namespace Gaussino {

  LHCb::MCVertex::MCVertexType vertexType( int id ) {
    LHCb::MCVertex::MCVertexType vType;
    switch ( id ) {
    case LHCb::MCVertex::ppCollision:
      vType = LHCb::MCVertex::ppCollision;
      break;
    case LHCb::MCVertex::DecayVertex:
      vType = LHCb::MCVertex::DecayVertex;
      break;
    case LHCb::MCVertex::OscillatedAndDecay:
      vType = LHCb::MCVertex::OscillatedAndDecay;
      break;
    case LHCb::MCVertex::HadronicInteraction:
      vType = LHCb::MCVertex::HadronicInteraction;
      break;
    case LHCb::MCVertex::Bremsstrahlung:
      vType = LHCb::MCVertex::Bremsstrahlung;
      break;
    case LHCb::MCVertex::PairProduction:
      vType = LHCb::MCVertex::PairProduction;
      break;
    case LHCb::MCVertex::Compton:
      vType = LHCb::MCVertex::Compton;
      break;
    case LHCb::MCVertex::DeltaRay:
      vType = LHCb::MCVertex::DeltaRay;
      break;
    case LHCb::MCVertex::PhotoElectric:
      vType = LHCb::MCVertex::PhotoElectric;
      break;
    case LHCb::MCVertex::Annihilation:
      vType = LHCb::MCVertex::Annihilation;
      break;
    case LHCb::MCVertex::RICHPhotoElectric:
      vType = LHCb::MCVertex::RICHPhotoElectric;
      break;
    case LHCb::MCVertex::Cerenkov:
      vType = LHCb::MCVertex::Cerenkov;
      break;
    case LHCb::MCVertex::LHCHalo:
      vType = LHCb::MCVertex::LHCHalo;
      break;
    case LHCb::MCVertex::MuonBackground:
      vType = LHCb::MCVertex::MuonBackground;
      break;
    case LHCb::MCVertex::MuonBackgroundFlat:
      vType = LHCb::MCVertex::MuonBackgroundFlat;
      break;
    case LHCb::MCVertex::MuonBackgroundSpillover:
      vType = LHCb::MCVertex::MuonBackgroundSpillover;
      break;
    default:
      vType = LHCb::MCVertex::Unknown;
    }

    return vType;
  }

  LHCb::MCVertex::MCVertexType GetLinkedVertexType( LinkedVertex* lv, MsgStream* msgStream ) {
    // Check if this vertex has an associated hepmc vertex, if it does it was
    // produced during the generation phase and we set sensible vertex types
    auto                         hepmc_endvertex = lv->hepmc_vtx;
    LHCb::MCVertex::MCVertexType return_vtx_type = LHCb::MCVertex::Unknown;
    if ( hepmc_endvertex ) {
      // Check if a single particle went in and oscillated
      auto parent = *std::begin( lv->incoming_particle );

      if ( parent->HasOscillated() && lv->incoming_particle.size() == 1 ) {
        return_vtx_type = LHCb::MCVertex::OscillatedAndDecay;
      } else if ( ( 4 == std::abs( parent->GetPDG() ) ) || ( 5 == std::abs( parent->GetPDG() ) ) )
        return_vtx_type = LHCb::MCVertex::StringFragmentation;
      else
        return_vtx_type = LHCb::MCVertex::DecayVertex;
    } else if ( lv->outgoing_particles.size() > 0 ) {
      // If the vertex was not produced during the generation phase, get something from G4
      // Therefore, all children should have been handled by G4 and have the same creatorID assigned
      if ( lv->HasOutgoingMCTruth() ) {
        if ( msgStream )
          *msgStream << MSG::ERROR
                     << "Failed to set LHCb::MCVertex type. Not generation and has outgoing MCTruth. Don't know how "
                        "you managed this but it is not supported."
                     << endmsg;
      }
      if ( std::all_of( std::begin( lv->outgoing_particles ), std::end( lv->outgoing_particles ),
                        []( LinkedParticle* p ) -> bool { return p->G4Truth(); } ) ) {
        auto first_proc = ( *std::begin( lv->outgoing_particles ) )->G4Truth()->GetCreatorID();
        if ( std::all_of( std::begin( lv->outgoing_particles ), std::end( lv->outgoing_particles ),
                          [&]( LinkedParticle* p ) -> bool { return p->G4Truth()->GetCreatorID() == first_proc; } ) ) {
          return_vtx_type = vertexType( first_proc );
        } else {
          if ( msgStream )
            *msgStream << MSG::WARNING << "Failed to set LHCb::MCVertex type. Not all G4 outgoing have same creator ID."
                       << endmsg;
          std::set<LHCb::MCVertex::MCVertexType>      unique_types{};
          std::multiset<LHCb::MCVertex::MCVertexType> types{};
          for ( auto& part : lv->outgoing_particles ) {
            unique_types.insert( vertexType( part->G4Truth()->GetCreatorID() ) );
            types.insert( vertexType( part->G4Truth()->GetCreatorID() ) );
          }
          if ( msgStream ) *msgStream << MSG::WARNING << "Choices:" << endmsg;
          unsigned int                 top_count{0};
          LHCb::MCVertex::MCVertexType most_common_type{LHCb::MCVertex::Unknown};
          for ( auto& t : unique_types ) {
            if ( msgStream ) *msgStream << MSG::WARNING << " --- " << t << " #" << types.count( t ) << endmsg;
            if ( top_count < types.count( t ) ) {
              top_count        = types.count( t );
              most_common_type = t;
            }
          }
          if ( msgStream ) *msgStream << MSG::WARNING << "Taking most common type: " << most_common_type << endmsg;
          return_vtx_type = most_common_type;
        }
      } else {
        if ( msgStream )
          *msgStream << MSG::ERROR << "Failed to set LHCb::MCVertex type. No generation and not all children from G4."
                     << endmsg;
      }
    } else {
      if ( msgStream )
        *msgStream << MSG::ERROR << "Failed to set LHCb::MCVertex type. No outgoing particles." << endmsg;
    }
    return return_vtx_type;
  }

} // namespace Gaussino
