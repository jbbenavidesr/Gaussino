##############################################################################
# File for running Gauss with latest tags, using the default configuration
#
# Syntax is:
#   gaudirun.py Gauss-DEV.py <someInputJobConfiguration>.py
##############################################################################

from Configurables import Gauss
theApp = Gauss()

#--Use latest database tags 
from Configurables import DDDBConf, CondDB, LHCbApp

LHCbApp().DDDBtag   = "dddb-20170721-3"   # Must be set for selecting VELO
                                          # geometry list in Gauss()

Gauss().DataType  = "2016"
DDDBConf(DataType = "2016")
CondDB().UseLatestTags = ["2016"]
