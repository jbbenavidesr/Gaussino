###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################

# STANDARD GAUSSINO OPTIONS
# i.e. number of events and threads
from Configurables import Gaussino
Gaussino().EvtMax = 100
Gaussino().EnableHive = True
Gaussino().ThreadPoolSize = 1
Gaussino().EventSlots = 1
from Configurables import GiGaMT
GiGaMT().NumberOfWorkerThreads = 1
# activate EM physics
from Gaussino.Simulation import SimPhase
SimPhase().PhysicsConstructors = [
    "GiGaMT_G4EmStandardPhysics",
]
# Enable EDM as we will create hits
from Configurables import Gaussino
Gaussino().ConvertEDM = True

# PARTICLE GUN
# -> we will create 5 photons per event
# -> transverse momentum sampled from [5, 100] MeV
# -> pseudorapidity sampled from [1., 2.1]
from Gaussino.Generation import GenPhase
from Gaudi.Configuration import DEBUG
GenPhase().ParticleGun = True
GenPhase().ParticleGunUseDefault = False
from Configurables import ParticleGun
pgun = ParticleGun("ParticleGun")
pgun.OutputLevel = DEBUG
from Configurables import FlatPtRapidity
pgun.ParticleGunTool = "FlatPtRapidity"
pgun.addTool(FlatPtRapidity, name="FlatPtRapidity")
from GaudiKernel.SystemOfUnits import GeV, MeV
pgun.FlatPtRapidity.PtMin = 5. * MeV
pgun.FlatPtRapidity.PtMax = 100. * MeV
pgun.FlatPtRapidity.RapidityMin = 1
pgun.FlatPtRapidity.RapidityMax = 2.1
pgun.FlatPtRapidity.PdgCodes = [22]
from Configurables import FlatNParticles
pgun.NumberOfParticlesTool = "FlatNParticles"
pgun.addTool(FlatNParticles, name="FlatNParticles")
pgun.FlatNParticles.MinNParticles = 5
pgun.FlatNParticles.MaxNParticles = 5

# MASS WORLD
# activating external geometry embedder in mass geometry
from Gaussino.Simulation import SimPhase
SimPhase().ExternalDetectorEmbedder = "MassGeometryEmbedder"
from Configurables import ExternalDetectorEmbedder
mass = ExternalDetectorEmbedder("MassGeometryEmbedder")
# activate empty world for testing
mass.World = {
    "WorldMaterial": "OuterSpace",
    "Type": "ExternalWorldCreator",
    "OutputLevel": DEBUG,
}
# no materials are defined during testing
# material needed for our setup are
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin
mass.Materials = {
    # almost vaccum
    "OuterSpace": {
        "AtomicNumber": 1.,
        "MassNumber": 1.01 * g / mole,
        "Density": 1.e-25 * g / cm3,
        "Pressure": 3.e-18 * pascal,
        "Temperature": 2.73 * kelvin,
        'OutputLevel': DEBUG,
    },
    # Pb for the toy calorimeter
    'Pb': {
        'Type': 'MaterialFromElements',
        'Symbols': ['Pb'],
        'AtomicNumbers': [82.],
        'MassNumbers': [207.2 * g / mole],
        'MassFractions': [1.],
        'Density': 11.29 * g / cm3,
        'State': 'Solid',
        'OutputLevel': DEBUG,
    },
}

# TOY CALORIMETER (IN MASS WORLD)
# -> It will register the hits coming from the detailed simulation
from GaudiKernel.SystemOfUnits import m, mm
# Creating a 10m x 10m x 2m toy CALO volume
# -> it will be placed at z = 3m
# -> and made out of Pb
mass.Shapes['Calo'] = {
    "Type": "Cuboid",
    "zPos": 3. * m,
    "xSize": 10. * m,
    "ySize": 10. * m,
    "zSize": 2. * m,
    "OutputLevel": DEBUG,
    "MaterialName": "Pb",
}
# Make the toy CALO an active/sensitive volume
# -> It will now register hits
# -> for this purpose we are using a generic MCCollectorSensDet factory
mass.Sensitive['Calo'] = {
    "Type": "MCCollectorSensDet",
    # print hits stats at the end of each event
    'PrintStats': True,
    # discard hits with no energy deposition
    "RequireEDep": True,
    "OnlyForward": False,
    "OnlyAtBoundary": False,
    "OutputLevel": DEBUG,
}
# Add the hit extraction algorithm
# -> this is necessary to convert G4Hits into MCHits
mass.Hit['Calo'] = {
    "Type": "GetMCCollectorHitsAlg",
    "OutputLevel": DEBUG,
}
# Add the tuple writer
# -> this is necessary to write the tuple to a .root file
# note: this won't work for many threads
mass.Moni['Calo'] = {
    "Type": "ShowerModelCaloTupleWriter",
    "HitsPropertyName": "HitsCollection",
    "OutputLevel": DEBUG,
}

# COLLECTOR PLANE (IN MASS WORLD)
# -> the collector plane collects information about
#    the particles passing through
# -> it is necessary in order to create the training sample
# -> it will be placed in the mass world since there are no
#    volume overlaps
#
# Creating a 10m x 10m x 0.01 mm volume
# -> it will be placed at z = 1m
# -> made out of vacuum
mass.Shapes["Collector"] = {
    "Type": "Cuboid",
    "zPos": 1. * m,
    "xSize": 10. * m,
    "ySize": 10 * m,
    "zSize": .01 * mm,
    "OutputLevel": DEBUG,
    "MaterialName": "OuterSpace",
}
# Make the collector sensitive
# -> It will now register hits
# -> for this purpose we are using a generic MCCollectorSensDet factory
mass.Sensitive["Collector"] = {
    "Type": "MCCollectorSensDet",
    'PrintStats': True,
    # not required to have an energy deposit as we want
    # the information about all particles
    "RequireEDep": False,
    # do not take into account particles from backsplash
    "OnlyForward": True,
    "OnlyAtBoundary": True,
    "OutputLevel": DEBUG,
}
# Add the hit extraction algorithm
# -> this is necessary to convert G4Hits into MCHits
mass.Hit["Collector"] = {
    "Type": "GetMCCollectorHitsAlg",
    "OutputLevel": DEBUG,
}
# Add the tuple writer
# -> this is necessary to write the tuple to a .root file
# note: this won't work for many threads
mass.Moni["Collector"] = {
    "Type": "ShowerModelCollectorTupleWriter",
    "HitsPropertyName": "HitsCollection",
    "OutputLevel": DEBUG,
}

# ZMAX options
# -> generating shower in the calo will create many particles
# -> we do not want to store the track IDs of these particles, but
#    just the IDs of the tracks that entered  CALO
# -> we add a threshold called zMAX -> all the particles
#    with a vertex > zMAX will have the ID of their mother
from Gaudi.Configuration import appendPostConfigAction


def updateZMax():
    from Configurables import TruthFlaggingTrackAction
    trth = TruthFlaggingTrackAction(
        "GiGaMT.GiGaActionInitializer.TruthFlaggingTrackAction")
    # just after the collector
    end_of_collector = mass.Shapes['Collector'][
        'zPos'] + mass.Shapes['Collector']['zSize'] / 2.
    trth.ZmaxForStoring = end_of_collector
    trth.OutputLevel = DEBUG


appendPostConfigAction(updateZMax)

# FAST CALO (IN PARALLEL WORLD)
# -> add a copy of the toy calo in the parallel world
# -> it will perform the fast simulation and then forward
#    the track to the mass geometry
# -> we need a separate external detector embedder for this
parallel = ExternalDetectorEmbedder("ParallelGeometryEmbedder")
# Creating a 10m x 10m x 0.01m toy CALO volume
# -> it will placed just in front of the toy calo (in the mass world)
toy_calo_front = mass.Shapes['Calo']['zPos'] - mass.Shapes['Calo']['zSize'] / 2.
parallel.Shapes['FastCalo'] = {
    "Type": "Cuboid",
    "zPos": toy_calo_front + 0.01 * mm / 2.,
    "xSize": 10. * m,
    "ySize": 10. * m,
    "zSize": 0.01 * mm,
    "OutputLevel": DEBUG,
}
# Make the FAST CALO an active/sensitive volume
# -> It will now register hits
# -> for this purpose we are using a generic MCCollectorSensDet factory
parallel.Sensitive['FastCalo'] = {
    "Type": "MCCollectorSensDet",
    'PrintStats': True,
    "RequireEDep": False,
    "OnlyForward": True,
    "OnlyAtBoundary": False,
    "OutputLevel": DEBUG,
}
# Add the hit extraction algorithm
# -> this is necessary to convert G4Hits into MCHits
parallel.Hit['FastCalo'] = {
    "Type": "GetMCCollectorHitsAlg",
    "OutputLevel": DEBUG,
}
# Add the tuple writer
# -> this is necessary to write the tuple to a .root file
# note: this won't work for many threads
parallel.Moni['FastCalo'] = {
    "Type": "ShowerModelCaloTupleWriter",
    "HitsPropertyName": "HitsCollection",
    "OutputLevel": DEBUG,
}

# PARALLEL WORLD
from Gaussino.Simulation import SimPhase
# activate parallel geometry
SimPhase().ParallelGeometry = True
from Configurables import ParallelGeometry
pargeo = ParallelGeometry()
# Options of the parallel world
pargeo.ParallelWorlds['FastCaloParallelWorld'] = {
    # -> add the external detector embedder
    'ExternalDetectorEmbedder': 'ParallelGeometryEmbedder',
    # -> add the fast simulation creator
    'FastSimulationCreator': 'ParallelFastSimulation',
    "OutputLevel": DEBUG,
}
pargeo.ParallelPhysics['FastCaloParallelWorld'] = {
    # -> LayeredMass = False eans that the parallel world will
    #    see the same materials as in the mass world
    'LayeredMass': False,
    # -> track only the gammas in the parallel world
    'ParticlePIDs': [22],
    "OutputLevel": DEBUG,
}

# SHOWER MODEL (IN PARALLEL WORLD)
from Configurables import FastSimulationCreator
from GaudiKernel.SystemOfUnits import cm, MeV
fastsim = FastSimulationCreator('ParallelFastSimulation')

fastsim.Model['CaloShowerModel'] = {
    'Type': 'ShowerModel',
    # do not kill track as we want to observe
    # detailed simulation in a standard calorimeter
    'KillTrack': False,
    # how many hits should be generated per shower
    'HitsPerShower': 2000,
    # moliere radius in Pb
    'ShowerRadius': 1.234688 * cm,
    # shape param of gamma distr.
    'ShowerShapeParameter': 2.5567196,
    # scale param of gamma distr.
    'ShowerScaleParameter': .5,
    'OutputLevel': DEBUG,
}
fastsim.Region['CaloShowerModel'] = {
    # specify to which sensitive detector will
    # that region be connected to
    'SensitiveDetectorName': 'FastCaloSDet',
    'OutputLevel': DEBUG,
}
fastsim.Physics = {
    'ParticlePIDs': [22],
    'OutputLevel': DEBUG,
}

# NTUPLE FILE
from Configurables import ApplicationMgr
ApplicationMgr().ExtSvc += ["NTupleSvc"]
from Configurables import NTupleSvc
NTupleSvc().Output = [
    "FILE1 DATAFILE='ShowerModelTuple.root' TYP='ROOT' OPT='NEW'"
]
