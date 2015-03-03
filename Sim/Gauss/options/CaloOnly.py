## Options to switch off all geometry (and related simulation)
## but for that of calorimeters area.  But leave containers in output.
##
## Author: G.Corti
## Date:   2009-09-16
##

from Gaudi.Configuration import *

def onlyCaloGeometry():
    
    from Configurables import GiGaInputStream

    # Define only the Calo geometry
    ## geo = GiGaInputStream('Geo')
    ## geo.StreamItems  = ["/dd/Structure/LHCb/DownstreamRegion/Spd"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Converter"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Prs"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Ecal"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Hcal"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeDownstream"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeSupportsDownstream"]
    ## geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeBakeoutDownstream"]

    from Configurables import SimulationSvc
    SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml" 

from Configurables import Gauss
Gauss().DetectorGeo = { "Detectors": [ 'Spd' , 'Prs' , 'Ecal' , 'Hcal', 'Magnet' ] }
Gauss().DetectorSim = { "Detectors": [ 'Spd' , 'Prs' , 'Ecal' , 'Hcal', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": [ 'Spd' , 'Prs' , 'Ecal' , 'Hcal', 'Magnet' ] }
Gauss().BeamPipe     = "BeamPipeInDet"

appendPostConfigAction(onlyCaloGeometry)
