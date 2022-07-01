
# Handling GDML files

Handling of GDML files is performed via the `SimPhase()` configurable (this might change in the nearest future).

## GDML import

If you wish to import the logical volumes from the GDML file you have to provide all the necessary options in the `ImportGDML` property of `SimPhase`. All the imported volumes will be embedded in a `G4Assembly` that can be translated and rotated with respect to the existing world. For example:

```python
from Gaudi.Configuration import DEBUG
from GaudiKernel.SystemOfUnits import m, deg
from Gaussino.Simulation import SimPhase
SimPhase().ImportGDML = [
    {
        "GDMLFileName": "path_to_your_gdml_file_1.gdml",
        "OutputLevel": DEBUG,
        "GDMLTranslationX": 10. * mm,
        "GDMLRotationX": 45. * deg,
    },
    {
        "GDMLFileName": "path_to_your_gdml_file_2.gdml",
        "OutputLevel": DEBUG,
    },
]
```

It will import the volumes from the two files: `path_to_your_gdml_file_1.gdml` and `path_to_your_gdml_file_2.gdml`. Volumes from the first files will be embedded in a rotated and translated assembly.


### Making the imported volumes sensitive

The sensitve detector classes have to be added manually to the imported sensitive volumes. Below you will find an example of how to add a generic `MCCollectorSensDet` factory to an imported volume `ImportedLVol`:

```python
from Gaussino.Simulation import SimPhase
SimPhase().SensDetMap = { 
    "MCCollectorSensDet/ImportedSDet": ["ImportedLVol"],
}

# and if you wish to change the default properties of the factory

from Configurables import MCCollectorSensDet
MCCollectorSensDet(
    "GiGaMT.DetConst.CubeToImportSDet",
    OutputLevel=DEBUG,
    PrintStats=True,
)
```

## GDML export

If you wish to export the whole world to aGDML file you have to provide all the necessary options in the `ExportGDML` property of `SimPhase`. For example:

```python
SimPhase().ExportGDML = {
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
