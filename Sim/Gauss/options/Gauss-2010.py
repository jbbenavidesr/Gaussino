##############################################################################
# File for running Gauss with Sim08 configuration and beam conditions as in
# production for 2010 data (3.5 TeV beams, nu=0.14, no spill-over)
#
# Syntax is:
#   gaudirun.py Gauss-2010.py <someInputJobConfiguration>.py
##############################################################################

#--Pick beam conditions as set in AppConfig
from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam3500GeV-md100-2010-nu0.14.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2010.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")
importOptions("$APPCONFIGOPTS/Gauss/NoPacking.py")

#--Set database tags
from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20160318"
LHCbApp().CondDBtag = "sim-20160614-0-vc-md100"

