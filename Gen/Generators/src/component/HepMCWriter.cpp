// Include files

// local
#include "Defaults/HepMCAttributes.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/WriterHEPEVT.h"
#include "HepMC3/WriterRoot.h"
#include "HepMC3/WriterRootTree.h"

// Include files
// from STL
#include <mutex>
#include <string>

// from Gaudi
#include "Defaults/Locations.h"
#include "GaudiAlg/Consumer.h"
#include "HepMC3/GenEvent.h"
#include "HepMCUser/typedefs.h"

namespace HepMC3
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
class HepMCWriter : public Gaudi::Functional::Consumer<void( const HepMC3::GenEventPtrs& )>
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

  void operator()( const HepMC3::GenEventPtrs& ) const override;
  virtual StatusCode finalize() override;
  virtual StatusCode initialize() override;

private:
  HepMC3::Writer* m_writer = nullptr;
  mutable std::mutex m_writer_lock;
  mutable std::atomic_uint m_counter{0};
};

DECLARE_COMPONENT( HepMCWriter )

StatusCode HepMCWriter::initialize()
{

  StatusCode sc = Consumer::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  if ( m_outputFileName != "" ) {
    if ( m_writer_name == "WriterRoot" ) {
      m_writer = new HepMC3::WriterRoot( m_outputFileName );
    } else if ( m_writer_name == "WriterRootTree" ) {
      m_writer = new HepMC3::WriterRootTree( m_outputFileName );
    } else if ( m_writer_name == "WriterAscii" ) {
      m_writer = new HepMC3::WriterAscii( m_outputFileName );
    } else if ( m_writer_name == "WriterHEPEVT" ) {
      m_writer = new HepMC3::WriterHEPEVT( m_outputFileName );
    } else {
      error() << "No valid writer for HepMC specified. Will not write anything." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

void HepMCWriter::operator()( const HepMC3::GenEventPtrs& hepmcevents ) const
{
  debug() << "==> Execute" << endmsg;
  if ( !m_writer ) {
    return;
  }

  // I have no idea if this is thread safe so just lock it for now
  std::lock_guard<std::mutex> writerguard( m_writer_lock );
  for ( auto& evt : hepmcevents ) {
    debug() << " Writing HepMC event with eventnumber "
            << evt->attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value() << " and runnumber "
            << evt->attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::GaudiRunNumber )->value() << endmsg;
    m_writer->write_event( *evt.get() );
    m_counter++;
  }
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HepMCWriter::finalize()
{
  if ( m_writer ) {
    m_writer->close();
    delete m_writer;
  }

  debug() << "==> Finalize" << endmsg;
  debug() << "Wrote " << m_counter << "HepMC events." << endmsg;

  return Consumer::finalize();
}
