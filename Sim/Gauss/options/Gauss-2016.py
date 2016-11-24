##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2016 data (6.5 TeV beams, nu=1.6, spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2016.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Beam6500GeV-md100-2016-nu1.6.py")
importOptions("$APPCONFIGOPTS/Gauss/EnableSpillover-25ns.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2016.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")


#--Set database tags for 2016 as in Sim09b for average conditions
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20150724"
LHCbApp().CondDBtag = "sim-20161124-2-vc-m100"
