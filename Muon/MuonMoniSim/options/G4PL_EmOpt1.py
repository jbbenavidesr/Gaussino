##
##  File containing options to activate the emstandard_opt1 Electromagnetic physics list
##  using the FTFP_BERT Hadronic Physics List in Geant4 (the default for production is 
##  FTFP_BERT and EMNoCuts)
##

from Configurables import Gauss

Gauss().PhysicsList = {"Em":'Opt1', "Hadron":'FTFP_BERT', "GeneralPhys":True, "LHCbPhys":True}
