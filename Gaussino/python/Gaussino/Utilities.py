from __future__ import print_function
from Gaudi.Configuration import Configurable


def configure_ppservice(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, LHCb__ParticlePropertySvc
    print("Configuring ParticlePropertySvc")
    ppservice = LHCb__ParticlePropertySvc(
        name,
        ParticlePropertiesFile="$GAUSSINOROOT/data/ParticleTable.txt")
    ApplicationMgr().ExtSvc += [ppservice]
    return ppservice


def configure_dataservice(name=Configurable.DefaultName):
    from Configurables import ApplicationMgr, EventDataSvc
    datasvc = EventDataSvc("EventDataSvc")
    datasvc.ForceLeaves = True;
    datasvc.RootCLID    =    1;
    print("Configuring EventDataSvc")
    ApplicationMgr().ExtSvc += [datasvc]
    return datasvc
