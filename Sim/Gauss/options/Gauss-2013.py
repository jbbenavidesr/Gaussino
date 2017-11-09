##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2013 data (1.38 TeV beams, nu=0.36, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2013.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam1380GeV-md100-2013-nu0.36.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2013.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")
importOptions("$APPCONFIGOPTS/Gauss/NoPacking.py")

#--Set database tags
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20171030-2"
LHCbApp().CondDBtag = "sim-20160321-3-vc-md100"
