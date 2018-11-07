from Configurables import MinimalStableInLHCb, Generation
cuts = Generation().addTool(MinimalStableInLHCb, name="MinimalStableInLHCb")
Generation().FullGenEventCutTool = "MinimalStableInLHCb"
cuts.NGenParticles = 30
