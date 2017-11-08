##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2011 data (3.5 TeV beams, nu=2, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2011.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam3500GeV-md100-2011-nu2.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2011.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")
importOptions("$APPCONFIGOPTS/Gauss/NoPacking.py")


#--Set database tags
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20171030-1"
LHCbApp().CondDBtag = "sim-20160614-1-vc-md100"

