from __future__ import print_function
from Gaudi.Configuration import Configurable, log


def run_once(func):
    def decorated(*args, **kwargs):
        if not hasattr(func, 'store'):
            func.store = {}
            if 'name' in kwargs:
                key = kwargs['name']
            else:
                key = Configurable.DefaultName
            if key not in func.store:
                func.store[key] = func(*args, **kwargs)
            return func.store[key]
    return decorated


@run_once
def ppService(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, LHCb__ParticlePropertySvc
    log.info("Configuring ParticlePropertySvc")
    ppservice = LHCb__ParticlePropertySvc(
        name,
        ParticlePropertiesFile="$GAUSSINOROOT/data/ParticleTable.txt")
    ApplicationMgr().ExtSvc += [ppservice]
    return ppservice


@run_once
def dataService(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, EventDataSvc
    datasvc = EventDataSvc("EventDataSvc")
    datasvc.ForceLeaves = True
    datasvc.RootCLID = 1
    log.info("Configuring EventDataSvc")
    ApplicationMgr().ExtSvc += [datasvc]
    return datasvc


@run_once
def beaminfoService(name=Configurable.DefaultName):
    from Configurables import BeamInfoSvc, ApplicationMgr
    log.info("Configuring BeamInfoSvc")
    svc = BeamInfoSvc()

    from GaudiKernel import SystemOfUnits
    beamMom = 6.5*SystemOfUnits.TeV
    xAngle = -0.395*SystemOfUnits.mrad
    yAngle = 0.000*SystemOfUnits.mrad
    xAngleBeamLine, yAngleBeamLine = [0.0, 0.0]
    emittance = 0.0050*SystemOfUnits.mm
    betaStar = 3.0*SystemOfUnits.m
    lumiPerBunch = 0.177*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
    totCrossSection = 101.5*SystemOfUnits.millibarn
    meanX, meanY, meanZ = [0.788*SystemOfUnits.mm,
                           0.091*SystemOfUnits.mm,
                           8.64*SystemOfUnits.mm]
    sigmaS = 55.58*SystemOfUnits.mm

    # svc.BeamEnergy = beamMom
    # svc.HorizontalCrossingAngle = xAngle
    # svc.VerticalCrossingAngle = yAngle
    # svc.NormalizedEmittance = emittance
    # svc.BetaStar = betaStar
    # svc.HorizontalBeamlineAngle = xAngleBeamLine
    # svc.VerticalBeamlineAngle = yAngleBeamLine
    # svc.Luminosity = lumiPerBunch
    # svc.TotalCrossSection = totCrossSection
    # svc.XLuminousRegion = meanX
    # svc.YLuminousRegion = meanY
    # svc.ZLuminousRegion = meanZ
    # svc.BunchLengthRMS = sigmaS
    ApplicationMgr().ExtSvc += [svc]
    return svc


@run_once
def auditorService(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, AuditorSvc
    ApplicationMgr().ExtSvc += ['AuditorSvc']
    ApplicationMgr().AuditAlgorithms = True
    AuditorSvc().Auditors += ['TimingAuditor']


@run_once
def histogramService(name=Configurable.DefaultName):
    from Configurables import RootHistCnv__PersSvc
    from Gaudi.Configuration import HistogramPersistencySvc
    from GaudiKernel.ProcessJobOptions import importOptions
    from Configurables import Gaussino
    from Configurables import ApplicationMgr

    ApplicationMgr().HistogramPersistency = "ROOT";

    RootHistCnv__PersSvc(name).ForceAlphaIds = True

    histOpt = Gaussino().getProp('Histograms').upper()
    if histOpt not in ['NONE', 'DEFAULT']:
        raise RuntimeError("Unknown Histograms option '%s'" % histOpt)
        return

    if (histOpt == 'NONE'):
        log.warning("No histograms produced")
        return

    # Use a default histogram file name if not already set
    if not HistogramPersistencySvc().isPropertySet("OutputFile"):
        histosName = Gaussino().outputName() + '-histos.root'
        HistogramPersistencySvc().OutputFile = histosName


@run_once
def gigaService(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, GiGaMT
    giga = GiGaMT()
    ApplicationMgr().ExtSvc += [giga]
    return giga
