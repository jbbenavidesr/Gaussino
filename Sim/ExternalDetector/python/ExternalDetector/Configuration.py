###############################################################################
# (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration           #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
from Configurables import LHCbConfigurableUser
from Gaudi.Configuration import log
import Configurables


class ExternalDetectorEmbedder(LHCbConfigurableUser):

    __slots__ = {
        'Shapes': {},
        'Sensitive': {},
        'Hit': {},
        'Moni': {},
    }

    _added_dets = []
    _added_hits_algs = []

    def embed(self, geo):
        if not geo:
            raise RuntimeError("ERROR: GeoService not provided")
        for name, props in self.getProp("Shapes").items():
            self._check_props(name, props)
            tool_name = props['Type'] + 'Embedder'
            tool = self._embedding_tool(name, tool_name, props)
            geo.addTool(tool, name=name)
            geo.ExternalDetectors.append(tool_name + '/' + name)
            self._added_dets.append(name)

    def activate_hits_alg(self, slot=""):
        algs = []
        hit_algs = self.getProp('Hit')
        if type(hit_algs) is dict:
            for det_name, hit_alg_props in hit_algs.items():
                if det_name not in self._added_dets:
                    log.warning("External geometry not set for " + det_name)
                    continue
                self._check_props(det_name, hit_alg_props)
                alg_conf = getattr(Configurables, hit_alg_props['Type'])
                hit_alg_name = 'Get' + det_name + 'Hits' + slot
                alg = alg_conf(
                    hit_alg_name,
                    MCHitsLocation='MC/' + det_name + '/Hits',
                    CollectionName=det_name + 'SDet/Hits',
                    **self._refine_props(hit_alg_props))
                log.info("Registered external hit extraction " + hit_alg_name)
                self._added_hits_algs.append(det_name)
                algs.append(alg)
        return algs

    def activate_moni_alg(self, slot=""):
        algs = []
        moni_algs = self.getProp('Moni')
        if type(moni_algs) is dict:
            for det_name, moni_alg_props in moni_algs.items():
                if det_name not in self._added_hits_algs:
                    log.warning("External hit algorithm not set for " + det_name)
                    continue
                self._check_props(det_name, moni_alg_props)
                alg_conf = getattr(Configurables, moni_alg_props['Type'])
                moni_alg_name = moni_alg_props['Type'] + slot
                alg = alg_conf(
                    moni_alg_name,
                    CollectorHits='MC/' + det_name + '/Hits',
                    **self._refine_props(moni_alg_props))
                log.info("Registered external monitoring " + moni_alg_name)
                algs.append(alg)
        return algs

    def _check_props(self, name, props, required=['Type']):
        if type(props) is not dict:
            raise RuntimeError(
                "ERROR: Dictionary of {} properties not provided.".format(
                    name))
        for req in required:
            if not props.get(req):
                raise RuntimeError(
                    "ERROR: Property {} for {} not provided.".format(
                        req.lower(), name))

    def _refine_props(self, props, keys_to_refine=['Type']):
        return {
            key: prop
            for key, prop in props.items() if key not in keys_to_refine
        }

    def _register_prop(self, props, key, prop):
        if not props.get(key):
            props[key] = prop

    def _embedding_tool(self, name, tool_name, props):
        log.info("Registering external {} as {}".format(name, tool_name))
        tool_conf = getattr(Configurables, tool_name)
        sens_det_props = self.getProp('Sensitive').get(name)
        self._register_prop(props, 'LogicalVolumeName', name + "LVol")
        self._register_prop(props, 'PhysicalVolumeName', name + "PVol")
        if sens_det_props:
            self._check_props(name, sens_det_props)
            sens_det_conf = getattr(Configurables, sens_det_props['Type'])
            sens_det_name = name + 'SDet'
            self._register_prop(props, 'SensDetName',
                                sens_det_props['Type'] + '/' + sens_det_name)
            sens_det_conf(sens_det_name, **self._refine_props(sens_det_props))
        return tool_conf(name, **self._refine_props(props))
