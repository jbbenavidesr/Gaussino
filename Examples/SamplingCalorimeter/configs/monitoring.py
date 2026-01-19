###############################################################################
# (c) Copyright 2025 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

import os
from GaudiKernel import SystemOfUnits as units
from SamplingCalorimeter.calorimeter_geometry import calorimeter_default_options
from SamplingCalorimeter.calorimeter_monitoring import set_monitoring

# Read parameters from environment variables
particles_per_event = int(os.environ.get("PARTICLES_PER_EVENT", 100))
particle_energy = float(os.environ.get("PARTICLE_ENERGY_MEV", 10.0)) * units.MeV

# Configure monitoring
set_monitoring(
    geometry_opts=calorimeter_default_options,
    particle_energy=particle_energy,
    particles_per_event=particles_per_event,
)
