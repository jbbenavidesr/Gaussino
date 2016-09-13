from Configurables import MinimumBias, Generation
Generation().addTool( MinimumBias )
Generation().MinimumBias.ProductionTool = "CRMCProduction"

from Configurables import CRMCProduction
Generation().MinimumBias.addTool(CRMCProduction, name = "CRMCProduction")

Generation().MinimumBias.CRMCProduction.ImpactParameter = True
Generation().MinimumBias.CRMCProduction.MinImpactParameter = 8.;
Generation().MinimumBias.CRMCProduction.MaxImpactParameter = 22.;
