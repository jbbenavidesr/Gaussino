#include "GiGaRunActionCommandFAC.h"

DECLARE_COMPONENT_WITH_ID( GiGaRunActionCommandFAC, "GiGaRunActionCommand" )

G4UserRunAction* GiGaRunActionCommandFAC::construct() const
{
  auto ret         = new GiGaRunActionCommand{};
  ret->m_beginCmds = m_beginCmds;
  ret->m_endCmds   = m_endCmds;
  ret->SetMessageInterface( message_interface() );
  return ret;
}
