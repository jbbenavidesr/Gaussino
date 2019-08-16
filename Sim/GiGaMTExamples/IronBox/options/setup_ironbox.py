from multiprocessing import cpu_count
from Configurables import GiGaMT, ApplicationMgr
giga = GiGaMT()
giga.NumberOfWorkerThreads = cpu_count()

# Set of IronBox detector construction
from Configurables import GiGaMTProxyDetectorConstructionFAC, IronBoxGeo
giga.DetectorConstruction = "GiGaMTProxyDetectorConstructionFAC"
dettool = giga.addTool(GiGaMTProxyDetectorConstructionFAC,
                       "GiGaMTProxyDetectorConstructionFAC")
dettool.GiGaMTGeoSvc = "IronBoxGeo"
dettool.OutputLevel = -10
ApplicationMgr().ExtSvc += [IronBoxGeo()]
