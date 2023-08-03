# Geometry

Configuration related to the geometry used in the simulation phase.

## Main Geometry Configuration

```{eval-rst}
.. currentmodule:: Gaussino.Geometry

.. autoclass:: GaussinoGeometry
   :show-inheritance:
   :undoc-members:
   :special-members: __apply_configuration__
   :members:
   :private-members:
```

## External Detector

Gaussino as a standalone application does not have a 'main' geometry service. `ExternalDetector` package provides an abstract geometry service that can work on its own, but it also works with any of the 'main' geometry services (e.g. LHCb's `GaussGeo`, `DD4hepCnvSvc`). `ExternalDetector` provides all the necessary tools that are needed to embed volumes and make them sensitive. Moreover, it is possible to attach hit extraction and monitoring algorithms.

```{eval-rst}
.. Important::
    `ExternalDetector` can work as a standalone geometry service or it can work wit other geometry services.

```

### `ExternalDetector` configuration

```{eval-rst}
.. currentmodule:: ExternalDetector.Configuration

.. autoclass:: ExternalDetectorEmbedder
   :show-inheritance:
   :undoc-members:
   :members:
   :private-members:
```

### External World (standalone mode)

This is a default mode for `Gaussino` as it does not provide any main geometry services. Main geometry services should be provided by the projects built on top of `Gaussino`. In the standalone mode, the `ExternalDetector` package provides all the necessary tools to create an abstract world volume. This is done by setting the `ExternalDetectorEmbedder.World` property.

```python
external.World = {
    "Type": "ExternalWorldCreator",
    # -> type of the factory used to build the world
    # -> see: Sim/ExternalDetector/WorldFactory.h if you need to create a custom one
    "WorldMaterial": "SomeG4Material",
    # -> material used by the world, it is the name of the G4Material
    #    that must be defined before
    # -> see section for materials and Sim/ExternalDetector/MaterialFactory.h
    # + other properties used by the WorldFactory e.g. OutputLevel etc.
}
```

### External Materials

Since there is no geometry service, Gaussino does not have any materials it can use for the creation of the worlds and volumes. `Materials` property will do this jon provided you give all the required properties needed to create a `G4Material`. As of now, there are 2 generic types of factories, but the template class `MaterialFactory` allows for adding any custom factory that constructs a `G4Material` object.

#### MaterialFromChemicalPropertiesFactory

`MaterialFromChemicalPropertiesFactory` creates a `G4Material` given the chemical properties. Below you'll find an example of the interstellar vacuum `OuterSpace` material.

```python
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin

external.Materials = {
    # -> dictionaries of properties, one dict per material
    "OuterSpace": {
        "Type": "MaterialFromChemicalPropertiesFactory"
        # -> type of material factory
        "AtomicNumber": 1.,
        # -> required, atomic number
        "MassNumber": 1.01 * g / mole,
        # -> required, mass number [g / mole]
        "Density": 1.e-25 * g / cm3,
        # -> required, density of the material [g / cm3]
        "Pressure": 3.e-18 * pascal,
        # -> optional, pressure [Pa], default: 1 atm
        "Temperature": 2.73 * kelvin,
        # -> optional, temperature [K], default: room temperature
        "State": 'Gas',
        # -> optional, state: ['Gas', 'Liquid', 'Solid'], default: 'Undefined'
        # + other properties used by the MaterialFactory e.g. OutputLevel etc.
    },
}
```

and the `Geant4` output:

```
GiGaMT.DetConst...  DEBUG
 Material: OuterSpace    density:  0.001 mg/cm3  RadL: 1010.068 km   Nucl.Int.Length: 560.762 km
                       Imean:  19.200 eV   temperature:   2.73 K  pressure:   0.00 atm

   --->  Element: H (H)   Z =  1.0   N =     1   A =  1.008 g/mole
         --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance: 99.989 %
         --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:  0.011 %
          ElmMassFraction: 100.00 %  ElmAbundance 100.00 %
```

#### MaterialFromElements

`MaterialFromElements` creates a `G4Material` given the properties of the used elements. Below you'll find an example of the material made out of lead.

```python
from GaudiKernel.SystemOfUnits import g, cm3, pascal, mole, kelvin

external.Materials = {
    # -> dictionaries of properties, one dict per material
    'Pb': {
        'Type': 'MaterialFromElements',
        # -> type of material factory
        'Symbols': ['Pb'],
        # -> required, list of the elements' symbols
        'AtomicNumbers': [82.],
        # -> required, list of atomic numbers per element
        'MassNumbers': [207.2 * g / mole],
        # -> required, list of mass numbers per element
        'MassFractions': [1.],
        # -> required, list of mass fractions in 0. - 1., total <= 1.
        'Density': 11.29 * g / cm3,
        # -> required, density of the material [g / cm3]
        'State': 'Solid',
        # -> optional, state: ['Gas', 'Liquid', 'Solid'], default: 'Undefined'
        # + other properties used by the MaterialFactory e.g. OutputLevel etc.
    },
}
```
and the `Geant4` output:

```
GiGaMT.DetConst.Pb  DEBUG
 Material:       Pb    density: 11.290 g/cm3   RadL:   5.642 mm   Nucl.Int.Length:  18.344 cm
                       Imean: 823.000 eV   temperature: 273.15 K  pressure: 6241.51 atm

   --->  Element: Pb (Pb)   Z = 82.0   N =   207   A = 207.200 g/mole
         --->  Isotope: Pb204   Z = 82   N = 204   A = 203.97 g/mole   abundance:  1.400 %
         --->  Isotope: Pb206   Z = 82   N = 206   A = 205.97 g/mole   abundance: 24.100 %
         --->  Isotope: Pb207   Z = 82   N = 207   A = 206.98 g/mole   abundance: 22.100 %
         --->  Isotope: Pb208   Z = 82   N = 208   A = 207.98 g/mole   abundance: 52.400 %
          ElmMassFraction: 100.00 %  ElmAbundance 100.00 %
```


### External Shapes / Volumes

External Detector provides a template class `Embedder` that is responsible for creating and embedding the volume of a particular shape in the geometry. The library comes only with just factory `CuboidEmbedder` creating cuboid-like shapes, but it is fairly easy to create your own (see: [section](#embedding-your-own-custom-shape)).

```python
from GaudiKernel.SystemOfUnits import m

external.Shapes = {
    "MyPlane": {
        # -> type of the embedder
        "Type": "Cuboid",
        # -> x position of its center
        "xPos": 0 * m,
        # -> y position of its center
        "yPos": 0 * m,
        # -> z position of its center
        "zPos": 0 * m,
        # -> length of the x side
        "xSize": 1. * m,
        # -> length of the y side
        "ySize": 1. * m,
        # -> length of the z side
        "zSize": 1. * m,
        # + other properties used by the CuboidEmbedder e.g. OutputLevel etc.
    },
}
```

If you want your volume to be sensitive, you have to specify exactly what factory creating object of type `G4VSensitiveDetector` will be activated. `Sensitive` property was created for this purpose. `MCCollector` package provides a very generic sensitive detector class.

```python
external.Sensitive = {
    # -> important! you have to specify what volume
    "MyPlane": {
        # -> required, type of the sensitive detector factory
        "Type": "MCCollectorSensDet",
        # + other properties used by the GiGaMTG4SensDetFactory e.g. OutputLevel etc.
    },
}
```


### External Hit extraction

If you want to attach a special algorithm resposible for extracting hits generated by your sensitive volume, then you can use the `Hit` property for this purpose. `MCCollector` package provides a very generic hit extraction algorithm that creats `MCExtendedHits`.

```python
external.Hit = {
    # -> important! you have to specify what sensitive volume
    "MyPlane": {
        # -> required, type of the hit extraction algorithm
        "Type": "GetMCCollectorHitsAlg",
        # + other properties used by the Gaudi::Functional algorithm e.g. OutputLevel etc.
    },
}
```



### External Monitoring

Finally, there is a possinility add a monitoring algorithm on top of other aglorithms associated with your sensitive volume. It should be also a `Gaudi::Functional` algorithm, most likely a `Consumer`. As of now, there's no monitoring algorithm in Gaussino that can be used here as an example.


```python
external.Moni = {
    # -> important! you have to specify what sensitive volume
    "MyPlane": {
        # -> required, type of the monitoring algorithm
        "Type": "SomeMonitoringAlgorithm",
        # + other properties used by the Gaudi::Functional algorithm e.g. OutputLevel etc.
    },
}
```

### Embedding your own, custom shape

`ExternalDetector::Embedder` is responsible for building and placing your detector in the mother volume. You have to inherit from it and override `ExternalDetector::Embedder::build()` method that creates an object inheriting from `G4VSolid`.

Below, you'll find an example of how a custom `SphereEmebedder` can be implemented.

```c++
namespace ExternalDetector {
  class SphereEmbedder : public Embedder<G4Sphere> { // here is G4Sphere, but can be anything < G4VSolid
    // some properties
    Gaudi::Property<double> m_rMin{this, "RMin", 0.};
    Gaudi::Property<double> m_rMax{this, "RMax", 0.};
    Gaudi::Property<double> m_sPhi{this, "SPhi", 0.};
    Gaudi::Property<double> m_dPhi{this, "DPhi", 0.};
    Gaudi::Property<double> m_sTheta{this, "STheta", 0.};
    Gaudi::Property<double> m_dTheta{this, "DTheta", 0.};
    // name of the sphere
    Gaudi::Property<std::string> m_sphereName{this, "SphereName", "Sphere"};

  public:
    using Embedder::Embedder;

    inline virtual G4Sphere* build() const override {
      return new G4Sphere( m_sphereName.value(),
                           m_rMin.value(),
                           m_rMax.value(),
                           m_sPhi.value(),
                           m_dPhi.value(),
                           m_sTheta.value(),
                           m_dTheta.value());
    }
  };
} // namespace ExternalDetector

DECLARE_COMPONENT_WITH_ID( ExternalDetector::SphereEmbedder, "SphereEmbedder" )
```

## Parallel Geometry

This adds a new package `ParallelGeometry` that allows for adding external detectors (from `ExternalDetector` package) in parallel worlds on top of mass geometry. This can be used in studies where we would like to have volumes / sensitive detectors overlapping each other.

Embedding a volume / sensitive detector in a parallel world is almost the same as for the mass geometry with the exception that materials are not required to be defined i.e. can be `nullptr`. If that is the case, then all the particles just "see" the boundaries coming from the parallel world.

If you actually want to set any material in your parallel world, then make sure you have `LayeredMass=True` in the configuration of parallel world physics. Otherwise, it won't work. If `LayeredMass=True`, then all the worlds below in your stack will see the material of that world.


### `ParallelGeometry` configuration

```{eval-rst}
.. currentmodule:: ParallelGeometry.Configuration

.. autoclass:: ParallelGeometry
   :show-inheritance:
   :undoc-members:
   :members:
   :private-members:
```
