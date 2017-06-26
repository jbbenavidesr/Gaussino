from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1955

importOptions("$APPCONFIGOPTS/Gauss/Sim08-Beam4000GeV-md100-2012-nu2.5.py")

from Configurables import GaudiSequencer
seqGenFSR = GaudiSequencer("GenFSRSeq")
seqGenFSR.Members += [ "GenFSRLog" ]

from Configurables import LHCbApp
LHCbApp().DDDBtag   = "dddb-20130929-1"
LHCbApp().CondDBtag = "sim-20130522-1-vc-md100"

#--Number of events
nEvts = 10
LHCbApp().EvtMax = nEvts

idFile = "genFSR_2012_Gauss_created"
OutputStream("GaussTape").Output = "DATAFILE='PFN:%s.xgen' TYP='POOL_ROOTTREE' OPT='RECREATE'" %idFile
