"""
High level configuration tools for Gaussino
"""

__author__ = "Dominik Muller <dominik.muller@cern.ch>"


from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from Gaussino.Utilities import configure_ppservice, configure_dataservice
from Gaussino.Generation import GenPhase

# ----------------------------------------------------------------------------------


class Gaussino(ConfigurableUser):

    __used_configurables__ = [GenPhase]

    ## Steering options
    __slots__ = {
        "Histograms"                    : "DEFAULT"
        ,"DatasetName"                  : "Gaussino"
        ,"DataType"                     : ""
        # Simple lists of sub detectors
        ,"SpilloverPaths"               : []
        ,"Phases"                       : ["Generator","Simulation"] # The Gauss phases to include in the SIM file
        ,"OutputType"                   : 'SIM'
        ,"EnablePack"                   : True
        ,"DataPackingChecks"            : True
        ,"WriteFSR"                     : True
        ,"MergeGenFSR"                  : False
        ,"Debug"                        : False
        ,"BeamPipe"                     : "BeamPipeOn" # _beamPipeSwitch = 1
        ,"ReplaceWithGDML"              : [ { "volsToReplace": [], "gdmlFile" : "" } ]
        ,"RandomGenerator"              : 'Ranlux'
        , "UseGaussGeo"                 : False
        , "evtMax"                      : -1
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
        # Propagate all the necessary information to the configurable for the
        # GenPhase
        configure_ppservice()
        configure_dataservice()

        self.setOtherProps(GenPhase(), ['evtMax'])
        GenPhase().configure_phase()

        ApplicationMgr().EvtMax = self.getProp('evtMax')
        ApplicationMgr().EvtSel = 'NONE'
