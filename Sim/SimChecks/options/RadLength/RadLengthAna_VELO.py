#################################################################################
## Example to run the Radiation lenght scan of the detector                    ##
## In this .py a geometry containing scoring planes is loaded                  ##
## and the radiation lenght tool is activated.                                 ##
## In order for this to work you also need Gauss-Job.py and MaterialEvalGun.py ##
##                                                                             ##
##  @author : K.Zarebski                                                       ##
##  @date   : last modified on 2017-06-09                                      ##
#################################################################################

from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import CondDB, LHCbApp
#from Configurables import DDDBConf, CondDBAccessSvc
#DDDBConf(DbRoot = "myDDDB/lhcb.xml")

importOptions("$GAUSSROOT/options/Gauss-2011.py")
CondDB.LocalTags = {"DDDB": ["radlength-20141010", "radlength-20141003", "radlength-20140908"]}

from Configurables import Gauss
from Gauss.Configuration import *
import sys

pwd = os.getcwd()

rad_length_velo_gauss = Gauss()

rad_length_velo_gauss.Production = 'PGUN'
rad_length_velo_gauss.DeltaRays = False

rad_length_velo_gauss.DetectorGeo = {"Detectors": ['PuVeto', 'Velo']}
rad_length_velo_gauss.DetectorSim = {"Detectors": ['PuVeto', 'Velo']}
rad_length_velo_gauss.DetectorMoni = {"Detectors": ['PuVeto', 'Velo']}


def scoringGeoGiGa():
    from Configurables import GiGaInputStream
    geo = GiGaInputStream('Geo')

    geo.StreamItems += ["/dd/Structure/LHCb/BeforeMagnetRegion/Scoring_Plane1"]

def scoringGeoGauss():
    from Configurables import GaussGeo
    geo = GaussGeo()
    geo.GeoItemsNames += ["/dd/Structure/LHCb/BeforeMagnetRegion/Scoring_Plane1"]

def choose_geo():
    if 'UseGaussGeo' in dir(rad_length_velo_gauss):
        try:
            assert rad_length_velo_gauss.UseGaussGeo == True
            print("Using 'GaussGeo' for Geometry Input.")
            appendPostConfigAction(scoringGeoGauss)
        except:
            print("Using 'GiGaGeo' for Geometry Input.")
            appendPostConfigAction(scoringGeoGiGa)

    else:
        print("'GaussGeo' not found in current Gauss version, using 'GiGaGeo' instead.")
        appendPostConfigAction(scoringGeoGiGa)

appendPostConfigAction(choose_geo)

# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += ["NTupleSvc"]

NTupleSvc().Output = ["FILE2 DATAFILE='%s/Rad_length/root_files/Rad_VELO.root' TYP='ROOT' OPT='NEW'" % pwd]


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
