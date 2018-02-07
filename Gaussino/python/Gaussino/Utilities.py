from __future__ import print_function
from Gaudi.Configuration import Configurable


def run_once(func):
    def decorated(*args, **kwargs):
        if not hasattr(func, 'store'):
            func.store = {}
            if 'name' in kwargs:
                key = name
            else:
                key = Configurable.DefaultName
            if key not in func.store:
                func.store[key] = func(*args, **kwargs)
            return func.store[key]
    return decorated


@run_once
def configure_ppservice(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, LHCb__ParticlePropertySvc
    print("Configuring ParticlePropertySvc")
    ppservice = LHCb__ParticlePropertySvc(
        name,
        ParticlePropertiesFile="$GAUSSINOROOT/data/ParticleTable.txt")
    ApplicationMgr().ExtSvc += [ppservice]
    return ppservice


@run_once
def configure_dataservice(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, EventDataSvc
    datasvc = EventDataSvc("EventDataSvc")
    datasvc.ForceLeaves = True
    datasvc.RootCLID = 1
    print("Configuring EventDataSvc")
    ApplicationMgr().ExtSvc += [datasvc]
    return datasvc


@run_once
def configure_beaminfoservice(name=Configurable.DefaultName):
    from Configurables import BeamInfoSvc, ApplicationMgr
    print("Configuring BeamInfoSvc")
    svc = BeamInfoSvc()
    ApplicationMgr().ExtSvc += [svc]
    return svc
