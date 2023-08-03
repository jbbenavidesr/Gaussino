/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#pragma once
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Relatives.h"

namespace HepMCUtils {
  /// Helper class to wrap calls to the global HepMC3::Relatives objects
  /// And to provide a compatibility layer with old style enum based access
  /// from HepMC2. This wrapper is necessary to allow a general HepMC3::Relatives & to
  /// be obtained by any thread and to correctly direct calls to the thread_local instance
  /// if this reference is used by more than one thread.
  class WrapperRelatives {
  public:
    class AncestorsWrapper : public HepMC3::Relatives {

    public:
      virtual ~AncestorsWrapper()  = default;
      constexpr AncestorsWrapper() = default;

      HepMC3::GenParticles_type<HepMC3::GenParticlePtr> operator()( HepMC3::GenParticlePtr input ) const override {
        return HepMC3::Relatives::ANCESTORS( input );
      }
      HepMC3::GenParticles_type<HepMC3::ConstGenParticlePtr>
      operator()( HepMC3::ConstGenParticlePtr input ) const override {
        return HepMC3::Relatives::ANCESTORS( input );
      }
      HepMC3::GenParticles_type<HepMC3::GenVertexPtr> operator()( HepMC3::GenVertexPtr input ) const override {
        return HepMC3::Relatives::ANCESTORS( input );
      }
      HepMC3::GenParticles_type<HepMC3::ConstGenVertexPtr>
      operator()( HepMC3::ConstGenVertexPtr input ) const override {
        return HepMC3::Relatives::ANCESTORS( input );
      }
    };
    class DescendantsWrapper : public HepMC3::Relatives {

    public:
      virtual ~DescendantsWrapper()  = default;
      constexpr DescendantsWrapper() = default;

      HepMC3::GenParticles_type<HepMC3::GenParticlePtr> operator()( HepMC3::GenParticlePtr input ) const override {
        return HepMC3::Relatives::DESCENDANTS( input );
      }
      HepMC3::GenParticles_type<HepMC3::ConstGenParticlePtr>
      operator()( HepMC3::ConstGenParticlePtr input ) const override {
        return HepMC3::Relatives::DESCENDANTS( input );
      }
      HepMC3::GenParticles_type<HepMC3::GenVertexPtr> operator()( HepMC3::GenVertexPtr input ) const override {
        return HepMC3::Relatives::DESCENDANTS( input );
      }
      HepMC3::GenParticles_type<HepMC3::ConstGenVertexPtr>
      operator()( HepMC3::ConstGenVertexPtr input ) const override {
        return HepMC3::Relatives::DESCENDANTS( input );
      }
    };
    static const DescendantsWrapper DESCENDANTS;
    static const AncestorsWrapper   ANCESTORS;
  };
  const HepMC3::Relatives& RelativesFromID( int idx );
} // namespace HepMCUtils
