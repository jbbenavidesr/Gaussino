#pragma once

// Include files
// from STL
#include <mutex>
#include <string>

// from Gaudi
#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "HepMC/GenEvent.h"

namespace HepMC
{
  class Writer;
}

/** @class HepMCWriter HepMCWriter.h Algorithms/HepMCWriter.h
 *
 *  Algorithm to write the produced HepMC events as a TTree into a ROOT file
 *
 *  @author Dominik Muller
 *  @date   2018-03-23
 */
class HepMCWriter : public Gaudi::Functional::Consumer<void( const std::vector<HepMC::GenEvent>& )>
{

private:
  // Name of the output file is automatically set in the configuration and manually specific values
  // are most likely ignored!
  Gaudi::Property<std::string> m_outputFileName{this, "OutputFileName", ""};
  Gaudi::Property<std::string> m_writer_name{
      this, "Writer", "WriterRootTree", "Writer to use. Options: [WriterRoot, WriterRootTree, WriterAscii, WriterHEPEVT]"};

public:
  /// Standard constructor
  HepMCWriter( const std::string& name, ISvcLocator* pSvcLocator )
      : Consumer( name, pSvcLocator, {KeyValue{"Input", Gaussino::HepMCEventLocation::Default}} ){};

  virtual ~HepMCWriter() = default;

  void operator()( const std::vector<HepMC::GenEvent>& ) const override;
  virtual StatusCode finalize() override;
  virtual StatusCode initialize() override;

private:
  HepMC::Writer* m_writer = nullptr;
  mutable std::mutex m_writer_lock;
};
