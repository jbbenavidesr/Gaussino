#include "CLHEP/Random/RandomEngine.h"

class ThreadLocalEngine{
  ThreadLocalEngine() = delete;
  public:
  class Guard{
    public:
    Guard() = delete;
    Guard(const Guard &) = delete;
    Guard(Guard &&) = delete;

    Guard(CLHEP::HepRandomEngine & engine){
      ThreadLocalEngine::Set(engine);
    }
    ~Guard(){ThreadLocalEngine::Unset();}
  };
  static CLHEP::HepRandomEngine & Get(){return *m_engine;}
  private:
    static void Set( CLHEP::HepRandomEngine & engine ){m_engine = &engine;}
    static void Unset(){m_engine = nullptr;}

  thread_local static CLHEP::HepRandomEngine * m_engine;
};
