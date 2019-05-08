#pragma once
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include <sstream>

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

std::string PrintDecay( const HepMC3::GenParticlePtr& part, int level = 0, LHCb::IParticlePropertySvc* ppsvc = nullptr );
