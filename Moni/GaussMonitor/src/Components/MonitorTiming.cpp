/*****************************************************************************\
* (c) Copyright 2023 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#include <fmt/core.h>
#include <mutex>

// Gaudi
#include "GaudiKernel/ConcurrencyFlags.h"

// Gaussino
#include "GiGaMTCoreMessage/IMessage.h"
#include "GiGaMTFactories/GiGaFactoryBase.h"
#include "GiGaMTFactories/GiGaTool.h"

// Geant4
#include "G4ParticleTable.hh"
#include "G4Step.hh"
#include "G4Timer.hh"
#include "G4UserSteppingAction.hh"
#include "G4VProcess.hh"

namespace Gsino {
  using VolumesMap         = std::map<std::string, std::vector<std::string>>;
  using Times              = std::map<std::string, double>;
  using VolPart            = std::pair<std::string, int>;
  using VolTime            = std::pair<std::string, double>;
  using VolPartTimes       = std::map<VolPart, double>;
  using ThreadTimes        = std::map<std::thread::id, Times>;
  using ThreadVolPartTimes = std::map<std::thread::id, VolPartTimes>;
  using TimesVec           = std::vector<Times>;

  struct DetailedTimingAction : public G4UserSteppingAction, public Gsino::Message {

    using G4UserSteppingAction::G4UserSteppingAction;
    void UserSteppingAction( const G4Step* ) override;

    ThreadVolPartTimes* m_volPartTimes = nullptr;
    ThreadTimes*        m_procTimes    = nullptr;

  private:
    G4Timer*          m_stepTimer    = new G4Timer;
    bool              m_timerStarted = false;
    static std::mutex m_timesMutex;
  };

  struct DetailedTimingActionFactory : public extends<GiGaTool, GiGaFactoryBase<G4UserSteppingAction>> {
    using extends::extends;

    StatusCode                    initialize() override;
    StatusCode                    finalize() override;
    StatusCode                    printAndWriteCSV( std::string, TimesVec&, bool ) const;
    virtual G4UserSteppingAction* construct() const override;

    mutable ThreadVolPartTimes m_volPartTimes;
    mutable ThreadTimes        m_procTimes;

  private:
    bool m_useGeant4Vols = false;

    Gaudi::Property<VolumesMap>  m_detectorsMap{ this, "DetectorPatterns", {} };
    Gaudi::Property<std::string> m_fileNameStem{ this, "OutputCSVNameStem", "" };
    Gaudi::Property<std::string> m_fileDir{ this, "OutputCSVDir", "./" };
  };
} // namespace Gsino

DECLARE_COMPONENT( Gsino::DetailedTimingActionFactory )

std::mutex Gsino::DetailedTimingAction::m_timesMutex{};

void Gsino::DetailedTimingAction::UserSteppingAction( const G4Step* step ) {
  double stepTime = 0.;
  if ( m_timerStarted ) {
    m_stepTimer->Stop();
    stepTime = m_stepTimer->GetRealElapsed();
  }
  auto vol    = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  auto proc   = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  auto partId = step->GetTrack()->GetDefinition()->GetPDGEncoding();
  {
    std::lock_guard lock( m_timesMutex );
    auto            tid = std::this_thread::get_id();
    ( *m_volPartTimes )[tid][std::make_pair( vol, partId )] += stepTime;
    ( *m_procTimes )[tid][proc] += stepTime;
  }
  m_stepTimer->Start();
  m_timerStarted = true;
}

G4UserSteppingAction* Gsino::DetailedTimingActionFactory::construct() const {
  auto tmp            = new DetailedTimingAction();
  tmp->m_volPartTimes = &m_volPartTimes;
  tmp->m_procTimes    = &m_procTimes;
  return tmp;
}

StatusCode Gsino::DetailedTimingActionFactory::printAndWriteCSV( std::string fileSuffix, TimesVec& timesVec,
                                                                 bool sortByValue = true ) const {
  std::stringstream ss;
  ss << "Detailed timing for " << fileSuffix << " [s]:" << std::endl;
  ss << std::left;

  auto          fileName = m_fileDir + m_fileNameStem + fileSuffix + ".csv";
  std::ofstream outf( fileName );
  if ( !outf.is_open() ) {
    error() << "Could not open file " << fileName << endmsg;
    return StatusCode::FAILURE;
  }
  outf << "# All values are given in seconds." << std::endl;

  std::map<std::string, std::vector<double>> tmap;
  std::map<std::string, double>              totals;
  auto                                       threadsNo = Gaudi::Concurrency::ConcurrencyFlags::numThreads();
  for ( size_t thID = 0; thID < threadsNo; thID++ ) {
    for ( auto& [K, V] : timesVec[thID] ) {
      if ( tmap[K].size() == 0 ) { tmap[K] = std::vector<double>( threadsNo + 1, 0. ); }
      tmap[K][thID + 1] = V;
      tmap[K][0] += V / threadsNo;
    }
  }

  using Pair = std::pair<std::string, std::vector<double>>;
  std::vector<Pair> tvec( tmap.begin(), tmap.end() );

  if ( sortByValue ) {
    std::sort( tvec.begin(), tvec.end(),
               []( const Pair& a, const Pair& b ) { return a.second.front() > b.second.front(); } );
  }

  outf << "#Name,Mean";
  ss << std::setw( 30 ) << "Name" << std::setw( 15 ) << "Mean";
  for ( size_t i = 0; i < threadsNo; ++i ) {
    outf << ",Thread " << i;
    ss << std::setw( 15 ) << "Thread " + std::to_string( i );
  }
  outf << std::endl;
  ss << std::endl;

  for ( auto const& [K, VS] : tvec ) {
    outf << K << ",";
    ss << std::setw( 30 ) << K;
    for ( auto const& V : VS ) {
      auto PV = fmt::format( "{:.3e}", V );
      outf << PV << ",";
      ss << std::setw( 15 ) << PV;
    }
    outf << std::endl;
    ss << std::endl;
  }

  outf.close();
  info() << ss.str() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode Gsino::DetailedTimingActionFactory::initialize() {
  return extends::initialize().andThen( [&]() {
    if ( m_detectorsMap.value().empty() ) {
      m_useGeant4Vols = true;
      info() << "No detector patterns provided, using Geant4 volumes instead." << endmsg;
    }
    return StatusCode::SUCCESS;
  } );
}

StatusCode Gsino::DetailedTimingActionFactory::finalize() {
  StatusCode sc = StatusCode::SUCCESS;

  TimesVec detTimesPerThread, partDetTimesPerThread, totalTimes;
  auto     particleTable = G4ParticleTable::GetParticleTable();

  debug() << "Scanning for known volumes:" << endmsg;
  std::set<std::string> foundDetParts, otherDetParts;
  for ( auto const& [tid, volPartTimes] : m_volPartTimes ) {
    Times  detTimes, partDetTimes;
    double knownDetectorsTime = 0., allDetectorsTime = 0.;

    auto addVolume = [&]( const std::string& volName, int pId, double time ) {
      std::string partName = "";
      if ( pId == 0 ) {
        partName = "opticalphoton";
      } else if ( auto partDef = particleTable->FindParticle( pId ); partDef ) {
        partName = partDef->GetParticleName();
      } else {
        partName = "unknown";
      }
      detTimes[volName] += time;
      partDetTimes[volName + "," + partName] += time;
      knownDetectorsTime += time;
    };

    for ( auto const& [volpart, time] : volPartTimes ) {
      if ( m_useGeant4Vols ) {
        addVolume( volpart.first, volpart.second, time );
      } else {
        bool matched = false;
        for ( auto const& [knownDet, patterns] : m_detectorsMap.value() ) {
          for ( auto const& pattern : patterns ) {
            if ( volpart.first.find( pattern ) != std::string::npos ) {
              matched = true;
              if ( msgLevel( MSG::DEBUG ) ) {
                if ( foundDetParts.find( volpart.first ) == foundDetParts.end() ) {
                  debug() << "Found '" << volpart.first << "' matching '" << pattern << "'" << endmsg;
                  foundDetParts.insert( volpart.first );
                }
              }
              addVolume( knownDet, volpart.second, time );
              break;
            }
          }
        }
        if ( msgLevel( MSG::DEBUG ) ) {
          if ( !matched && otherDetParts.find( volpart.first ) == otherDetParts.end() ) {
            debug() << "Unmatched '" << volpart.first << "'" << endmsg;
            otherDetParts.insert( volpart.first );
          }
        }
      }
      allDetectorsTime += time;
    }
    detTimesPerThread.push_back( detTimes );
    partDetTimesPerThread.push_back( partDetTimes );
    totalTimes.push_back( {
        { "Time in known detectors", knownDetectorsTime },
        { "Time in all detectors", allDetectorsTime },
    } );
  }

  TimesVec procTimes;
  for ( const auto& pair : m_procTimes ) { procTimes.push_back( pair.second ); }
  sc &= printAndWriteCSV( "DetectorTiming", detTimesPerThread );
  sc &= printAndWriteCSV( "ParticleDetectorTiming", partDetTimesPerThread );
  sc &= printAndWriteCSV( "ProcessTiming", procTimes );
  sc &= printAndWriteCSV( "GeneralTiming", totalTimes, false );
  sc &= extends::finalize();
  return sc;
}
