# Adding a new sub-detector with Detector [TO BE MOVED]

## Preparing Gauss for adding new sub-detectors

```{eval-rst}
.. attention::
    This section will be moved to a new documentation website dedicated to Gauss.
```

Make sure that before you start this tutorial you have followed the examples that show how to build Gaussino with DD4hep support. In particular, the decision has to be made if you want work with your local copy of the Detector or the Detector project built in the nightly system:

- [](../getting_started/gauss.md#example-3-working-with-dd4hep-and-detector-from-the-nightlies)
- [](../getting_started/gauss.md#example-4-working-with-dd4hep-and-detector-built-locally)

```{eval-rst}
.. tip::
    Use ``x86_64_v2-centos7-gcc11+dd4hep-opt`` if you want to work with **Detector/DD4hep**.
```

## Checklist to activate a sub-detector

Once you have correctly configured Gauss, you can start integrating your sub-detector with the simulation framework. We have prepared a list of the required tasks:

1. [](./adding_subdetector_in_detector.md#modify-the-xml-geometry-description)
2. [](./adding_subdetector_in_detector.md#implement-python-configuration)
3. [](./adding_subdetector_in_detector.md#check-if-extra-modifications-are-needed)
4. [](./adding_subdetector_in_detector.md#create-a-mr-in-gauss-targeting-futurev4-branch)
5. [](./adding_subdetector_in_detector.md#put-the-sub-detector-in-the-nighlty-and-lhcbpr-tests)

### Modify the XML geometry description

In order to load the geometry in Gauss from the Detector project, you have to make sure that your sub-detector is completely described in XML files in `Detector/compact/trunk` directory of the Detector project. Therefore, Gauss has to know what is the hiararchy of the volumes in the geometry and this has to be implemented in [xml_writer.py](https://gitlab.cern.ch/lhcb/Gauss/-/blob/Futurev4/Sim/Gauss/python/Gauss/Geometry/xml_writer.py). We will use `VP` as an example. `VP` has to be added in `/world/BeforeMagnetRegion` and therefore the following changes

```diff
+     ET.SubElement(define, "constant", name="VP:parent",
+                   value="/world/BeforeMagnetRegion", type="string")
```

are needed in the `xml_writer.py`:

```python
# Combine the includes into a valid LHCb DD4hep xml and return name of temporary
# file
def create_xml(includesfiles):
    unique_includes = list(OrderedDict.fromkeys(includesfiles))
    detectorroot = os.path.join(os.environ['DETECTOR_PROJECT_ROOT'], 'compact/trunk/')

    def addroot(p):
        if p[0] != '.' and p[0] != '/':
            return detectorroot + p
        return p

    root = ET.Element("lccdd")
    ET.SubElement(root, "std_conditions", type="STP")
    debug = ET.SubElement(root, "debug")
    ET.SubElement(debug, "type", name="includes", value="0")
    ET.SubElement(debug, "type", name="materials", value="0")
    includes = ET.SubElement(root, "includes")
    ET.SubElement(includes, "gdmlFile", ref=addroot("defaults.xml"))
    define = ET.SubElement(root, "define")
    ET.SubElement(define, "constant", name="Bls:parent",
                  value="/world/BeforeMagnetRegion/BeforeVelo", type="string")
    ET.SubElement(define, "constant", name="BcmUp:parent",
                  value="/world/BeforeMagnetRegion/BeforeVelo", type="string")
    ET.SubElement(define, "constant", name="BcmDown:parent",
                  value="/world/DownstreamRegion/AfterMuon", type="string")
    ET.SubElement(define, "constant", name="GValve:parent",
                  value="/world/BeforeMagnetRegion/BeforeVelo", type="string")
    ET.SubElement(define, "constant", name="MBXWUp:parent",
                  value="/world/UpstreamRegion", type="string")
    ET.SubElement(define, "constant", name="Cavern:parent",
                  value="/world/Infrastructure", type="string")
    ET.SubElement(define, "constant", name="Tunnel:parent",
                  value="/world/Infrastructure", type="string")
    ET.SubElement(define, "constant", name="Bunker:parent",
                  value="/world/Infrastructure", type="string")
    # here we added VP
    ET.SubElement(define, "constant", name="VP:parent",
                  value="/world/BeforeMagnetRegion", type="string")
    ET.SubElement(define, "constant", name="UT:parent",
                  value="/world/BeforeMagnetRegion", type="string")
    ET.SubElement(define, "constant", name="FT:parent",
                  value="/world/AfterMagnetRegion/T", type="string")
    ET.SubElement(define, "constant", name="Magnet:parent",
                  value="/world/MagnetRegion", type="string")
    ET.SubElement(define, "constant", name="Magnet:ignore", value="0")
    ET.SubElement(define, "constant", name="UpstreamRegion:ignore", value="0")
    ET.SubElement(define, "constant", name="BeforeMagnetRegion:ignore", value="0")  # NOQA
    ET.SubElement(define, "constant", name="MagnetRegion:ignore", value="0")
    ET.SubElement(define, "constant", name="AfterMagnetRegion:ignore", value="0")  # NOQA
    ET.SubElement(define, "constant", name="DownstreamRegion:ignore", value="0")

    ET.SubElement(root, "include", ref=addroot("global/conditions.xml"))

    # FIXME: Regions need something from this file
    ET.SubElement(root, "include", ref=addroot("T/parameters.xml"))
    ET.SubElement(root, "include", ref=addroot("Regions/detector.xml"))

    for inc in unique_includes:
        ET.SubElement(root, "include", ref=addroot(inc))

    _, tmpfile = tempfile.mkstemp('.xml')
    xmlstr = minidom.parseString(ET.tostring(root)).toprettyxml(indent="   ")
    with open(tmpfile, "w") as f:
        f.write(xmlstr)
    log.info('Wrote xml file to {}'.format(tmpfile))
    return tempfile.tempdir, tmpfile.replace(tempfile.tempdir + '/', '')
```

A temporary XML file will be created in `/tmp` on your local machine.


### Implement python configuration


What happens in Gauss is that we use the python configuration to decide which sub-detectors should be included and which not. If a sub-detector is added, then all of its XML files have to be imported. Please note that Gauss has to support both DD4hep and DetDesc geometries and therefore we need a separate configuration for each of them. Python configuration of each sub-detector should be handled in a separate python file in `Sim/Gauss/python/Gauss/Geometry/`. We will use `VP` again as an example. In this case, a file `VP.py` is already there. If the sub-detector is completely new, you should create a new file and add the following import in [__init__.py](https://gitlab.cern.ch/lhcb/Gauss/-/blob/Futurev4/Sim/Gauss/python/Gauss/Geometry/__init__.py):

```diff
+ from Gauss.Geometry.VP import *
```   

You have to make sure that you sub-detector class inherits from `det_base` and uses the `subdetector` decorator. There is also a set of functions that have to be implemented. For `VP`:

```python
from Gauss.Geometry.det_base import det_base
from Gauss.Geometry.Helpers import subdetector

@subdetector
class VP(det_base):

    def ApplyDetectorDetDesc(self, basePieces, detPieces):
        # Add the necessary detector pieces for DetDesc
  
    def ApplyDetectorDD4hep(self, basePieces, detPieces):
        # Add a list of all the necessary DD4hep includes

    def SetupExtractionImpl(self, slot=''):
        # Configure the hit getter that transforms the Geant4 hit collection
        # to our event model. Wrapping function in base class configures SimConf.

    def SetupMonitor(self, slot=''):
        # Setup any necessary monitoring algorithms
``` 

Therefore, in order to use the DD4hep implementation of your sub-detector you have to implement `ApplyDetectorDetDesc` and populate the `LHCbGeo._listOfXMLIncludes_` list with the XML files required for your sub-detector. If your sub-detector is registering hits, then you should also provide a mapping between the name of the sensitive volume and the name of the factory used to construct the sensitive detector object. For `VP` we have:

```python
def ApplyDetectorDD4hep(self, basePieces, detPieces):
    # Configuring the DD4hep detector conversion.
    # 1. Add the mapping to have the LHCbDD4hepCnvSvc instrument
    # the G4 volume with the correct sensdet factory if marked with a
    # dd4hep sensitive detector named VP
    from Configurables import LHCbDD4hepCnvSvc
    mappings = LHCbDD4hepCnvSvc().getProp('SensDetMappings')
    mappings['VP'] = 'GiGaSensDetTrackerDD4hep/VPSDet'
    LHCbDD4hepCnvSvc().SensDetMappings = mappings

    # Add the necessary dd4hep includes for the VP. Also added all the
    # dependencies for material definitions that were identified using
    # trial and error. As the list is made unique before being added to
    # the xml this should be fine if they appear in multiple places
    from Gauss.Geometry import LHCbGeo
    go = LHCbGeo._listOfXMLIncludes_

    go += ["Pipe/parameters.xml"]
    go += ["Rich1/DetElem/RichRun3PropertySpecParam.xml"]
    go += ["Rich1/RichMatDir/RichMaterials.xml"]
    go += ["VP/parameters.xml"]
    go += ["VP/visualization.xml"]
    go += ["VP/detector.xml"]
    go += ["VP/conditions.xml"]
```

### Check if extra modifications are needed

This is more as a reminder. You have to make sure that the extraction hits classes and monitoring classes are implemented or modernized. Moreover, some sub-detectors use non-standard classes that might need additional changes for the Detector/DD4hep implementation. This has to be checked.

### Create a MR in Gauss targeting Futurev4 branch

Simply follow the same procedure as in: [](../getting_started/contributing.md#developing-gauss-on-gaussino).

### Put the sub-detector in the nighlty and LHCbPR tests

```{eval-rst}
.. attention::
    The generic way of doing this is a work-in-progress.
```
