#################################################################################
## Example to run the Radiation length scan of the detector                    ##
## In this .py a geometry containing scoring planes is loaded                  ##
## and the radiation lenght tool is activated.                                 ##
## In order for this to work you also need Gauss-Job.py and MaterialEvalGun.py ##
##                                                                             ##
##  @author : K.Zarebski                                                       ##
##  @date   : last modified on 2017-10-06                                      ##
#################################################################################

from Gauss.Configuration import importOptions, appendPostConfigAction
from Configurables import CondDB, LHCbApp, Gauss
import sys

importOptions("$GAUSSROOT/options/Gauss-2011.py")
CondDB.LocalTags = {"DDDB": ["radlength-20141010", "radlength-20141003", "radlength-20140908"]}

rad_length_gauss = Gauss()

rad_length_gauss.Production = 'PGUN'
rad_length_gauss.DeltaRays = False

def scoringGeoGauss():
    from Configurables import GaussGeo
    geo = GaussGeo()
    geo.GeoItemsNames += ["/dd/Structure/LHCb/MagnetRegion/Scoring_Plane2"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/MagnetRegion/Scoring_Plane3"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane4"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane5"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane6"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/AfterMagnetRegion/Scoring_Plane7"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane8"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane9"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane10"]
    geo.GeoItemsNames += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane11"]
    

def scoringGeoGiGa():
    from Configurables import GiGaInputStream
    geo = GiGaInputStream('Geo')
    geo.StreamItems += ["/dd/Structure/LHCb/MagnetRegion/Scoring_Plane2"]
    geo.StreamItems += ["/dd/Structure/LHCb/MagnetRegion/Scoring_Plane3"]
    geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane4"]
    geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane5"]
    geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/T/Scoring_Plane6"]
    geo.StreamItems += ["/dd/Structure/LHCb/AfterMagnetRegion/Scoring_Plane7"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane8"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane9"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane10"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Scoring_Plane11"]

def choose_geo():
    if 'UseGaussGeo' in dir(rad_length_gauss):
        try:
            assert rad_length_gauss.UseGaussGeo == True
            print("Using 'GaussGeo' for Geometry Input.")
            appendPostConfigAction(scoringGeoGauss)
        except:
            print("Using 'GiGaGeo' for Geometry Input.")
            appendPostConfigAction(scoringGeoGiGa)
    
    else:
        print("'GaussGeo' not found in current Gauss version, using 'GiGaGeo' instead.")
        appendPostConfigAction(scoringGeoGiGa)

appendPostConfigAction(choose_geo)

# --- activate RadLengthColl tool
def addMyTool():
    from Configurables import GiGa, GiGaStepActionSequence
    giga = GiGa()
    giga.StepSeq.Members.append("RadLengthColl")

appendPostConfigAction(addMyTool)


def trackNeutrinos():
    from Configurables import GiGa, GiGaRunActionSequence, TrCutsRunAction
    giga = GiGa()
    giga.addTool(GiGaRunActionSequence("RunSeq"), name="RunSeq")
    giga.RunSeq.addTool(TrCutsRunAction("TrCuts"), name="TrCuts")
    giga.RunSeq.TrCuts.DoNotTrackParticles = []

appendPostConfigAction(trackNeutrinos)
