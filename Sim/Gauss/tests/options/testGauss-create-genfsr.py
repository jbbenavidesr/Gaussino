from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1955

from Configurables import GaudiSequencer
seqGenFSR = GaudiSequencer("GenFSRSeq")
seqGenFSR.Members += ["GenFSRLog"]
    
ApplicationMgr().TopAlg += [seqGenFSR]

#--Number of events
nEvts = 10

from Configurables import LHCbApp
LHCbApp().EvtMax = nEvts
LHCbApp().Simulation = True

#--Set database tags
LHCbApp().DDDBtag   = "dddb-20170721-2"
LHCbApp().CondDBtag = "sim-20160321-2-vc-md100"

idFile = "genFSR_2012_Gauss_created"
OutputStream("GaussTape").Output = "DATAFILE='PFN:%s.xgen' TYP='POOL_ROOTTREE' OPT='RECREATE'" %idFile
