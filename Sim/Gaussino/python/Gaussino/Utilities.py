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
__author__ = "Dominik Muller, Michal Mazurek, and Gloria Corti"
__email__ = "lhcb-simulation@cern.ch"

from Gaudi.Configuration import (
    log,
    ConfigurableUser,
    Configurable,
)

from GaudiKernel.ConfigurableDb import getConfigurable


class GaussinoConfigurable(ConfigurableUser):

    _rigid = False

    def __init__(self, name=Configurable.DefaultName, _enabled=True, **kwargs):
        super(GaussinoConfigurable, self).__init__(
            name=name, _enabled=_enabled, **kwargs
        )
        self._rigid = True

    def __setattr__(self, key, value):
        if self._rigid and not hasattr(self, key) and key not in self._properties:
            msg = f"Configurable '{self.__class__.__name__}' does not have '{key}' property."
            log.error(msg)
            raise AttributeError(msg)
        super(GaussinoConfigurable, self).__setattr__(key, value)


def add_constructors_with_names(tool, joint_names):
    for joint_name in joint_names:
        if "/" in joint_name:
            template, name = joint_name.split("/")
            tool.addTool(getConfigurable(template), name=name)


# FIXME: Michal M.: moved from old SimUtils,
#        it is too convoluted and probably buggy...
def get_set_configurable(parent, propertyname, value=""):
    if value == "":
        propertyvalue = parent.getProp(propertyname)
        try:
            objectname = propertyvalue.getType()
            propertyvalue_short = propertyvalue.getName()
        except:
            try:
                propertyvalue_short = propertyvalue.split("/")[-1]
                objectname = propertyvalue.split("/")[0]
            except:
                pass
    else:
        parent.setProp(propertyname, value)
        propertyvalue_short = value.split("/")[-1]
        objectname = value.split("/")[0]

    propertyvalue_short = propertyvalue_short.split(".")[-1]
    objectname = objectname.replace("::", "__")
    if not hasattr(parent, propertyvalue_short):
        import Configurables

        conf = getattr(Configurables, objectname)
        child = parent.addTool(conf, propertyvalue_short)
    else:
        child = getattr(parent, propertyvalue_short)
    return child
