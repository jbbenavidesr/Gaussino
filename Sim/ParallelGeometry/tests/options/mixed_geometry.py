###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Configurables import CondDB
CondDB().Upgrade = True

from Configurables import LHCbApp
LHCbApp().DDDBtag = "upgrade/dddb-20210617"
LHCbApp().CondDBtag = "upgrade/sim-20210617-vc-mu100"
LHCbApp().Simulation = True
LHCbApp().EvtMax = 1

# empty LHCb
from Configurables import Gauss
Gauss().DetectorGeo = {'Detectors': []}
Gauss().DetectorSim = {'Detectors': []}
Gauss().DetectorMoni = {'Detectors': []}
Gauss().BeamPipe = 'BeamPipeOff'
Gauss().DataType = 'Upgrade'

# Particle Gun On
# shoots just one photon along z
from Gaudi.Configuration import importOptions
importOptions("$LBPGUNSROOT/options/PGuns.py")
from Configurables import ParticleGun
pgun = ParticleGun("ParticleGun")

from Configurables import FixedMomentum
pgun.ParticleGunTool = "FixedMomentum"
pgun.addTool(FixedMomentum, name="FixedMomentum")
from GaudiKernel.SystemOfUnits import GeV
pgun.FixedMomentum.px = 0. * GeV
pgun.FixedMomentum.py = 0. * GeV
pgun.FixedMomentum.pz = 1. * GeV
pgun.FixedMomentum.PdgCodes = [22]

from Configurables import FlatNParticles
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 1
pgun.FlatNParticles.MaxNParticles = 1

# Generic options for both detectors
from GaudiKernel.SystemOfUnits import m
generic_shape = {
    'Type': 'Cuboid',
    'xPos': 0. * m,
    'yPos': 0. * m,
    'zPos': 5. * m,
    'xSize': 5. * m,
    'ySize': 5. * m,
    'zSize': 1. * m,
}

generic_sensitive = {
    'Type': 'MCCollectorSensDet',
    'RequireEDep': False,
    'OnlyForward': False,
}

generic_hit = {
    'Type': 'GetMCCollectorHitsAlg',
}

# External Detector in Mass Geometry
from Configurables import ExternalDetectorEmbedder
mass_embedder = ExternalDetectorEmbedder('MassEmbedder')

mass_embedder.Shapes = {'MassPlane': dict(generic_shape)}
mass_embedder.Shapes['MassPlane']['MaterialName'] = '/dd/Materials/Ecal/EcalPb'
mass_embedder.Sensitive = {'MassPlane': dict(generic_sensitive)}
mass_embedder.Hit = {'MassPlane': dict(generic_hit)}

Gauss().ExternalDetectorEmbedder = 'MassEmbedder'

# External Detector in Parallel Geometry
from Configurables import ExternalDetectorEmbedder
parallel_embedder_1 = ExternalDetectorEmbedder('ParallelEmbedder1')
parallel_embedder_2 = ExternalDetectorEmbedder('ParallelEmbedder2')

materials_to_prefetch = []

parallel_embedder_1.Shapes = {'ParallelPlane1': dict(generic_shape)}
parallel_embedder_1.Sensitive = {'ParallelPlane1': dict(generic_sensitive)}
parallel_embedder_1.Hit = {'ParallelPlane1': dict(generic_hit)}

parallel_embedder_1_material = '/dd/Materials/Vacuum'
parallel_embedder_1.Shapes['ParallelPlane1'][
    'MaterialName'] = parallel_embedder_1_material
materials_to_prefetch.append(
    parallel_embedder_1_material)  # prefetch the material in GaussGeo

parallel_embedder_2.Shapes = {'ParallelPlane2': dict(generic_shape)}
parallel_embedder_2.Sensitive = {'ParallelPlane2': dict(generic_sensitive)}
parallel_embedder_2.Hit = {'ParallelPlane2': dict(generic_hit)}

parallel_embedder_2_material = '/dd/Materials/Ecal/EcalPb'
parallel_embedder_2.Shapes['ParallelPlane2'][
    'MaterialName'] = parallel_embedder_2_material
materials_to_prefetch.append(
    parallel_embedder_2_material)  # prefetch the material in GaussGeo

from Configurables import ParallelGeometry
ParallelGeometry().ParallelWorlds = {
    'ParallelWorld1': {
        'ExternalDetectorEmbedder': 'ParallelEmbedder1',
    },
    'ParallelWorld2': {
        'ExternalDetectorEmbedder': 'ParallelEmbedder2',
    },
}
ParallelGeometry().ParallelPhysics = {
    'ParallelWorld1': {
        'LayeredMass': True,
    },
    'ParallelWorld2': {
        'LayeredMass': False,
    },
}

from Configurables import GaussGeo
GaussGeo().PrefetchMaterials = materials_to_prefetch

from Configurables import Gauss
Gauss().ParallelGeometry = True

import GaudiPython as GP


class CheckHits(GP.PyAlgorithm):
    def execute(self):
        evt = appMgr.evtsvc()
        mass_plane_1_vals = [
            hit.entry().X() + hit.entry().Y() + hit.energy()
            for hit in evt['MC/MassPlane/Hits']
        ]
        parallel_plane_1_vals = [
            hit.entry().X() + hit.entry().Y() + hit.energy()
            for hit in evt['MC/ParallelPlane1/Hits']
        ]
        parallel_plane_2_vals = [
            hit.entry().X() + hit.entry().Y() + hit.energy()
            for hit in evt['MC/ParallelPlane2/Hits']
        ]

        if mass_plane_1_vals == parallel_plane_1_vals == parallel_plane_2_vals:
            print("All collectors generated the same hits!"
                  )  # it means that parallel geo works ok
        if sum(mass_plane_1_vals) == sum(parallel_plane_1_vals) == sum(
                parallel_plane_1_vals) == 0.:
            print("All collectors see vacuum!"
                  )  # it means that layeres are applied correctly
        return True


appMgr = GP.AppMgr()
appMgr.addAlgorithm(CheckHits())
appMgr.run(1)
