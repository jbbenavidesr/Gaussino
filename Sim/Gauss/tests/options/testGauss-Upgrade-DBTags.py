from Configurables import CondDB, Gauss
CondDB().Upgrade = True
Gauss().DataType = "Upgrade"

from Configurables import LHCbApp
LHCbApp().EvtMax    = 1
LHCbApp().DDDBtag   = "dddb-20170726"
LHCbApp().CondDBtag = "sim-20170301-vc-md100"
