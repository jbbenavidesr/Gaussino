"""
Utilities to configure the Simulation step of Gaussino
"""
from Gaudi.Configuration import ConfigurableUser, Configurable, ApplicationMgr
from Gaussino.Utilities import gigaService
from Gaussino.SimUtils import configure_giga_alg, append_truth_actions


class SimPhase(ConfigurableUser):

    """Configurable for the Simulation phase in Gaussino. Does not implement
    a self.__apply_configuration__ itself. Instead, all member functions are
    explicitly called during the configuration of Gaussino()"""

    __slots__ = {
        "DebugCommunication": False,
        "TrackTruth": True
    }

    def __init__(self, name=Configurable.DefaultName, **kwargs):
        kwargs["name"] = name
        super(SimPhase, self).__init__(*(), **kwargs)

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

    def configure_phase(self):

        gigaService(debugcommunication=self.getProp('DebugCommunication'))

        giga_alg = configure_giga_alg()

        seq = []
        seq += [giga_alg]
        ApplicationMgr().TopAlg += seq
        if self.getProp('TrackTruth'):
            if self.getProp('DebugCommunication'):
                append_truth_actions(OutputLevel=-10)
            else:
                append_truth_actions()
