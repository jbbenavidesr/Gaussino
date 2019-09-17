from multiprocessing import cpu_count
from Configurables import GiGaMT, ApplicationMgr
giga = GiGaMT()
giga.NumberOfWorkerThreads = cpu_count()

# Set of IronBox detector construction
from Configurables import GiGaMTDetectorConstructionFAC, IronBoxGeo
giga.DetectorConstruction = "GiGaMTDetectorConstructionFAC"
dettool = giga.addTool(GiGaMTDetectorConstructionFAC,
                       "GiGaMTDetectorConstructionFAC")
dettool.GiGaMTGeoSvc = "IronBoxGeo"
dettool.OutputLevel = -10
ApplicationMgr().ExtSvc += [IronBoxGeo()]
