// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GiGa/GiGaExtPhysics.h"


/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorsky
 *  @author Vanya Belyaev
 *  @author Gloria Corti, port to Gaudi v19
 *  @date 2002-09-26, last modified 2007-01-19
 */

// Geant4 physics lists
#include "Geant4/G4DecayPhysics.hh"

#// EM physics 
#include "Geant4/G4EmStandardPhysics_option1.hh"
#include "Geant4/G4EmStandardPhysics_option2.hh"
#include "Geant4/G4EmStandardPhysics_option3.hh"
#include "Geant4/G4EmStandardPhysics.hh"
#include "Geant4/G4EmExtraPhysics.hh"
#include "Geant4/G4EmStandardPhysics_option1LHCb.hh"
#include "Geant4/G4EmStandardPhysics_option1NoApplyCuts.hh"
#include "Geant4/G4EmStandardPhysics_LHCbTest.hh"

// Ion and hadrons
#include "Geant4/G4IonPhysics.hh"
#include "Geant4/G4StoppingPhysics.hh"                // G4QStoppingPhysics.hh -> G4StoppingPhysics.hh in G4r10
#include "Geant4/G4HadronElasticPhysics.hh"
//#include "Geant4/G4HadronElasticPhysicsLHEP.hh"     // Removed from G4r10
#include "Geant4/G4HadronElasticPhysicsHP.hh"
#include "Geant4/G4NeutronTrackingCut.hh"

// LHEP hadrons
//#include "Geant4/HadronPhysicsLHEP.hh"              // Removed from G4r10

// QGSP hadrons
//#include "Geant4/G4HadronPhysicsQGSP.hh"            // Removed from G4r10
#include "Geant4/G4HadronPhysicsQGSP_BERT.hh"
#include "Geant4/G4HadronPhysicsQGSP_BERT_HP.hh"
//#include "Geant4/G4HadronPhysicsQGSP_BERT_CHIPS.hh" // Removed from G4r10
#include "Geant4/G4HadronPhysicsQGSP_FTFP_BERT.hh"

// FTFP hadrons
#include "Geant4/G4HadronPhysicsFTFP_BERT.hh"

// Declaration of the External Physics list Factories
typedef GiGaExtPhysics< G4DecayPhysics > DecayFactory;
DECLARE_TOOL_FACTORY( DecayFactory )

typedef GiGaExtPhysics< G4EmStandardPhysics_option1 > EmStdOpt1PhysFactory;
DECLARE_TOOL_FACTORY( EmStdOpt1PhysFactory )
typedef GiGaExtPhysics< G4EmStandardPhysics_option2 > EmStdOpt2PhysFactory;
DECLARE_TOOL_FACTORY( EmStdOpt2PhysFactory )
typedef GiGaExtPhysics< G4EmStandardPhysics_option3 > EmStdOpt3PhysFactory;
DECLARE_TOOL_FACTORY( EmStdOpt3PhysFactory )
typedef GiGaExtPhysics< G4EmStandardPhysics > EmStdPhysFactory;
DECLARE_TOOL_FACTORY( EmStdPhysFactory )

typedef GiGaExtPhysics< G4EmStandardPhysics_option1LHCb > EmStdLHCbPhysFactory;
DECLARE_TOOL_FACTORY( EmStdLHCbPhysFactory )
typedef GiGaExtPhysics< G4EmStandardPhysics_option1NoApplyCuts > EmStdOpt1NoCutsPhysFactory;
DECLARE_TOOL_FACTORY( EmStdOpt1NoCutsPhysFactory )
typedef GiGaExtPhysics< G4EmStandardPhysics_LHCbTest > EmStdLHCbTestPhysFactory;
DECLARE_TOOL_FACTORY( EmStdLHCbTestPhysFactory )

typedef GiGaExtPhysics< G4EmExtraPhysics > EmExtraPhysFactory;
DECLARE_TOOL_FACTORY( EmExtraPhysFactory )

typedef GiGaExtPhysics< G4IonPhysics > IonPhysFactory;
DECLARE_TOOL_FACTORY( IonPhysFactory )
typedef GiGaExtPhysics< G4StoppingPhysics > StopPhysFactory;
DECLARE_TOOL_FACTORY( StopPhysFactory )
typedef GiGaExtPhysics< G4HadronElasticPhysics > HadElPhysFactory;
DECLARE_TOOL_FACTORY( HadElPhysFactory )
// Removed in G4r10
//typedef GiGaExtPhysics< G4HadronElasticPhysicsLHEP > HadElLHEPPhysFactory;
//DECLARE_TOOL_FACTORY( HadElLHEPPhysFactory )
typedef GiGaExtPhysics< G4HadronElasticPhysicsHP > HadElHPPhysFactory;
DECLARE_TOOL_FACTORY( HadElHPPhysFactory )

typedef GiGaExtPhysics< G4NeutronTrackingCut > NeuTrkCutFactory;
DECLARE_TOOL_FACTORY( NeuTrkCutFactory )

// Removed in G4r10
//typedef GiGaExtPhysics< HadronPhysicsLHEP > HadPhysLHEPFactory;
//DECLARE_TOOL_FACTORY( HadPhysLHEPFactory )

// Removed in G4r10
//typedef GiGaExtPhysics< HadronPhysicsQGSP > HadPhysQGSPFactory;
//DECLARE_TOOL_FACTORY( HadPhysQGSPFactory )
typedef GiGaExtPhysics< G4HadronPhysicsQGSP_BERT > HadPhysQGSP_BERTFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_BERTFactory )
typedef GiGaExtPhysics< G4HadronPhysicsQGSP_BERT_HP > HadPhysQGSP_BERT_HPFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_BERT_HPFactory )
// Removed in G4r10
//typedef GiGaExtPhysics< HadronPhysicsQGSP_BERT_CHIPS > HadPhysQGSP_BERT_CHIPSFactory;
//DECLARE_TOOL_FACTORY( HadPhysQGSP_BERT_CHIPSFactory )
typedef GiGaExtPhysics< G4HadronPhysicsQGSP_FTFP_BERT > HadPhysQGSP_FTFP_BERTFactory;
DECLARE_TOOL_FACTORY( HadPhysQGSP_FTFP_BERTFactory )

typedef GiGaExtPhysics< G4HadronPhysicsFTFP_BERT > HadPhysFTFP_BERTFactory;
DECLARE_TOOL_FACTORY( HadPhysFTFP_BERTFactory )


// =========== Specialized extensions to GiGaExtPhysics ===========

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4EmStandardPhysics *newInstance(const std::string &name, int verbosity) const {
    return new G4EmStandardPhysics(verbosity, name);
  }
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_option1> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4EmStandardPhysics_option1 *newInstance(const std::string &name, int verbosity) const {
    return new G4EmStandardPhysics_option1(verbosity, name);
  }
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_option2> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4EmStandardPhysics_option2 *newInstance(const std::string &name, int verbosity) const {
    return new G4EmStandardPhysics_option2(verbosity, name);
  }
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_option3> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4EmStandardPhysics_option3 *newInstance(const std::string &name, int verbosity) const {
    return new G4EmStandardPhysics_option3(verbosity, name);
  }
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_option1NoApplyCuts> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4EmStandardPhysics_option1NoApplyCuts *newInstance(const std::string &name, int verbosity) const {
    return new G4EmStandardPhysics_option1NoApplyCuts(verbosity, name);
  }
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_option1LHCb> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("ApplyCuts", m_applyCuts = true, 
                          "Apply production cuts to all EM processes for the LHCb EM constructor");
    tool->declareProperty("NewModelForE", m_newForE = true,
                          "Use new MS models for electrons and positrons");
  }
  inline G4EmStandardPhysics_option1LHCb *newInstance(const std::string &/*name*/, int verbosity) const {
    return new G4EmStandardPhysics_option1LHCb(verbosity, m_applyCuts, m_newForE);
  }
private:
  bool m_applyCuts;
  bool m_newForE;
};

template <>
class GiGaExtPhysicsExtender<G4EmStandardPhysics_LHCbTest> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("ApplyCuts", m_applyCuts = true, 
                          "Apply production cuts to all EM processes for the LHCb EM constructor");
    tool->declareProperty("NewModelForE", m_newForE = true,
                          "Use new MS models for electrons and positrons");
  }
  inline G4EmStandardPhysics_LHCbTest *newInstance(const std::string &/*name*/, int verbosity) const {
    return new G4EmStandardPhysics_LHCbTest(verbosity, m_applyCuts, m_newForE);
  }
private:
  bool m_applyCuts;
  bool m_newForE;
};

template <>
class GiGaExtPhysicsExtender<G4DecayPhysics> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4DecayPhysics *newInstance(const std::string &name, int verbosity) const {
    return new G4DecayPhysics(name, verbosity);
  }
};


template <>
class GiGaExtPhysicsExtender<G4StoppingPhysics> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("UseMuonMinusCapture", m_useMuonMinusCapture = true,
                          "Parameter 'UseMuonMinusCapture' for the constructor of G4StoppingPhysics");
  }
  inline G4StoppingPhysics *newInstance(const std::string &name, int verbosity) const {
    return new G4StoppingPhysics(name, verbosity, m_useMuonMinusCapture);
  }
private:
  bool m_useMuonMinusCapture;
};


template <>
class GiGaExtPhysicsExtender<G4HadronElasticPhysics> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
  // no specialised properties
    //    tool->declareProperty("HighPrecision", m_highPrecision = false,
    //                          "Parameter 'HighPrecision' for the constructor of G4HadronElasticPhysics");
    // tool->declareProperty("Glauber", m_glauber = false,
    //                          "Parameter 'Glauber' for the constructor of G4HadronElasticPhysics");
  }
  inline G4HadronElasticPhysics *newInstance(const std::string &/*name*/, int verbosity) const {
    //return new G4HadronElasticPhysics(name, verbosity, m_highPrecision, m_glauber);
    return new G4HadronElasticPhysics(verbosity);
  }
//private:
//  bool m_highPrecision;
//  bool m_glauber;
};

// Removed in G4r10
/*template <>
class GiGaExtPhysicsExtender<G4HadronElasticPhysicsLHEP> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
  // No specific properties
  }
  inline G4HadronElasticPhysicsLHEP *newInstance(const std::string &name, int verbosity) const {
    return new G4HadronElasticPhysicsLHEP(verbosity);
  }
};*/

template <>
class GiGaExtPhysicsExtender<G4HadronElasticPhysicsHP> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
  // No specific properties
  }
  inline G4HadronElasticPhysicsHP *newInstance(const std::string &/*name*/, int verbosity) const {
    return new G4HadronElasticPhysicsHP(verbosity);
  }
};

// Removed in G4r10
/*template <>
class GiGaExtPhysicsExtender<HadronPhysicsQGSP> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("QuasiElastic", m_quasiElastic = true,
                          "Parameter 'quasiElastic' for the constructor of HadronPhysicsQGSP");
  }
  inline HadronPhysicsQGSP *newInstance(const std::string &name, int verbosity) const {
    return new HadronPhysicsQGSP(name, m_quasiElastic);
  }
private:
  bool m_quasiElastic;
};*/

template <>
class GiGaExtPhysicsExtender<G4HadronPhysicsQGSP_BERT> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("QuasiElastic", m_quasiElastic = true,
                          "Parameter 'quasiElastic' for the constructor of HadronPhysicsQGSP_BERT");
  }
  inline G4HadronPhysicsQGSP_BERT *newInstance(const std::string &name, int /*verbosity*/) const {
    return new G4HadronPhysicsQGSP_BERT(name, m_quasiElastic);
  }
private:
  bool m_quasiElastic;
};

template <>
class GiGaExtPhysicsExtender<G4HadronPhysicsQGSP_BERT_HP> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("QuasiElastic", m_quasiElastic = true,
                          "Parameter 'quasiElastic' for the constructor of HadronPhysicsQGSP_BERT_HP");
  }
  inline G4HadronPhysicsQGSP_BERT_HP *newInstance(const std::string &name, int /*verbosity*/) const {
    return new G4HadronPhysicsQGSP_BERT_HP(name, m_quasiElastic);
  }
private:
  bool m_quasiElastic;
};

// Removed in G4r10
/*template <>
class GiGaExtPhysicsExtender<HadronPhysicsQGSP_BERT_CHIPS> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("QuasiElastic", m_quasiElastic = true,
                          "Parameter 'quasiElastic' for the constructor of HadronPhysicsQGSP_BERT_HP");
  }
  inline HadronPhysicsQGSP_BERT_CHIPS *newInstance(const std::string &name, int verbosity) const {
    return new HadronPhysicsQGSP_BERT_CHIPS(name, m_quasiElastic);
  }
private:
  bool m_quasiElastic;
};*/

template <>
class GiGaExtPhysicsExtender<G4HadronPhysicsQGSP_FTFP_BERT> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("QuasiElastic", m_quasiElastic = true,
                          "Parameter 'quasiElastic' for the constructor of HadronPhysicsQGSP_FTFP_BERT");
  }
  inline G4HadronPhysicsQGSP_FTFP_BERT *newInstance(const std::string &name, int /*verbosity*/) const {
    return new G4HadronPhysicsQGSP_FTFP_BERT(name, m_quasiElastic);
  }
private:
  bool m_quasiElastic;
};

template <>
class GiGaExtPhysicsExtender<G4HadronPhysicsFTFP_BERT> {
public:
  inline void addPropertiesTo(AlgTool *tool) {
    tool->declareProperty("QuasiElastic", m_quasiElastic = false,
                          "Parameter 'quasiElastic' for the constructor of HadronPhysicsFTFP_BERT");
  }
  inline G4HadronPhysicsFTFP_BERT *newInstance(const std::string &name, int /*verbosity*/) const {
    return new G4HadronPhysicsFTFP_BERT(name, m_quasiElastic);
  }
private:
  bool m_quasiElastic;
};

template <>
class GiGaExtPhysicsExtender<G4NeutronTrackingCut> {
public:
  inline void addPropertiesTo(AlgTool */*tool*/) {
    // No specific properties
  }
  inline G4NeutronTrackingCut *newInstance(const std::string &name, int verbosity) const {
    return new G4NeutronTrackingCut(name, verbosity);
  }
};

