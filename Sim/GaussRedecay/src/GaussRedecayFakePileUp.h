#ifndef GENERATORS_FIXEDLUMINOSITY_H
#define GENERATORS_FIXEDLUMINOSITY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IPileUpTool.h"

class IGaussRedecayStr;  ///< GaussRedecay storage service

/** @class GaussRedecayFakePileUp GaussRedecayFakePileUp.h
 * "GaussRedecayFakePileUp.h"
 *
 *  Tool to get the number of pileup events to redecay.
 *  Basically just a getter to have Generation get the pileup from the
 *  service without code changes to Generation
 *
 *  @author Dominik Muller
 *  @date   2016-05-28
 */
class GaussRedecayFakePileUp : public GaudiTool, virtual public IPileUpTool {
  public:
  /// Standard constructor
  GaussRedecayFakePileUp(const std::string& type, const std::string& name,
                         const IInterface* parent);

  virtual ~GaussRedecayFakePileUp();  ///< Destructor

  virtual StatusCode initialize();

  virtual StatusCode finalize();

  virtual unsigned int numberOfPileUp();

  virtual void printPileUpCounters(){};

  private:
  std::string m_gaussRDSvcName;
  IGaussRedecayStr* m_gaussRDStrSvc = nullptr;
};
#endif  // GENERATORS_FIXEDLUMINOSITY_H
