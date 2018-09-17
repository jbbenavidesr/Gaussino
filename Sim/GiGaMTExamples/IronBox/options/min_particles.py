from Configurables import MinimalNGenParticles, Generation
cuts = Generation().addTool(MinimalNGenParticles, name="MinimalNGenParticles")
Generation().FullGenEventCutTool = "MinimalNGenParticles"
cuts.NGenParticles = 1500
