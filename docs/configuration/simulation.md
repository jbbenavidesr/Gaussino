# Simulation

Configuration related to the simulation phase.

## Main Simulation Configuration

```{eval-rst}
.. currentmodule:: Gaussino.Simulation

.. autoclass:: GaussinoSimulation
   :show-inheritance:
   :undoc-members:
   :special-members: __apply_configuration__
   :members:
   :private-members:
```

## Custom Simulation

This adds a new package `CustomSimulation` that allows for adding custom simulation models (interfacing `Geant4` fast simulation hooks). Custom simulation models can be added in both the mass and parallel geometry. Adding custom simulation models in a separate parallel worlds is recommended. Please, take a look at the examples section to get more information.

### `CustomSimulation` class description

```{eval-rst}
.. currentmodule:: CustomSimulation.Configuration

.. autoclass:: CustomSimulation
   :show-inheritance:
   :undoc-members:
   :members:
   :private-members:
```

## Visualization

Configuration related to visualization.

```{eval-rst}
.. currentmodule:: GiGaMTVis.Configuration

.. autoclass:: GaussinoVisualization
   :show-inheritance:
   :undoc-members:
   :members:
   :private-members:
```

### Trajectory model options

As seen in previous section, users can fully modify or create their own trajectory models. Property `TrajectoryModelOptions` is an object which is used for that purpose. It's properties are shown in next snippet.

```python
vis.TrajectoryModelOptions = {
   "ModelName": "MyModel",
      # (string, optional) - default ""
      # Model name for trajectory model.
      # Applied only when using customTrajectoryModel option for TrajectoryModel.

   "Verbose": True,
      # (bool, optional) - default False
      # Determines the verbosity of the model output.

   "Attribute": "PN",
      # (string, optional) - default ""
      # Specifies the attribute to draw by when using customTrajectoryModel.

   "Values": ["gamma_key gamma", "electron_key e+"],
      # (list, optional) - default []
      # Specifies values that are visualized.
      # Each value is an item in the list which should consist of a key and a value (i.e. "key value").
      # The key can be used to specify color in the LineColors property.
      # Value, if applicable, should be written with units with space between numerical value and unit.

   # "Intervals": []
      # (list, optional) - default []
      # Specifies intervals that are visualized.
      # Each interval is an item in the list which should consist of a key and two values - lower and upper boundary (i.e. "key lower_bound upper_bound").
      # The key can be used to specify color in the LineColors property.
      # Values, if applicable, should be written with units with space between numerical value and unit.

   "LineColors": ["gamma_key green"],
      # (list, optional) - default []
      # Determines the line colors using color names of trajectories with specific value or in specific interval.
      # By default, all lines have color yellow when using custom model.
      # Item in list should be in the form of a string containing interval or value key and color (i. e. "key color").
      # Available colors are: red, green, blue, yellow, cyan, magenta, white, grey, black and brown.

   "RGBAColors": ["electron_key 1 0.1 0.1 0.1"],
      #(list, optional) - default []
      # Determines the line colors using RGBA color of trajectories with specific value or in specific interval.
      # Item in list should be in the form of a string containing an interval or a value key
      # and 4 numbers from 0 to 1 (i. e. "key r_value g_value b_value a_value").

   "DrawStepPts": True,
      # (bool, optional) - default ``True``

   "StepPtsSize": 1
      # (int, optional) - default ``2``
      # Determines the size of the step when drawing
}
```

### Trajectory filters

Trajectory filters can be configured through `TrajectoryFilters` property.
It takes a list of objects, each containing information for one filter.
Properties of the object are shown in the next example.

```python
vis.TrajectoryFilters = [{
   "FilterType": "momentumMagnitudeFilter",
      # (string, required) - default ""
      # Determines the filter type.
      # The list of available trajectory filter types is : ['chargeFilter', 'customTrajectoryFilter', 'encounteredVolumeFilter', 'originVolumeFilter', 'particleFilter', 'momentumMagnitudeFilter', 'kineticEnergyFilter', 'transverseMomentumFilter']

   "FilterName": "myMomMagFilter",
      # (string, optional) - default ""
      # Specifies the filter name. Should be used when there are two filters of the same type (two custom trajectory filters).

   "Verbose": True,
      # (bool, optional) - default False
      # Determines verbosity of a filter.

   "IsInclusive": True,
      # (bool, optional) - default True
      # Determines if specified values or intervals are passed through the filter or are they filtered out.
      # If set to True only values that are specified will pass through the filter while if it's set to False every values but specified will pass through the filter.

   # "Options": [],
      # (list, optional) - default []
      # List of options when using chargeFilter used to specify which values should be passed through filter (or filtered out - see property :IsInclusive:).
      # Available options are: ['addPositiveChargeParticles', 'addNegativeChargeParticles', 'addNeutralParticles'].

   "MinValue": 10 * MeV,
      # (float, optional) - default 0
      # Minimum value of attribute when using momentumMagnitudeFilter, kineticEnergyFilter or transverseMomentumFilter.

   "MaxValue": 1500 * MeV
      # (float, optional) - default null
      # Maximum value of attribute when using momentumMagnitudeFilter, kineticEnergyFilter or transverseMomentumFilter.

   # "Particles":
      # (list, optional) - default []
      # List of particles to be passed through filter (or filtered out - see property :IsInclusive:) when using particleFilter.

   # "Attribute": (string, optional) - default ""
      # Specifies the attribute to filter by when using customTrajectoryFilter.

   # "Values": [],
      # (list, optional) - default []
      # Specifies values that should pass through the filter (or be filtered out - see property :IsInclusive:) when using customTrajectoryFilter.
      # Each value is an item in the list which should consist of only a value (i.e. "value").

   # "Intervals": []
      # (list, optional) - default []
      # Specifies Intervals that should pass through the filter (or be filtered out - see property :IsInclusive:) when using customTrajectoryFilter.
      # Each interval is an item in the list which should consist of two values - lower and upper boundary (i.e. "lower_bound upper_bound").
}]
```

### Magnetic and electric field visualization

Both the magnetic and the electric field can be visualized using `MagneticField`and `ElectricField` properties, respectively.
Both have same properties shown in the next snippet.

```python

vis.MagneticField = {
   "IsVisible": True,
      # (bool, required) - default False
      # Boolean flag that enables visualization of the field.

   "N": 5,
      # (int, optional) default 0
      # Number of points where the field is visualized in one octant.
      # Total number of points is (2N + 1) ^ 3.

   "Representation": "fullArrow"
      # (string, optional) - default "fullArrow"
      # Specifies the representation of field lines.
}
```
