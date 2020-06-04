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


def get_set_configurable(parent, propertyname, value=""):
    if value == "":
        propertyvalue = parent.getProp(propertyname)
        try:
            objectname = propertyvalue.getType()
            propertyvalue_short = propertyvalue.getName()
        except:
            try:
                propertyvalue_short = propertyvalue.split('/')[-1]
                objectname = propertyvalue.split('/')[0]
            except:
                pass
    else:
        parent.setProp(propertyname, value)
        propertyvalue_short = value.split('/')[-1]
        objectname = value.split('/')[0]

    propertyvalue_short = propertyvalue_short.split('.')[-1]
    objectname = objectname.replace('::', '__')
    if not hasattr(parent, propertyvalue_short):
        import Configurables
        conf = getattr(Configurables, objectname)
        child = parent.addTool(conf, propertyvalue_short)
    else:
        child = getattr(parent, propertyvalue_short)
    return child


@run_once
def ppService(table, name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, LHCb__ParticlePropertySvc
    log.info("Configuring ParticlePropertySvc")
    ppservice = LHCb__ParticlePropertySvc(
        name,
        ParticlePropertiesFile=table)
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

    from Gaussino.Generation import GenPhase

    beamMom = GenPhase().getProp("BeamMomentum")
    xAngle = GenPhase().getProp("BeamHCrossingAngle")
    yAngle = GenPhase().getProp("BeamVCrossingAngle")
    xAngleBeamLine, yAngleBeamLine = GenPhase().getProp("BeamLineAngles")
    emittance = GenPhase().getProp("BeamEmittance")
    betaStar = GenPhase().getProp("BeamBetaStar")
    lumiPerBunch = GenPhase().getProp("Luminosity")
    totCrossSection = GenPhase().getProp("TotalCrossSection")
    meanX, meanY, meanZ = GenPhase().getProp("InteractionPosition")
    sigmaS = GenPhase().getProp("BunchRMS")
    # b2Mom = GenPhase().getProp("B2Momentum")
    # B1Particle = GenPhase().getProp("B1Particle")
    # B2Particle = GenPhase().getProp("B2Particle")

    svc.BeamEnergy = beamMom
    svc.HorizontalCrossingAngle = xAngle
    svc.VerticalCrossingAngle = yAngle
    svc.NormalizedEmittance = emittance
    svc.BetaStar = betaStar
    svc.HorizontalBeamlineAngle = xAngleBeamLine
    svc.VerticalBeamlineAngle = yAngleBeamLine
    svc.Luminosity = lumiPerBunch
    svc.TotalCrossSection = totCrossSection
    svc.XLuminousRegion = meanX
    svc.YLuminousRegion = meanY
    svc.ZLuminousRegion = meanZ
    svc.BunchLengthRMS = sigmaS
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
    from Configurables import Gaussino
    from Configurables import ApplicationMgr

    ApplicationMgr().HistogramPersistency = "ROOT"

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
def gigaService(name=Configurable.DefaultName, debugcommunication=False):
    from Configurables import ApplicationMgr, GiGaMT
    from Gaussino.Simulation import SimPhase
    giga = GiGaMT()
    if debugcommunication:
        conf = get_set_configurable(giga, 'WorkerPilotFactory')
        conf.PrintMCTruth = True
        conf.OutputLevel = -10

    actioninit = get_set_configurable(giga, 'ActionInitializer')
    from Configurables import GiGaRunActionCommand
    actioninit.RunActions += ['GiGaRunActionCommand']
    commands = actioninit.addTool(
        GiGaRunActionCommand,
        "GiGaRunActionCommand")
    commands.BeginOfRunCommands = SimPhase().getProp('G4BeginRunCommand')
    commands.EndOfRunCommands = SimPhase().getProp('G4EndRunCommand')
    ApplicationMgr().ExtSvc += [giga]
    return giga


def configure_edm_conversion(redecay=False, **kwargs):
    """Simple utility function to create and configure the
    EDM conversion algorithms

    :**kwargs: Optional keyword arguments (not curently used)
    :returns: GenMonitorAlg instance

    """
    from Configurables import CheckMCStructure, MCTruthMonitor, Gaussino
    if redecay:
        from Configurables import ReDecayMCTruthToEDM
        conv = ReDecayMCTruthToEDM()
    else:
        from Configurables import MCTruthToEDM
        conv = MCTruthToEDM()
    return [conv, CheckMCStructure(),
            MCTruthMonitor("MainMCTruthMonitor", HistoProduce=True)]


@run_once
def redecayService(name=Configurable.DefaultName, debugcommunication=False):
    from Configurables import ApplicationMgr, ReDecaySvc
    from Configurables import Gaussino
    redecaysvc = ReDecaySvc()
    redecaysvc.EvtMax = Gaussino().getProp('EvtMax')
    ApplicationMgr().ExtSvc += [redecaysvc]
    return redecayService
