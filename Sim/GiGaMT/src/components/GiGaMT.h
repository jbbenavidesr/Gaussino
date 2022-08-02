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

// from STD & STL
#include <chrono>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>

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
#include "GiGaMTCoreUtils/GiGaMTUtils.h"
#include "GiGaMTCoreRun/GiGaWorkerPayload.h"
#include "Utils/ToolProperty.h"
#include "SimInterfaces/IG4MonitoringTool.h"

// Forwad declarations
// from Gaudi
class IChronoStatSvc;
class ISvcLocator;
template <class TYPE>
class SvcFactory;
class IHepMC3ToMCTruthConverter;

// GiGaMT factories
template <typename T, typename... Args>
class GiGaFactoryBase;
// from G4
class G4UImanager;
class G4VisManager;
class G4VExceptionHandler;
class G4VUserPhysicsList;
class G4VUserActionInitialization;
class G4VUserPhysicsList;
class GiGaWorkerPilot;
class GiGaMTRunManager;
class G4VUserDetectorConstruction;

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
  // Gaudi::Property<std::string> m_MTRunMgrFactoryName{this, "MTRunManagerFactory", "GiGaMTRunManagerFAC"};
  ToolHandle<GiGaFactoryBase<GiGaMTRunManager>> m_mTRunManagerFactory{this, "MTRunManagerFactory",
                                                                      "GiGaMTRunManagerFAC"};
  ToolHandle<GiGaFactoryBase<G4VUserPhysicsList>> m_physListFactory{this, "PhysicsListFactory", "GiGaMT_FTFP_BERT"};
  ToolHandle<GiGaFactoryBase<GiGaWorkerPilot>> m_workerPilotFactory{this, "WorkerPilotFactory", "GiGaWorkerPilotFAC"};
  ToolHandle<GiGaFactoryBase<G4VUserDetectorConstruction>> m_detConstFactory{this, "DetectorConstruction",
                                                                             "GiGaMTDetectorConstructionFAC"};
  ToolHandle<GiGaFactoryBase<G4VUserActionInitialization>> m_ActionInitializerFactory{this, "ActionInitializer",
                                                                                      "GiGaActionInitializer"};
  ToolHandle<IHepMC3ToMCTruthConverter> m_converterTool{this, "HepMCConverter", "HepMC3ToMCTruthConverter"};
  ToolHandleArray<IG4MonitoringTool> m_MoniTools{this};
  ToolHandle<GiGaFactoryBase<G4VisManager>> m_visMgrFactory{this, "VisManager", ""};
  Gaudi::Property<std::vector<std::string>> m_MoniToolNames{
      this, "MonitorTools", {}, tool_array_setter( m_MoniTools, m_MoniToolNames )};

  Gaudi::Property<size_t> m_nWorkerThreads{this, "NumberOfWorkerThreads", 0};
  Gaudi::Property<bool> m_splitPileUp{this, "SplitPileUp", false};
  Gaudi::Property<bool> m_printParticles{this, "PrintG4Particles", false};
  Gaudi::Property<bool> m_printMaterials{this, "PrintG4Materials", false};

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

  virtual std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> simulate( Gaussino::MCTruthConverterPtrs&&,
                                                                      HepRandomEnginePtr& ) const override;

  virtual std::tuple<G4EventProxies, Gaussino::MCTruthPtrs> simulate( const HepMC3::GenEventPtrs & _in,
                                                                      HepRandomEnginePtr& ) const override;

  /** Simulate the particle and its decay products. Results of the simulation are attached to the particle
   * as SimResults
   *  @param   ptr   GenParticlePtr
   *  @param   eng   Random engine reference
   *  @return status code
   */
  virtual std::tuple<G4EventProxyPtr, Gaussino::MCTruthPtr> simulateDecay( const HepMC3::GenParticlePtr & _in, HepRandomEnginePtr& ) const override;

  virtual bool particleKnownToGeant4(int pdg_id) const override;

protected:
  // Function to initialize the master G4MTRunManager to run in the main Gaudi
  // thread which executes the initialization of all Gaudi objects and spawns
  // the GaudiHive workers.
  virtual StatusCode InitializeMainThread() const;

  virtual StatusCode InitializeWorkerThreads() const;

private:

  mutable std::vector<std::thread> m_workerThreads{};
  mutable GiGaPayloadQueue m_payloadQueue{};

private:
  ServiceHandle<IChronoStatSvc> m_chronoSvc{"ChronoStatSvc", "ChronoStatSvc"};

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
