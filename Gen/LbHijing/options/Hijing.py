from Configurables import Generation, Special, HijingProduction, MinimumBias, Inclusive, SignalPlain, SignalRepeatedHadronization

gen = Generation()
gen.addTool( Special )
gen.addTool( MinimumBias )
gen.addTool( Inclusive )
gen.addTool( SignalPlain )
gen.addTool( SignalRepeatedHadronization )

gen.Special.ProductionTool = "HijingProduction"
gen.MinimumBias.ProductionTool = "HijingProduction"
gen.Inclusive.ProductionTool = "HijingProduction"
gen.SignalPlain.ProductionTool = "HijingProduction"
gen.SignalRepeatedHadronization.ProductionTool = "HijingProduction"
