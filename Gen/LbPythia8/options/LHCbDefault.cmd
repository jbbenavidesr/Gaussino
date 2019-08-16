#################################
# the default settings of pythia8
#################################
# -------------------------------
# Beam settings
# -------------------------------
Beams:frameType = 3
Beams:idA = 2212
Beams:idB = 2212
Beams:allowMomentumSpread = on
# -------------------------------
# Particle Decays and mixing
# -------------------------------
ParticleDecays:mixB = off
# HadronLevel:Decay = off
# -------------------------------
# control verbosity
# -------------------------------
Init:showAllSettings = off                   
Init:showMultipartonInteractions = off 
Init:showProcesses = off 
Init:showChangedSettings = off
Init:showChangedParticleData = off
Next:numberShowInfo = 0 
Next:numberShowProcess = 0 
Next:numberShowEvent = 0
Next:numberCount = 0
# -------------------------------
# process selection
# -------------------------------
SoftQCD:all = on
# -------------------------------
# onia tuning
# -------------------------------
LhcbHooks:pT0Ref = 1.43
LhcbHooks:ecmRef = 13000
LhcbHooks:ecmPow = 0.09
Bottomonium:O(3S1)[3S1(1)] = 9.28,4.63,3.54
Bottomonium:O(3S1)[3S1(8)] = 0.15,0.045,0.075
Bottomonium:O(3S1)[1S0(8)] = 0.02,0.006,0.01
Bottomonium:O(3S1)[3P0(8)] = 0.02,0.006,0.01
# -------------------------------
# couplings and scales
# -------------------------------
SigmaProcess:alphaSorder = 2
# -------------------------------
# pdf selection
# be carefull it changes (behind the scene) pythia8 settings
# -------------------------------
PDF:pSet = LHAPDF6:CT09MCS/0
# -------------------------------
# Tuning for pythia8 and ct09mcs: Sim10 Lambda0 tune
# -------------------------------
SpaceShower:rapidityOrder = off      # General
MultipartonInteractions:bProfile = 1 # reset the default bProfile
SpaceShower:alphaSvalue             = 0.130
MultipartonInteractions:alphaSvalue = 0.130
MultipartonInteractions:ecmRef      = 7000
MultipartonInteractions:pT0Ref      = 2.4188 # Multiparton interactions
MultipartonInteractions:ecmPow      = 0.238
# -------------------------------
# flavour selection and excited states with color reconnection
# with setting for light flavour as determined in LHCb Sim10 Lambda0 Tune
# mesonSvector, probQQtoQ, probStoUD
# -------------------------------
BeamRemnants:remnantMode = 1
ColourReconnection:mode  = 1
ColourReconnection:lambdaForm = 0
ColourReconnection:allowDoubleJunRem = off
ColourReconnection:timeDilationPar = 1.9667
ColourReconnection:m0              = 0.9021
ColourReconnection:junctionCorrection = 0.8399
StringFlav:mesonUDvector = 0.6
StringFlav:mesonSvector = 0.3819
StringFlav:mesonCvector = 3.0
StringFlav:mesonBvector = 3.0
StringFlav:probQQtoQ    = 0.1086
StringFlav:probStoUD    = 0.3590
StringFlav:probSQtoQQ   = 0.6451
StringFlav:probQQ1toQQ0 = 0.05
StringFlav:mesonUDL1S0J1 = 0.0989
StringFlav:mesonUDL1S1J0 = 0.0132
StringFlav:mesonUDL1S1J1 = 0.0597
StringFlav:mesonUDL1S1J2 = 0.0597
StringFlav:mesonSL1S0J1 = 0.0989
StringFlav:mesonSL1S1J0 = 0.0132
StringFlav:mesonSL1S1J1 = 0.0597
StringFlav:mesonSL1S1J2 = 0.0597
StringFlav:mesonCL1S0J1 = 0.0990
StringFlav:mesonCL1S1J0 = 0.0657
StringFlav:mesonCL1S1J1 = 0.2986
StringFlav:mesonCL1S1J2 = 0.2986
StringFlav:mesonBL1S0J1 = 0.0990
StringFlav:mesonBL1S1J0 = 0.0657
StringFlav:mesonBL1S1J1 = 0.2986
StringFlav:mesonBL1S1J2 = 0.2986
StringFlav:etaSup = 1.
StringFlav:etaPrimeSup = 0.4
# -------------------------------
# cross sections tuning (as in pythia6 default)
# -------------------------------
#SigmaProcess:renormMultFac = 0.075 
#BeamRemnants:primordialKThard = 1.0
# -------------------------------
# Lund fragmentation tuning (as in pythia6 default)
# -------------------------------
#StringFragmentation:stopMass = 0.4
#StringZ:aLund = 0.3 
#StringZ:bLund = 0.58 
#StringZ:rFactB = 1.
#StringPT:sigma = 0.36
