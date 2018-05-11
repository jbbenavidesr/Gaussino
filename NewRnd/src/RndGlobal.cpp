#include "NewRnd/RndGlobal.h"

thread_local CLHEP::HepRandomEngine * ThreadLocalEngine::m_engine = nullptr;
