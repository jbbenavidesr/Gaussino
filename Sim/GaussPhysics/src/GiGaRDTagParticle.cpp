// $Id: GiGaRDTagParticle.cpp,v 1.6 2009-10-20 07:35:07 marcin Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/PhysicalConstants.h"

// G4
#include "G4UnknownParticle.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"

// local
#include "G4RDTag.h"

//
#include "GiGaRDTagParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaRDTagParticle
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(GiGaRDTagParticle)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaRDTagParticle::GiGaRDTagParticle(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : GiGaPhysConstructorBase(type, name, parent) {}

//=============================================================================
// Destructor
//=============================================================================
GiGaRDTagParticle::~GiGaRDTagParticle() {}

//=============================================================================
// ConstructParticle
//=============================================================================
void GiGaRDTagParticle::ConstructParticle() { G4RDTag::RDTagDefinition(); }
