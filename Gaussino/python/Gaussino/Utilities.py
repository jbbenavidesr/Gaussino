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
