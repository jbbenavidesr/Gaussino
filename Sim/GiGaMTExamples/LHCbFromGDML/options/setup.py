from multiprocessing import cpu_count
from Configurables import GiGaMT, ApplicationMgr
giga = GiGaMT()

# Set of IronBox detector construction
from Configurables import GDMLConstructionFactory
giga.DetectorConstruction = "GDMLConstructionFactory"
dettool = giga.addTool(GDMLConstructionFactory,
                       "GDMLConstructionFactory")
dettool.GDML = "$LHCBFROMGDMLROOT/data/LHCb.gdml"
