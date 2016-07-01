from Configurables import MinimumBias, Generation

Generation().addTool( MinimumBias )
Generation().MinimumBias.ProductionTool = "CRMCProduction"



from Configurables import CRMCProduction

Generation().MinimumBias.addTool(CRMCProduction, name = "CRMCProduction")

Generation().MinimumBias.CRMCProduction.SwitchOffEventTruncation = True # to use the code without limit at 10000 particles
Generation().MinimumBias.CRMCProduction.ProduceTables = False
Generation().MinimumBias.CRMCProduction.BoostAndRotate = False
Generation().MinimumBias.CRMCProduction.Frame = "nucleon-nucleon"
#To define a user impact parameter range
#Generation().MinimumBias.CRMCProduction.ImpactParameter = True
#Generation().MinimumBias.CRMCProduction.MinImpactParameter = 9.;
#Generation().MinimumBias.CRMCProduction.MaxImpactParameter = 20.;
#To Add User Settings to the Default configuration of EPOS
Generation().MinimumBias.CRMCProduction.AddUserSettingsToDefault = True
Generation().MinimumBias.CRMCProduction.Commands +=[
"fdpmjetpho dat $CRMC_TABS/phojet_fitpar.dat",
"fdpmjet dat $CRMC_TABS/dpmjet.dat",
"fqgsjet dat $CRMC_TABS/qgsjet.dat",
"fqgsjet ncs $CRMC_TABS/qgsjet.ncs",
"fqgsjetII03 dat $CRMC_TABS/qgsdat-II-03.lzma",
"fqgsjetII03 ncs $CRMC_TABS/sectnu-II-03",
"fqgsjetII dat $CRMC_TABS/qgsdat-II-04.lzma",
"fqgsjetII ncs $CRMC_TABS/sectnu-II-04",
"fname check  none",
"fname initl  $CRMC_TABS/epos.initl",
"fname iniev  $CRMC_TABS/epos.iniev",
"fname inirj  $CRMC_TABS/epos.inirj",
"fname inics  $CRMC_TABS/epos.inics",
"fname inihy  $CRMC_TABS/epos.inihy"
]

