from Gaudi.Configuration import *
from Gauss.Configuration import *


from Configurables import MuonHitChecker

MuonHitCheckerTest = MuonHitChecker("MuonHitCheckerTest")
MuonHitCheckerTest.HistoDir = "MuonHitChecker/MuonHitCheckerTest"
MuonHitCheckerTest.OutputLevel = INFO

from Configurables import MuonMultipleScatteringChecker

MuonMultipleScatteringTest = MuonMultipleScatteringChecker("MuonMultipleScatteringTest")
MuonMultipleScatteringTest.HistoDir = "MuonMultipleScatteringChecker/MuonMultipleScatteringTest"
MuonMultipleScatteringTest.OutputLevel = INFO

GaudiSequencer( "DetectorsMonitor" ).Members += [ MuonHitCheckerTest,MuonMultipleScatteringTest]

HistogramPersistencySvc().OutputFile ='MuonMoniSim_histos.root'
