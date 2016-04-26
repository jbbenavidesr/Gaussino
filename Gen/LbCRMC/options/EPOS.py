from Configurables import MinimumBias, Generation

Generation().addTool( MinimumBias )
Generation().MinimumBias.ProductionTool = "CRMCProduction"



from Configurables import CRMCProduction

Generation().MinimumBias.addTool(CRMCProduction, name = "CRMCProduction")

Generation().MinimumBias.CRMCProduction.SwitchOffEventTruncation = True # to use the code without limit at 10000 particles
Generation().MinimumBias.CRMCProduction.EPOSParamFileName = '$LBCRMCROOT/options/crmc.param' # to use the external param file
Generation().MinimumBias.CRMCProduction.ProduceTables = False
Generation().MinimumBias.CRMCProduction.BoostAndRotate = False

