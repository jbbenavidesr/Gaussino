###############################################################################
# (c) Copyright 2022 CERN for the benefit of the LHCb and FCC Collaborations  #
#                                                                             #
# This software is distributed under the terms of the Apache License          #
# version 2 (Apache-2.0), copied verbatim in the file "COPYING".              #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Gaudi.Configuration import (
    ConfigurableUser,
    log,
    DEBUG,
)

__author__ = "Filip Bilandžija, Michał Mazurek"
__email__ = "michal.mazurek@cern.ch"


class Geant4Visualization(ConfigurableUser):
    """Documentation to be filled in later...

    :var Driver: default: ``ASCIITree`` Geant4 visualization drivers.
        The list of available drivers is : ``['ASCIITree']``
    :vartype Driver: str, required

    :var DrawGeometry: default: ``True``
    :vartype DrawGeometry: bool, optional

    :var Debug: default: ``False``
    :vartype Debug: bool, optional

    """

    __slots__ = {
        "Driver": "",
        "DrawGeometry": True,
        "Debug": True,
    }

    _available_drivers = [
        'ASCIITree',
    ]

    _parsed_drivers = {
        'ASCIITree': 'ATree',
    }

    def apply(self, giga):
        """ Main method to be called from SimPhase()

        :param giga: GiGaMT configurable
        """
        if not self.getProp("Driver"):
            log.info("Geant4 visualization is turned off.")
            return

        # activate GiGaVisManager
        giga.VisManager = "GiGaVisManager"
        if self.getProp("Debug"):
            giga.VisManager.OutputLevel = DEBUG

        # access the UI interface
        actioninit = giga.ActionInitializer
        run_action = actioninit.GiGaRunActionCommand
        event_action = actioninit.GiGaEventActionCommand

        from Configurables import GiGaMTRunManagerFAC
        # this has to be accessed this way...
        run_mgr = GiGaMTRunManagerFAC("GiGaMT.GiGaMTRunManagerFAC")

        cmds = {
            # -> init commands:
            'init': run_mgr.InitCommands,
            # -> run commands:
            'begin_run': run_action.BeginOfRunCommands,
            'end_run': run_action.EndOfRunCommands,
            # -> event commands:
            'begin_event': event_action.BeginOfEventCommands,
            'end_event': event_action.EndOfEventCommands,
        }

        # and now append vis UI commands
        # note: order is important!
        self._activate_g4_driver(cmds)
        if self.getProp("DrawGeometry"):
            self._draw_geometry(cmds)

    def _activate_g4_driver(self, cmds):
        driver = self.getProp("Driver")
        if driver not in self._available_drivers:
            raise NotImplementedError(driver +
                                      "is not an available G4 driver.")
        main_cmd = '/vis/open '
        parsed_driver = self._parsed_drivers.get(driver, driver)
        main_cmd += parsed_driver
        cmds['init'] += [main_cmd]

    def _draw_geometry(self, cmds):
        cmds['init'] += [
            "/vis/drawVolume",
            "/vis/viewer/flush",  # TODO: this should be optional
        ]
