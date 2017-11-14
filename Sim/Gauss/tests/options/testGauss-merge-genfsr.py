from Gauss.Configuration import *

from Configurables import GaudiSequencer
seqGenFSR = GaudiSequencer("GenFSRSeq")
seqGenFSR.Members += ["GenFSRMerge"]
seqGenFSR.Members += ["GenFSRLog"]

ApplicationMgr().TopAlg += [seqGenFSR]

from Configurables import LHCbApp
LHCbApp(DataType = "2012", Simulation = True)

EventSelector().Input += [
    "DATAFILE='root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/swtest/genFSR-2012/Gauss-13114005-test1.sim' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/swtest/genFSR-2012/Gauss-13114005-test2.sim' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:genFSR_2012_Gauss_created.xgen' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

LHCbApp().EvtMax = -1
LHCbApp().Simulation = True
    
