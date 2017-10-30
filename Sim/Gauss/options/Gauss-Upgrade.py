## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade.py <someInputJobConfiguration>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import Gauss

## # Here are beam settings as for various nu (i.e. mu and Lumi per bunch with
## # 25 ns bunch spacing are given
## This is the Run3 default luminosity 
##   nu=7.6 (i.e. mu=5.31, Lumi=2.0*(10**33) with2400 colliding bunches)
importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu7.6-HorExtAngle.py")
## Nominal Lumi to begin with
##   nu=3.8 (i.e. mu=2.66, Lumi=1.0*(10**33) )
#importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu3.8-HorExtAngle.py")
## For robustness studies
##   nu=11.4 (i.e. mu=4, Lumi=1.77*(10**33) )
#importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu11.4-HorExtAngle.py")


## # The spill-over is off for quick tests
## to enable spill-over use the followign options
#importOptions("$APPCONFIGOPTS/Gauss/EnableSpillover-25ns.py")

# And the upgrade database is picked up 
# The baseline upgrade configuration is given here with the latest supported global tag for this
# version of Gauss - the FT neutron shield is added by hand in the Baseline options
from Configurables import LHCbApp
from Configurables import CondDB

Gauss().DataType     = "Upgrade" 
CondDB().Upgrade     = True
LHCbApp().DDDBtag   = "dddb-20171010"
LHCbApp().CondDBtag = "sim-20170301-vc-md100"

## Choose the detector configuration to run selecting the appropriate file
# Existing detector with MapPMT in RICHes and no Aerogel
importOptions("$GAUSSOPTS/Gauss-Upgrade-Baseline.py") 









