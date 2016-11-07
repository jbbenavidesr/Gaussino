from Configurables import MinimumBias, Generation, CRMCProduction
from Configurables import Special, Inclusive, SignalPlain

Generation().addTool( MinimumBias )
Generation().MinimumBias.ProductionTool = "CRMCProduction"
Generation().MinimumBias.addTool(CRMCProduction)

Generation().addTool( Special )
Generation().Special.addTool( CRMCProduction )
Generation().addTool( Inclusive )
Generation().Inclusive.addTool( CRMCProduction )
Generation().addTool( SignalPlain )
Generation().SignalPlain.addTool( CRMCProduction )

Generation().MinimumBias.CRMCProduction.ImpactParameter = True
Generation().MinimumBias.CRMCProduction.MinImpactParameter = 8.
Generation().MinimumBias.CRMCProduction.MaxImpactParameter = 22.

Generation().Special.CRMCProduction.ImpactParameter = True
Generation().Special.CRMCProduction.MinImpactParameter = 8.
Generation().Special.CRMCProduction.MaxImpactParameter = 22.

Generation().Inclusive.CRMCProduction.ImpactParameter = True
Generation().Inclusive.CRMCProduction.MinImpactParameter = 8.
Generation().Inclusive.CRMCProduction.MaxImpactParameter = 22.

Generation().SignalPlain.CRMCProduction.ImpactParameter = True
Generation().SignalPlain.CRMCProduction.MinImpactParameter = 8.
Generation().SignalPlain.CRMCProduction.MaxImpactParameter = 22.
