#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IThreadInitTool.h"
#include "LbPythia8/Pythia8ProductionMT.h"

#include <atomic>

class P8ThreadInitTool : public GaudiTool, virtual public IThreadInitTool
{
  using GaudiTool::GaudiTool;

  /// Perform worker thread initialization. Called concurrently on each thread.
  virtual void initThread() override
  {
    for ( auto& tool : Pythia8ProductionMT::Instances() ) {
      tool->InitializeThread();
    }
    m_n_init++;
  };

  /// Perform worker thread finalization. Called concurrently on each thread.
  virtual void terminateThread() override
  {
    for ( auto& tool : Pythia8ProductionMT::Instances() ) {
      tool->FinalizeThread();
    }
    m_n_init--;
  };

  virtual unsigned int nInit() const override { return m_n_init; }

private:
  std::atomic_uint m_n_init{0};
};
