"""
High level configuration tools for Gaussino
"""

__author__ = "Dominik Muller <dominik.muller@cern.ch>"


from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from Gaussino.Utilities import (ppService, dataService,
                                auditorService, histogramService)
from Gaussino.Generation import GenPhase


class Gaussino(ConfigurableUser):

    __used_configurables__ = [GenPhase]

    __slots__ = {
        "Histograms"                    : "DEFAULT"  # NOQA
        ,"DatasetName"                  : "Gaussino"  # NOQA
        ,"DataType"                     : ""  # NOQA
        ,"SpilloverPaths"               : []  # NOQA
        ,"Phases"                       : ["Generator","Simulation"] # The Gauss phases to include in the SIM file  # NOQA
        ,"OutputType"                   : 'SIM'  # NOQA
        ,"EnablePack"                   : True  # NOQA
        ,"DataPackingChecks"            : True  # NOQA
        ,"WriteFSR"                     : True  # NOQA
        ,"MergeGenFSR"                  : False  # NOQA
        ,"Debug"                        : False  # NOQA
        ,"BeamPipe"                     : "BeamPipeOn" # _beamPipeSwitch = 1  # NOQA
        ,"ReplaceWithGDML"              : [ { "volsToReplace": [], "gdmlFile" : "" } ]  # NOQA
        ,"RandomGenerator"              : 'Ranlux'  # NOQA
        , "UseGaussGeo"                 : False  # NOQA
        , "evtMax"                      : -1  # NOQA
      }

    def __init__(self, name=Configurable.DefaultName, **kwargs):
        kwargs["name"] = name
        super(Gaussino, self).__init__(*(), **kwargs)

    # @brief Set the given property in another configurable object
    #  @param other The other configurable to set the property for
    #  @param name  The property name
    def setOtherProp(self, other, name):
        self.propagateProperty(name, other)

    # @brief Set the given properties in another configurable object
    #  @param other The other configurable to set the property for
    #  @param names The property names
    def setOtherProps(self, other, names):
        self.propagateProperties(names, other)

    def __apply_configuration__(self):
        ppService()
        dataService()
        auditorService()

        self.setOtherProps(GenPhase(), ['evtMax'])
        GenPhase().configure_phase()

        histogramService()

        ApplicationMgr().EvtMax = self.getProp('evtMax')
        ApplicationMgr().EvtSel = 'NONE'

    @staticmethod
    def eventType():
        return GenPhase.eventType()

    def outputName(self):
        """
        Build a name for the output file, based on input options.
        Combines DatasetName, EventType, Number of events and Date
        """
        import time
        outputName = self.getProp("DatasetName")
        if outputName == "":
            outputName = 'Gaussino'
        if self.eventType() != "":
            if outputName != "":
                outputName += '-'
            outputName += self.eventType()
        if self.evtMax > 0:
            outputName += '-' + str(self.evtMax) + 'ev'
        idFile = str(time.localtime().tm_year)
        if time.localtime().tm_mon < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mon)
        if time.localtime().tm_mday < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mday)
        outputName += '-' + idFile
        return outputName
