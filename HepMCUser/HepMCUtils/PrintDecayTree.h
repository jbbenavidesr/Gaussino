#pragma once
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include <sstream>

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

std::string PrintDecay( const HepMC3::ConstGenParticlePtr& part, int level = 0, const LHCb::IParticlePropertySvc* ppsvc = nullptr );
