#pragma once

// from STD & STL
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/StatusCode.h"

// from GiGa
#include "GiGaMT/GiGaException.h"
#include "GiGaMT/IGiGaMTSetUpSvc.h"
#include "GiGaMT/IGiGaMTSvc.h"
#include "GiGaMTCore/GiGaMTUtils.h"
#include "GiGaMTCore/GiGaWorkerPayload.h"

// Forwad declarations
// from Gaudi
class IChronoStatSvc;
class ISvcLocator;
template <class TYPE>
class SvcFactory;

// GiGaMT factories
template <typename T>
class GiGaFactoryBase;
// from G4
class G4UImanager;
class G4VVisManager;
class G4VExceptionHandler;
class G4VUserPhysicsList;
class G4VUserActionInitialization;
class G4VUserPhysicsList;
class GiGaWorkerPilot;
class GiGaMTRunManager;
class G4VUserDetectorConstruction;
class IHepMC3ToGeant4Tool;
class IG4MonitoringTool;

/**  @class GiGaMT GiGaMT.h
 *
 *   Implementation of abstract Interfaces IGiGaMTSvc
 *   (for event-by-event communications with Geant4)
 *    and IGiGaMTSetUpSvc (for configuration of Geant4)
 *
 *    Based on GiGa service by Vanya Belyaev
 *
 *    @author: Dominik Muller
 */

class GiGaMT : public Service, virtual public IGiGaMTSvc, virtual public IGiGaMTSetUpSvc
{
  // TODO: GiGaActionInitializer is very modular. No idea if any other option might be used here.
  Gaudi::Property<std::string> m_MTRunMgrFactoryName{this, "MTRunManagerFactory", "GiGaMTRunManagerFAC"};
  Gaudi::Property<std::string> m_PhysListFactoryName{this, "PhysicsListFactory", "GiGaMT_FTFP_BERT"};
  // TODO: GiGaActionInitializer is very modular. No idea if any other option might be used here.
  Gaudi::Property<std::string> m_UserActionInitializerName{this, "ActionInitializer", "GiGaActionInitializer"};
  Gaudi::Property<std::string> m_WorkerPilotFactoryName{this, "WorkerPilotFactory", "GiGaWorkerPilotFAC"};
  Gaudi::Property<std::string> m_DetectorConstructionName{this, "DetectorConstruction", "GiGaMTDetectorConstructionFAC"};
  Gaudi::Property<std::vector<std::string>> m_MoniToolNames{this, "MonitorTools", {}};

  Gaudi::Property<size_t> m_nWorkerThreads{this, "NumberOfWorkerThreads", 0};
  Gaudi::Property<bool> m_splitPileUp{this, "SplitPileUp", false};
  Gaudi::Property<bool> m_printParticles{this, "PrintG4Particles", false};
  Gaudi::Property<bool> m_printMaterials{this, "PrintG4Materials", false};

  /// friend factory
  friend class SvcFactory<GiGaMT>;

protected:
  using Service::Service;
  using Clock = std::chrono::high_resolution_clock;

public:
  /** service initialization
   *  @see  Service
   *  @see IService
   *  @return status code
   */
  virtual StatusCode initialize() override;

  /** service finalization
   *  @return status code
   */
  virtual StatusCode finalize() override;

  /** query interface
   *  @param   iid   InterfaceID
   *  @param   pI    placeholder for returned interface
   *  @return status code
   */
  virtual StatusCode queryInterface( const InterfaceID& iid, void** pI ) override;

  virtual G4EventProxies simulate( Gaussino::MCTruthConverterPtrs&&, HepRandomEnginePtr& ) const override;
protected:
  // Function to initialize the master G4MTRunManager to run in the main Gaudi
  // thread which executes the initialization of all Gaudi objects and spawns
  // the GaudiHive workers.
  virtual StatusCode InitializeMainThread() const;

  virtual StatusCode InitializeWorkerThreads() const;

private:
  /// accessor to GiGa Geometry Source
  // inline IGiGaGeoSrc*     geoSrc    () const { return m_geoSrc         ; } ;
  /// accessor to Chrono & Stat  service
  inline IChronoStatSvc* chronoSvc() const { return m_chronoSvc; };
  /// accessor to Tool Service
  inline IToolSvc* toolSvc() const { return m_toolSvc; };
  /// accessor to Service Locator
  inline ISvcLocator* svcLoc() const { return serviceLocator(); };

  /** assertion
   *  @param assertion   assertion condition
   *  @param msg         assertion message
   *  @param sc          assertion status code
   *  @return status code
   */
  inline StatusCode Assert( bool assertion, const std::string& msg = "GiGaSvc::unknown",
                            const StatusCode& sc = StatusCode::FAILURE ) const;

  /** assertion
   *  @param assertion   assertion condition
   *  @param msg         assertion message
   *  @param sc          assertion status code
   *  @return status code
   */
  inline StatusCode Assert( bool assertion, const char* msg, const StatusCode& sc = StatusCode::FAILURE ) const;

  /** error printout
   *  @param msg         error message
   *  @param sc          error status code
   *  @return status code
   */
  StatusCode Error( const std::string& msg, const StatusCode& sc = StatusCode::FAILURE ) const;

  /** warning printout
   *  @param msg         warning message
   *  @param sc          warning status code
   *  @return status code
   */
  StatusCode Warning( const std::string& msg, const StatusCode& sc = StatusCode::FAILURE ) const;

  /** printout
   *  @param msg            message
   *  @param lvl            printout level
   *  @param sc             status code
   *  @return status code
   */
  StatusCode Print( const std::string& msg, const MSG::Level& lvl = MSG::INFO,
                    const StatusCode& sc = StatusCode::FAILURE ) const;

  /** exception
   *  @param msg            exception message
   *  @param exc            reference to "previous" exception
   *  @param lvl            exception printout level
   *  @param sc             exception status code
   *  @return status code
   */
  StatusCode Exception( const std::string& msg, const GaudiException& exc, const MSG::Level& lvl = MSG::FATAL,
                        const StatusCode& sc = StatusCode::FAILURE ) const;

  /** exception
   *  @param msg            exception message
   *  @param exc            reference to "previous" exception
   *  @param lvl            exception printout level
   *  @param sc             exception status code
   *  @return status code
   */
  StatusCode Exception( const std::string& msg, const std::exception& exc, const MSG::Level& lvl = MSG::FATAL,
                        const StatusCode& sc = StatusCode::FAILURE ) const;

  /** exception
   *  @param msg            exception message
   *  @param lvl            exception printout level
   *  @param sc             exception status code
   *  @return status code
   */
  StatusCode Exception( const std::string& msg, const MSG::Level& lvl = MSG::FATAL,
                        const StatusCode& sc = StatusCode::FAILURE ) const;

  /** the useful method for location of tools.
   *  for empty "name" delegates to another method
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception GiGaException for invalid Tool Service
   *  @exception GiGaException for error from Tool Service
   *  @exception GiGaException for invalid tool
   *  @param type   tool type
   *  @param name   tool name
   *  @param Tool   tool itself (return)
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools
   *  @return pointer to the tool
   */
  template <class TOOL>
  TOOL* tool( const std::string& type, const std::string& name, TOOL*& Tool, const IInterface* parent = 0,
              bool create = true ) const
  {
    // for empty names delegate to another method
    if ( name.empty() ) {
      return tool( type, Tool, parent, create );
    }
    Assert( 0 != toolSvc(), "IToolSvc* points toNULL!" );
    // get the tool from Tool Service
    StatusCode sc = toolSvc()->retrieveTool( type, name, Tool, parent, create );
    Assert( sc.isSuccess(), "Could not retrieve Tool'" + type + "'/'" + name + "'", sc );
    Assert( 0 != Tool, "Could not retrieve Tool'" + type + "'/'" + name + "'" );
    // debug printout
    Print( " The Tool of type '" + Tool->type() + "'/'" + Tool->name() + "' is retrieved from IToolSvc ", MSG::DEBUG,
           sc );
    // return located tool
    return Tool;
  }

  mutable std::vector<std::thread> m_workerThreads{};
  mutable GiGaPayloadQueue m_payloadQueue{};

  /** the useful method for location of tools.
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception CaloException for invalid Tool Service
   *  @exception CaloException for error from Tool Service
   *  @exception CaloException for invalid tool
   *  @param type   tool type, could be of "Type/Name" format
   *  @param Tool   tool itself (return)
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools
   *  @return pointer to the tool
   */
  template <class TOOL>
  TOOL* tool( const std::string& type, TOOL*& Tool, const IInterface* parent = 0, bool create = true ) const
  {
    // check the environment
    Assert( 0 != toolSvc(), "IToolSvc* points toNULL!" );
    // "type" or "type/name" ?
    std::string::const_iterator it = std::find( type.begin(), type.end(), '/' );
    // "type" is compound!
    if ( type.end() != it ) {
      std::string::size_type pos = it - type.begin();
      const std::string ntype( type, 0, pos );
      const std::string nname( type, pos + 1, std::string::npos );
      return tool( ntype, // new type
                   nname, // new name
                   Tool, parent, create );
    }
    // retrieve the tool from Tool Service
    StatusCode sc = toolSvc()->retrieveTool( type, Tool, parent, create );
    Assert( sc.isSuccess(), "Could not retrieve Tool'" + type + "'", sc );
    Assert( 0 != Tool, "Could not retrieve Tool'" + type + "'" );
    // debug printout
    Print( " The Tool of type '" + Tool->type() + "'/'" + Tool->name() + "' is retrieved from IToolSvc ", MSG::DEBUG,
           sc );
    // return located tool
    return Tool;
  }

  template <class TOOL>
  inline TOOL* tool( const std::string& type, const IInterface* parent = nullptr, bool create = true ) const
  {
    // check the environment
    Assert( toolSvc(), "IToolSvc* points to NULL!" );
    // retrieve the tool from Tool Service
    TOOL* Tool          = nullptr;
    const StatusCode sc = this->toolSvc()->retrieveTool( type, Tool, parent, create );
    Assert( sc.isSuccess(), "Could not retrieve Tool'" + type + "'", sc );
    Assert( 0 != Tool, "Could not retrieve Tool'" + type + "'" );
    // return *VALID* located tool
    return Tool;
  }

private:
  IChronoStatSvc* m_chronoSvc = nullptr;
  IToolSvc* m_toolSvc         = nullptr;

  GiGaFactoryBase<GiGaMTRunManager>* m_mTRunManagerFactory           = nullptr;
  GiGaFactoryBase<G4VUserPhysicsList>* m_physListFactory             = nullptr;
  GiGaFactoryBase<GiGaWorkerPilot>* m_workerPilotFactory             = nullptr;
  GiGaFactoryBase<G4VUserDetectorConstruction>* m_detConstFactory    = nullptr;
  GiGaFactoryBase<G4VUserActionInitialization>* m_ActionInitializerFactory = nullptr;
  std::vector<IG4MonitoringTool*> m_MoniTools{};

  // std::string       m_geoSrcName          ; ///< name of geoemtry source
  // IGiGaGeoSrc*      m_geoSrc              ; ///< pointer to geometry source

  // std::string       m_GiGaPhysListName    ; ///< type/name of Physics List
  // IGiGaPhysicsList* m_GiGaPhysList        ; ///< pointer to Physics List
  // std::string       m_GiGaStackActionName ; ///< type/name of Stacking Action
  // IGiGaStackAction* m_GiGaStackAction     ; ///< pointer to   Stacking Action
  // std::string       m_GiGaTrackActionName ; ///< type/name of Tracking Action
  // IGiGaTrackAction* m_GiGaTrackAction     ; ///< pointer to   Tracking Action
  // std::string       m_GiGaStepActionName  ; ///< type/name of Stepping Action
  // IGiGaStepAction*  m_GiGaStepAction      ; ///< pointer to   Stepping Action
  // std::string       m_GiGaEventActionName ; ///< type/name of Event    Action
  // IGiGaEventAction* m_GiGaEventAction     ; ///< pointer to   Event    Action
  // std::string       m_GiGaRunActionName   ; ///< type/name of Run      Action
  // IGiGaRunAction*   m_GiGaRunAction       ; ///< pointer to   Run      Action

  // std::string       m_uiSessionName       ; ///< GiGa UI session type/name
  // IGiGaUIsession*   m_uiSession           ; ///< GiGa UI session
  // std::string       m_visManagerName      ; ///< GiGa Vis manager type/name
  // IGiGaVisManager*  m_visManager          ; ///< GiGa Vis manager

  typedef std::map<std::string, unsigned int> Counter;
  /// counter of errors
  mutable Counter m_errors{};
  /// counter of warning
  mutable Counter m_warnings{};
  /// counter of exceptions
  mutable Counter m_exceptions{};
  /// pointer to custom exception handler
  G4VExceptionHandler* m_exceptionHandler = nullptr;
};

// ============================================================================
/** assertion
 *  @param assertion   assertion condition
 *  @param msg         assertion message
 *  @param sc          assertion status code
 */
// ============================================================================
inline StatusCode GiGaMT::Assert( bool assertion, const std::string& msg, const StatusCode& sc ) const
{
  StatusCode status = StatusCode::SUCCESS;
  return ( assertion ) ? status : Exception( msg, MSG::FATAL, sc );
}

// ============================================================================
/** assertion
 *  @param assertion   assertion condition
 *  @param msg         assertion message
 *  @param sc          assertion status code
 */
// ============================================================================
inline StatusCode GiGaMT::Assert( bool assertion, const char* msg, const StatusCode& sc ) const
{
  StatusCode status = StatusCode::SUCCESS;
  return ( assertion ) ? status : Exception( msg, MSG::FATAL, sc );
}
