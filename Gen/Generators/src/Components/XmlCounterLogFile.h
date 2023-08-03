/*****************************************************************************\
* (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  *
*                                                                             *
* This software is distributed under the terms of the Apache License          *
* version 2 (Apache-2.0), copied verbatim in the file "COPYING".              *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/
#ifndef GENERATORS_XMLCOUNTERLOGFILE_H
#define GENERATORS_XMLCOUNTERLOGFILE_H 1

// Include files
// from STL
#include <fstream>
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GenInterfaces/ICounterLogFile.h" // Interface

/** @class XmlCounterLogFile XmlCounterLogFile.h component/XmlCounterLogFile.h
 *
 *  Tool to write counters in a xml file
 *
 *  @author Patrick Robbe
 *  @date   2013-01-15
 */
class XmlCounterLogFile : public GaudiTool, virtual public ICounterLogFile {
public:
  /// Standard constructor
  XmlCounterLogFile( const std::string& type, const std::string& name, const IInterface* parent );

  virtual ~XmlCounterLogFile(); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize() override;

  /// Finalize method
  virtual StatusCode finalize() override;

  /// Add efficiency number
  virtual void addEfficiency( const std::string& name, const unsigned int after, const unsigned int before,
                              const double fraction, const double err_fraction ) override;

  virtual void addCounter( const std::string& name, const unsigned int value ) override;

  virtual void addFraction( const std::string& name, const unsigned int number, const double fraction,
                            const double err_fraction ) override;

  virtual void addCrossSection( const std::string& name, const unsigned int processId, const unsigned int number,
                                const double value ) override;

  virtual void addEventType( const unsigned int evType ) override;

  /// Add the Gauss version
  virtual void addGaussVersion( const std::string& version ) override;

  virtual void addMethod( const std::string& type ) override;

  virtual void addGenerator( const std::string& generator ) override;

protected:
private:
  std::string   m_version;
  std::string   m_fileName;
  std::ofstream m_file;
};
#endif // GENERATORS_XMLCOUNTERLOGFILE_H
