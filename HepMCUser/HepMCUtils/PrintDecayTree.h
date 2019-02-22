#pragma once
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include <sstream>

// from Gaudi
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

std::string PrintDecay( const HepMC::GenParticlePtr& part, int level = 0, LHCb::IParticlePropertySvc* ppsvc = nullptr );
