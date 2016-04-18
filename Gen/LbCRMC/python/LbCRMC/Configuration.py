"""
High level configuration tools for CRMC.
"""

__version__ = "$Id: Configuration.py, v1.0 2014/02/03 Exp $"
__author__  = "Dmitry Popov <Dmitry.Popov@cern.ch>"

from os import environ
from os.path import expandvars

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables import LHCbConfigurableUser, LHCbApp


class LbCRMC(LHCbConfigurableUser):
	## Steering options
	__slots__ = {
		 "PrintEvents"									: False
		,"RandomSeed"										: -1
		,"ReSeedCRMCRandGenEveryEvt"		: False
		,"HEModel"											: 0
		,"ProjectileID"									: 2212
		,"TargetID"											: 2212
		,"BeamMomentum"									: 0.123456789
		,"TargetMomentum"								: 0.123456789
		,"MinDecayLength"								: 1.
		,"EPOSParamFileName"						: ""
		,"BoostAndRotate"								: True
		,"SwitchOffDecayInEPOS"					: False
	}

	## Documentation dictionary
	_propertyDocDct = {
		 "PrintEvents"              		: """ Print out the information on every generated event (default False). """
		,"RandomSeed"										: """ Custom seed for CRMC's random generator (default: -1 - get seed from Gaudi random generator service). """
		,"ReSeedCRMCRandGenEveryEvt"		: """ For every event get a new random number from Gaudi's random generator service and use it to
																					reinitialise CRMC's random generator. CRMC random generator's seed is an integer between 1 and 1e9,
																					so it has 99999999 independent sequence, but each of them has about 1e21 independent random numbers.
																					Using this option allows you to use Gaudi's random generator service for every simulated event,
																					but limits you to 1e9 events (for the same collision conditions) (default False). """
		,"HEModel"                  		: """ Which model to use (CRMC must be built with these models): 0 == 'EPOS LHC', 1 == 'EPOS 1.99',
																					2 == 'QGSJet01', 3 == 'Gheisha', 4 == 'Pythia 6', 5 == 'Hijing', 6 == 'Sibyll',
																					7 == 'QGSJetII-04', 8 == 'Phojet', 12 == 'DPMJet'; (default 0).
																					FYI: PYTHIA and Hijing are switched off by default, to not intefere with LHCb's versions of the
																					generators! """
		,"ProjectileID"             		: """ Set the projectile particle id by PDG id, e.g. 2212 (proton), 1000822080 (Pb); (default 2212). """
		,"TargetID"                 		: """ Set the target particle id by PDG id, e.g. 2212 (proton), 1000822080 (Pb); (default 2212). """
		,"ProjectileMomentum"						: """ Override the projectile momentum value (GeV) (default 0.123456789, meaning that the momentum
																					is taken from BeamTool, please set both ProjectileMomentum and TargetMomentum or none). """
		,"TargetMomentum"								: """ Override the target momentum value (GeV) (default 0.123456789, meaning that the momentum is taken
																					from BeamTool, please set both TargetMomentum and ProjectileMomentum or none). """
		,"MinDecayLength"           		: """ Minimum c.Tau to define stable particles (cm), set to -1 to decay everything in CRMC; (default 1.). """
		,"EPOSParamFileName"         		: """ Path and filename of the CRMC configuration file (default is "" - CRMCProduction uses a default options set). """
		,"BoostAndRotate"           		: """ Apply or do not apply boost and rotation (default True). """
		,"SwitchOffDecayInEPOS"     		: """ Toggle on/off the decaying of particles in EPOS (default False). """
	}

	## Apply the configuration
	def __apply_configuration__(self):
		print "******* calling ", self.name()
