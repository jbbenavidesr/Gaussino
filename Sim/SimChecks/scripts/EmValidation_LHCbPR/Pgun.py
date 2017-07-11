####################################################################################################
#
# Set options for particle gun. Options are imported from config function.
# 
# Peter Griffith 21/11/2014
# email:peter.griffith@cern.ch
#
####################################################################################################


import sys
sys.path.append('./')
from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *
from configurations import config


opts   = config()
pgunID = opts['pgunID']
pgunE  = opts['pgunE']


ParticleGun = ParticleGun("ParticleGun")
ParticleGun.EventType = 53210205;


from Configurables import MomentumRange
ParticleGun.addTool(MomentumRange, name="MomentumRange")
ParticleGun.ParticleGunTool = "MomentumRange"


from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
ParticleGun.MomentumRange.PdgCodes = [-1*pgunID, pgunID]


ParticleGun.MomentumRange.MomentumMin = pgunE*GeV
ParticleGun.MomentumRange.MomentumMax = pgunE*GeV
ParticleGun.MomentumRange.ThetaMin    = 0.0*rad 
ParticleGun.MomentumRange.ThetaMax    = 0.39*rad 

