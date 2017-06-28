# Input Histograms
from Gaudi.Configuration import importOptions
from Configurables import HistogramDataSvc
HistogramDataSvc().Input += ["GaussCalo DATAFILE='$PARAMFILESROOT/data/gausscalo.root' TYP='ROOT'"]

# Configuration of Spd sensitive detector 
importOptions("$GAUSSCALOROOT/options/GaussGeo-Spd.opts")

# Configuration of Prs sensitive detector 
importOptions("$GAUSSCALOROOT/options/GaussGeo-Prs.opts")

# Configuration of Ecal sensitive detector 
importOptions("$GAUSSCALOROOT/options/GaussGeo-Ecal.opts")

# Configuration of Hcal sensitive detector 
importOptions("$GAUSSCALOROOT/options/GaussGeo-Hcal.opts")

# Configuration Sensitive plane detector 
importOptions("$GAUSSCALOROOT/options/GaussGeo-CaloSP.opts")
