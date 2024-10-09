# Running your first simulation

Below you will find instructions on how to prepare a configuration file with a minimum working example. We will try to generate 5 events with a 1 MeV photon hitting a 1m x 1m x 1m lead cube.

## Prerequisites

1. Make sure that your Gaussino is correctly configured and built following the instructions in [](./installation.md#installation) section.

2. Create and open a new python file. It will consists of all the options you need to configure the simulation job.

    ```shell
    vim your_gaussino_options.py
    ```

3. We will be now setting the properties of Gaussino's configurables. There are 4 main configurables:

    ```{eval-rst}
    - :class:`Gaussino <Gaussino.Configuration.Gaussino>` (main configurable)
    - :class:`GaussinoGeneration <Gaussino.Generation.GaussinoGeneration>` (configuration of the generation phase)
    - :class:`GaussinoSimulation <Gaussino.Simulation.GaussinoSimulation>` (configuration of the simulation phase)
    - :class:`GaussinoGeometry <Gaussino.Geometry.GaussinoGeometry>` (configuration of the geometry)
    ```

    These configurables should be enough for most of the cases, but sometime you will also find a few other configurables, for example:

    ```{eval-rst}
    - :class:`ExternalDetectorEmbedder <ExternalDetector.Configuration.ExternalDetectorEmbedder>` (optional configuration of the internal geometry service of Gaussino)
    ```

    and so on... Each of the configurable you can import from ``Configurables``. For example, for :class:`Gaussino <Gaussino.Configuration.Gaussino>` it will be:

    ```python
    from Configurables import Gaussino
    ```

## Simple example

1. Specify the number of events by adding the following lines:

    ```python
    Gaussino().EvtMax = 5
    ```

2. Do you want to run the generator phase and the simulation phase? Or just the generator? You can modify this by adding the following lines:

    ```python
    Gaussino().Phases = ["Generator", "Simulation"]
    ```

    Please, note that the default is to have both the generator and the simulation phase.

3. Specify what to particles to generate. In this example we will use a particle gun to generate 1 MeV photons. In order to do so, we have to add a `FixedMomentum` and `FlatNParticles` tools.

    ```python
    from GaudiKernel import SystemOfUnits as units
    from Configurables import (
        GaussinoGeneration,
        ParticleGun,
        FixedMomentum,
        FlatNParticles,
    )

    GaussinoGeneration().ParticleGun = True
    pgun = ParticleGun("ParticleGun")
    pgun.addTool(FixedMomentum, name="FixedMomentum")
    pgun.ParticleGunTool = "FixedMomentum"
    pgun.addTool(FlatNParticles, name="FlatNParticles")
    pgun.NumberOfParticlesTool = "FlatNParticles"
    pgun.FlatNParticles.MinNParticles = 1
    pgun.FlatNParticles.MaxNParticles = 1
    pgun.FixedMomentum.px = 0.0 * units.GeV
    pgun.FixedMomentum.py = 0.0 * units.GeV
    pgun.FixedMomentum.pz = 1.0 * units.GeV
    pgun.FixedMomentum.PdgCodes = [22]
    ```

4. Add a required Geant4 physics constructor, for example:

    ```python
    from Configurables import GaussinoSimulation

    GaussinoSimulation().PhysicsConstructors.append("GiGaMT_G4EmStandardPhysics")
    ```

5. Add some geometry: we will construct a world and add a 1 m x 1 m x 1 m lead cube.

    ```python
    from GaudiKernel import SystemOfUnits as units
    from Configurables import (
        GaussinoGeometry,
        ExternalDetectorEmbedder,
    )
    from ExternalDetector.Materials import (
        OUTER_SPACE,
        LEAD,
    )

    emb_name = "ExternalDetectorEmbedder_0"
    cube_name = f"{emb_name}_Cube"

    GaussinoGeometry().ExternalDetectorEmbedder = emb_name
    external = ExternalDetectorEmbedder(emb_name)
    external.Shapes = {
        cube_name: {
            "Type": "Cuboid",
            "xSize": 1.0 * units.m,
            "ySize": 1.0 * units.m,
            "zSize": 1.0 * units.m,
            "MaterialName": "Pb",
        },
    }

    external.Sensitive = {
        cube_name: {
            "Type": "MCCollectorSensDet",
        },
    }

    external.World = {
        "WorldMaterial": "OuterSpace",
        "Type": "ExternalWorldCreator",
    }


    external.Materials = {
        # material needed for the external world
        "OuterSpace": OUTER_SPACE,
        # material needed for the lead cube
        "Pb": LEAD,
    }
    ```

## Running Gaussino

Once your `your_gaussino_options.py` file is ready you can test running Gaussino!

```shell
./build.x86_64_v2-centos7-gcc11-opt/run gaudirun.py your_gaussino_options.py
```
