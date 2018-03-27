// Include files

// local
#include "HepMC/WriterRoot.h"
#include "HepMC/WriterRootTree.h"
#include "HepMC/WriterAscii.h"
#include "HepMC/WriterHEPEVT.h"
#include "HepMCWriter.h"

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
    if ( m_writer_name == "WriterRoot") {
      m_writer = new HepMC::WriterRoot( m_outputFileName );
    } else if(m_writer_name == "WriterRootTree"){
      m_writer = new HepMC::WriterRootTree( m_outputFileName );
    } else if(m_writer_name == "WriterAscii"){
      m_writer = new HepMC::WriterAscii( m_outputFileName );
    } else if(m_writer_name == "WriterHEPEVT"){
      m_writer = new HepMC::WriterHEPEVT( m_outputFileName );
    } else {
      error() << "No valid writer for HepMC specified. Will not write anything." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

void HepMCWriter::operator()( const std::vector<HepMC::GenEvent>& hepmcevents ) const
{
  debug() << "==> Execute" << endmsg;
  if ( !m_writer ) {
    return;
  }

  // I have no idea if this is thread safe so just lock it for now
  std::lock_guard<std::mutex> writerguard( m_writer_lock );
  for ( auto& evt : hepmcevents ) {
    m_writer->write_event( evt );
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

  return Consumer::finalize();
}
