// Include files

// local
#include "HepMCWriter.h"
#include "Defaults/HepMCAttributes.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/WriterHEPEVT.h"
//#include "HepMC3/WriterRoot.h"
//#include "HepMC3/WriterRootTree.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HepMCWriter
//
// 2018-03-23 : D. Muller
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_COMPONENT( HepMCWriter )

StatusCode HepMCWriter::initialize()
{

  StatusCode sc = Consumer::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  if ( m_outputFileName != "" ) {
    //if ( m_writer_name == "WriterRoot" ) {
      //m_writer = new HepMC3::WriterRoot( m_outputFileName );
    //} else if ( m_writer_name == "WriterRootTree" ) {
      //m_writer = new HepMC3::WriterRootTree( m_outputFileName );
    if ( m_writer_name == "WriterAscii" ) {
      m_writer = new HepMC3::WriterAscii( m_outputFileName );
    } else if ( m_writer_name == "WriterHEPEVT" ) {
      m_writer = new HepMC3::WriterHEPEVT( m_outputFileName );
    } else {
      error() << "No valid writer for HepMC specified. Will not write anything." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

void HepMCWriter::operator()( const std::vector<HepMC3::GenEvent>& hepmcevents ) const
{
  debug() << "==> Execute" << endmsg;
  if ( !m_writer ) {
    return;
  }

  // I have no idea if this is thread safe so just lock it for now
  std::lock_guard<std::mutex> writerguard( m_writer_lock );
  for ( auto& evt : hepmcevents ) {
    debug() << " Writing HepMC event with eventnumber "
            << evt.attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::GaudiEventNumber )->value() << " and runnumber "
            << evt.attribute<HepMC3::IntAttribute>( Gaussino::HepMC::Attributes::GaudiRunNumber )->value() << endmsg;
    m_writer->write_event( evt );
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
