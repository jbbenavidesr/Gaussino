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
#include "HepMC/GenEvent.h"
#include "HepMC3/GenEvent.h"
#include "Pythia8/Pythia.h"

#include "Pythia8Plugins/HepMC3.h"

int main( int, char** ) {
  Pythia8::Pythia pythia{
      "/cvmfs/sft.cern.ch/lcg/releases/LCG_96/MCGenerators/pythia8/240/x86_64-centos7-gcc8-opt/share/Pythia8/xmldoc",
      false };

  pythia.readString( "Beams:frameType = 3" );
  pythia.readString( "Beams:idA = 2212" );
  pythia.readString( "Beams:idB = 2212" );
  pythia.readString( "SoftQCD:singleDiffractive = on" );
  pythia.readString( "Beams:allowMomentumSpread = on" );

  pythia.init();
  for ( size_t i = 0; i < 100; i++ ) {
    pythia.next();
    auto                    HepMC3_Event = new HepMC3::GenEvent{};
    auto                    HepMC2_Event = new HepMC::GenEvent{};
    HepMC3::Pythia8ToHepMC3 HepMC3_conv;
    HepMC::Pythia8ToHepMC   HepMC2_conv;
    HepMC3_conv.fill_next_event( pythia, HepMC3_Event );
    HepMC2_conv.fill_next_event( pythia, HepMC2_Event );
    size_t n2 = HepMC2_Event->particles_size();
    size_t n3 = HepMC3_Event->particles().size();
    if ( n2 != n3 ) { std::cerr << "#Particles not identical! HepMC2: " << n2 << ", HepMC3: " << n3 << std::endl; }
    delete HepMC3_Event;
    delete HepMC2_Event;
  }
}
