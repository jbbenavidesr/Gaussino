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
        "TrackTruth": True,
        "G4BeginRunCommand":
        ["/tracking/verbose 0", "/process/eLoss/verbose 0"],
        "G4EndRunCommand": [],
        "ExternalDetectorEmbedder": "",
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
        seq = []
        gigaService(debugcommunication=self.getProp('DebugCommunication'))

        giga_alg = configure_giga_alg()
        seq += [giga_alg]

        geo_algs = self.set_base_detector_geometry()
        seq += geo_algs

        ApplicationMgr().TopAlg += seq
        if self.getProp('TrackTruth'):
            if self.getProp('DebugCommunication'):
                append_truth_actions(OutputLevel=-10)
            else:
                append_truth_actions()

    def set_base_detector_geometry(self):
        from Configurables import GiGaMT, GiGaMTDetectorConstructionFAC
        giga = GiGaMT()
        algs = []
        dettool = giga.addTool(GiGaMTDetectorConstructionFAC,
                               "GiGaMTDetectorConstructionFAC")

        # Add external detectors geometries
        # TODO: external geometry was prepared to operate with spillover
        # but it is not available yet
        # so for now there are no 'slot' param in the algos
        embedder_name = self.getProp("ExternalDetectorEmbedder")
        if embedder_name:
            from Configurables import ExternalDetectorEmbedder
            embedder = ExternalDetectorEmbedder(embedder_name)
            embedder.embed(dettool)
            algs += embedder.activate_hits_alg()  # no slot for now!
            algs += embedder.activate_moni_alg()  # no slot for now!
        return algs
