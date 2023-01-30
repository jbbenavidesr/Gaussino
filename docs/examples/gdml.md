
# Handling GDML files

Handling of GDML files is performed via the `GaussinoGeometry()` configurable.

## GDML import

If you wish to import the logical volumes from the GDML file you have to provide all the necessary options in the `ImportGDML` property of `GaussinoGeometry`. All the imported volumes will be embedded in a `G4Assembly` that can be translated and rotated with respect to the existing world. For example:

```python
from GaudiKernel.SystemOfUnits import m, deg
from Gaussino.Geometry import GaussinoGeometry
GaussinoGeometry().ImportGDML = [
    {
        "GDMLFileName": "path_to_your_gdml_file_1.gdml",
        "GDMLTranslationX": 10. * mm,
        "GDMLRotationX": 45. * deg,
    },
    {
        "GDMLFileName": "path_to_your_gdml_file_2.gdml",
    },
]
```

It will import the volumes from the two files: `path_to_your_gdml_file_1.gdml` and `path_to_your_gdml_file_2.gdml`. Volumes from the first files will be embedded in a rotated and translated assembly.


```{eval-rst}
.. attention::
    GDML importing feature is not a geometry service on its own. If you are not using `DD4hepCnvSvc` or any other geometry service that creates the world volume, then you can use the `ExternalDetector` package. Options below will create an empty world volume, in which you will have your volumes imported from a GDML file:

    .. code-block:: python

        from Configurables import (
            GaussinoGeometry,
            ExternalDetectorEmbedder,
        )

        emb_name = "Embedder_1"
        GaussinoGeometry().ExternalDetectorEmbedder = emb_name
        external = ExternalDetectorEmbedder(emb_name)

        # plain/testing geometry service
        external.World = {
            "WorldMaterial": "OuterSpace",
            "Type": "ExternalWorldCreator",
        }

        from ExternalDetector.Materials import OUTER_SPACE
        # material needed for the external world
        external.Materials = {
            "OuterSpace": OUTER_SPACE,
        }
```

### Making the imported volumes sensitive

The sensitve detector classes have to be added manually to the imported sensitive volumes. Below you will find an example of how to add a generic `MCCollectorSensDet` factory to an imported volume `ImportedLVol`:

```python
GaussinoGeometry().SensDetMap = {
    "MCCollectorSensDet/ImportedSDet": ["ImportedLVol"],
}

# and if you wish to change the default properties of the factory

from Configurables import MCCollectorSensDet
MCCollectorSensDet(
    "GiGaMT.DetConst.CubeToImportSDet",
    PrintStats=True,
)
```

## GDML export

If you wish to export the whole world to aGDML file you have to provide all the necessary options in the `ExportGDML` property of `GaussinoGeometry`. For example:

```python
GaussinoGeometry().ExportGDML = {
    "GDMLFileName": "export.gdml",
    # G4 will crash if the file with same name already exists
    "GDMLFileNameOverwrite": True,
    # add unique references to the names
    "GDMLAddReferences": True,
    # export auxilliary information
    "GDMLExportEnergyCuts": True,
    "GDMLExportSD": True,
}
```

### GDML exporting of a parallel world

Parallel worlds follow the same configuration as in the mass geometry case, but it has to be provided in the properties of you parallel world. For example:

```python
ParallelGeometry().ParallelWorlds = {
    'ParallelWorld1': {
        'ExternalDetectorEmbedder': 'ParallelEmbedder',
        'ExportGDML': {
            'GDMLFileName': 'ParallelWorld1.gdml',
            'GDMLFileNameOverwrite': True,
            'GDMLExportSD': True,
            'GDMLExportEnergyCuts': True,
        },
    },
}
```
