###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
"""
High level configuration tools for Gaussino
"""

__author__ = "Dominik Muller <dominik.muller@cern.ch>"

from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from Gaudi.Configuration import log
from Gaussino.Utilities import (ppService, dataService, redecayService,
                                auditorService, histogramService)
from Gaussino.Utilities import configure_edm_conversion
from Gaussino.Generation import GenPhase
from Gaussino.Simulation import SimPhase


class Gaussino(ConfigurableUser):
    """Main Configurable of Gaussino.

    .. warning::
        Some of these options have to be revisited as they
        might be inactive or redundant.

    :var Histograms: default: ``"DEFAULT"``
    :vartype Histograms: str, optional

    :var DatasetName: default: ``"Gaussino"``
    :vartype DatasetName: str, optional

    :var DatasetNameForced: default: ``False``
    :vartype DatasetNameForced: bool, optional

    :var DataType: default: ``""``
    :vartype DataType: str, optional

    :var SpilloverPaths: default: ``[]``
    :vartype SpilloverPaths: list, optional

    :var Phases: default: ``["Generator","Simulation"]``
    :vartype Phases: list, optional

    :var OutputType: default: ``'SIM'``
    :vartype OutputType: str, optional

    :var EnablePack: default: ``True``
    :vartype EnablePack: bool, optional

    :var DataPackingChecks: default: ``True``
    :vartype DataPackingChecks: bool, optional

    :var WriteFSR: default: ``True``
    :vartype WriteFSR: bool, optional

    :var MergeGenFSR: default: ``False``
    :vartype MergeGenFSR: bool, optional

    :var Debug: default: ``False``
    :vartype Debug: bool, optional

    :var BeamPipe: default: ``"BeamPipeOn"``
    :vartype BeamPipe: str, optional

    :var ReplaceWithGDML: default:
        ``[ { "volsToReplace": [], "gdmlFile": "" } ]``
    :vartype ReplaceWithGDML: list, optional

    :var RandomGenerator: default: ``'Ranlux'``
    :vartype RandomGenerator: str, optional

    :var EvtMax: default: ``-1``
    :vartype EvtMax: int, optional

    :var EnableHive: default: ``True``
    :vartype EnableHive: bool, optional

    :var ReDecay: default: ``False``
    :vartype ReDecay: bool, optional

    :var ThreadPoolSize: default: ``1``
    :vartype ThreadPoolSize: int, optional

    :var EventSlots: default: ``1``
    :vartype EventSlots: int, optional

    :var ConvertEDM: default: ``False``
    :vartype ConvertEDM: bool, optional

    :var ForceRandomEngine: default: ``'NONE'``
    :vartype ForceRandomEngine: str, optional

    :var ParticleTable: default: ``'$GAUSSINOROOT/data/ParticleTable.txt'``
    :vartype ParticleTable: str, optional
    """
    __used_configurables__ = [GenPhase]

    __slots__ = {
        "Histograms":
        "DEFAULT"  # NOQA
        ,
        "DatasetName":
        "Gaussino"  # NOQA
        ,
        "DatasetNameForced":
        False  # NOQA
        ,
        "DataType":
        ""  # NOQA
        ,
        "SpilloverPaths": []  # NOQA
        ,
        "Phases":
        ["Generator",
         "Simulation"]  # The Gauss phases to include in the SIM file  # NOQA
        ,
        "OutputType":
        'SIM'  # NOQA
        ,
        "EnablePack":
        True  # NOQA
        ,
        "DataPackingChecks":
        True  # NOQA
        ,
        "WriteFSR":
        True  # NOQA
        ,
        "MergeGenFSR":
        False  # NOQA
        ,
        "Debug":
        False  # NOQA
        ,
        "BeamPipe":
        "BeamPipeOn"  # _beamPipeSwitch = 1  # NOQA
        ,
        "ReplaceWithGDML": [{
            "volsToReplace": [],
            "gdmlFile": ""
        }]  # NOQA
        ,
        "RandomGenerator":
        'Ranlux'  # NOQA
        ,
        "EvtMax":
        -1  # NOQA
        ,
        "EnableHive":
        True  # NOQA
        ,
        "ReDecay":
        False  # NOQA
        ,
        "ThreadPoolSize":
        1  # NOQA
        ,
        "EventSlots":
        1  # NOQA
        ,
        "ConvertEDM":
        False  # NOQA
        ,
        "ForceRandomEngine":
        'NONE'  # NOQA
        ,
        "ParticleTable":
        "$GAUSSINOROOT/data/ParticleTable.txt"  # NOQA
    }

    def __init__(self, name=Configurable.DefaultName, **kwargs):
        kwargs["name"] = name
        super(Gaussino, self).__init__(*(), **kwargs)

    def setOtherProp(self, other, name):
        """Set the given property in another configurable object

        :param other: The other configurable to set the property for
        :param name:  The property name
        """
        self.propagateProperty(name, other)

    def setOtherProps(self, other, names):
        """ Set the given properties in another configurable object

        :param other: The other configurable to set the property for
        :param names: The property names
        """
        self.propagateProperties(names, other)

    def setupHive(self):
        '''Enable Hive event loop manager
           this is a very similar method as in LHCbApp
        '''
        if not self.getProp("EnableHive"):
            # FIXME: Running without GaudiHive has not been tested
            #        and may lead to unexpected behaviour
            #        this is disabled for now
            log.error("EnableHive must be set. Running without "
                      "GaudiHive has not been tested and may lead to"
                      "unexpected behaviour")
            raise ValueError("EnableHive must be set.")
        from Configurables import HiveWhiteBoard
        whiteboard = HiveWhiteBoard("EventDataSvc")
        whiteboard.EventSlots = self.getProp('EventSlots')
        ApplicationMgr().ExtSvc.insert(0, whiteboard)

        from Configurables import HiveSlimEventLoopMgr, AvalancheSchedulerSvc
        scheduler = AvalancheSchedulerSvc()
        eventloopmgr = HiveSlimEventLoopMgr(SchedulerName=scheduler)

        # initialize hive settings if not already set
        self.propagateProperty('ThreadPoolSize', eventloopmgr)
        scheduler.ThreadPoolSize = self.getProp('ThreadPoolSize')
        ApplicationMgr().EventLoop = eventloopmgr
        # appendPostConfigAction(self.co)

    def __apply_configuration__(self):
        self.setupHive()
        ppService(self.getProp('ParticleTable'))
        dataService()
        auditorService()
        if (self.getProp("ReDecay")):
            redecayService()

        phases = self.getProp("Phases")
        if "Generator" not in phases:
            raise Exception("Must have Generator phase")
        self.setOtherProps(GenPhase(), ['EvtMax'])
        GenPhase().configure_phase()
        if "Simulation" in phases:
            SimPhase().configure_phase()
        else:
            GenPhase().configure_genonly()

        if self.getProp('ConvertEDM'):
            ApplicationMgr().TopAlg += configure_edm_conversion(
                self.getProp("ReDecay"))

        histogramService()

        ApplicationMgr().EvtMax = self.getProp('EvtMax')
        ApplicationMgr().EvtSel = 'NONE'

        from Gaudi.Configuration import appendPostConfigAction

        def force_engine():
            from Gaudi import Configuration
            neweng = self.getProp('ForceRandomEngine')
            for name, conf in Configuration.allConfigurables.items():
                try:
                    conf.setProp('RandomEngine', neweng)
                except:
                    pass
                else:
                    log.info('Forced random engine of {} to {}'.format(
                        name,
                        neweng,
                    ))

        if self.getProp('ForceRandomEngine') != 'NONE':
            appendPostConfigAction(force_engine)

    eventType = staticmethod(GenPhase.eventType)

    def outputName(self):
        """
        Build a name for the output file, based on input options.
        Combines DatasetName, EventType, Number of events and Date
        """
        import time
        outputName = self.getProp("DatasetName")
        if self.getProp("DatasetNameForced"):
            return outputName
        if outputName == "":
            outputName = 'Gaussino'
        if self.eventType() != "":
            if outputName != "":
                outputName += '-'
            outputName += self.eventType()
        if self.EvtMax > 0:
            outputName += '-' + str(self.EvtMax) + 'ev'
        idFile = str(time.localtime().tm_year)
        if time.localtime().tm_mon < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mon)
        if time.localtime().tm_mday < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mday)
        outputName += '-' + idFile
        return outputName

    # hack from MiniBrunel configuration
    def co(self):
        from Gaudi.Configuration import allConfigurables
        for c in allConfigurables:
            if hasattr(c, 'ExtraInputs'
                       ) and '/Event/IOVLock' not in c.ExtraInputs:  # NOQA
                c.ExtraInputs.append('/Event/IOVLock')
