#ifndef REDECAYPRODUCTION_H
#define REDECAYPRODUCTION_H 1

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"

class IGaussRedecayStr;  ///< GaussRedecay storage service

/**
 * Implementation of a production tool to be used in SignalPlain as the
 * production tool when generating the redecayed event. Its only purpose is the
 * construction of an event based on the information stored in the service when
 * asked to 'generate' an event. The decay happens due to the decay heavy
 * particles mechanism in SignalPlain.
 *
 * @class  RedecayProduction
 * @file   RedecayProduction.h
 * @author Dominik Muller
 * @date   2016-04-28
 */
class RedecayProduction : public GaudiTool, virtual public IProductionTool {
public:
    RedecayProduction(const std::string& type, const std::string& name,
                      const IInterface* parent);

    /// Set of functions we need for this
    StatusCode initialize() override;
    StatusCode finalize() override;
    StatusCode generateEvent(HepMC::GenEvent*,
                                     LHCb::GenCollision*) override;

    // set of functions which need a dummy implementation as they are abstract
    // in IProductionTool and some are called in ExternalGenerator so be sure to
    // not do something silly
    StatusCode initializeGenerator() override {
        return StatusCode::SUCCESS;
    };
    void setStable(const LHCb::ParticleProperty*) override {}
    void updateParticleProperties(
        const LHCb::ParticleProperty*) override {}
    void turnOnFragmentation() override {}
    void turnOffFragmentation() override {}
    StatusCode hadronize(HepMC::GenEvent*,
                                 LHCb::GenCollision*) override {
        return StatusCode::SUCCESS;
    }
    void savePartonEvent(HepMC::GenEvent*) override {}
    void retrievePartonEvent(HepMC::GenEvent*) override {}
    void printRunningConditions() override {}
    bool isSpecialParticle(
        const LHCb::ParticleProperty*) const override {
        return false;
    }
    StatusCode setupForcedFragmentation(const int) override {
        return StatusCode::SUCCESS;
    }

private:
    std::string m_gaussRDSvcName;
    IGaussRedecayStr* m_gaussRDStrSvc = nullptr;
};

#endif
