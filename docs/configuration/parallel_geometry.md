# Parallel Geometry

This adds a new package `ParallelGeometry` that allows for adding external detectors (from `ExternalDetector` package) in parallel worlds on top of mass geometry. This can be used in studies where we would like to have volumes / sensitive detectors overlapping each other. 

Embedding a volume / sensitive detector in a parallel world is almost the same as for the mass geometry with the exception that materials are not required to be defined i.e. can be `nullptr`. If that is the case, then all the particles just "see" the boundaries coming from the parallel world. 

If you actually want to set any material in your parallel world, then make sure you have `LayeredMass=True` in the configuration of parallel world physics. Otherwise, it won't work. If `LayeredMass=True`, then all the worlds below in your stack will see the material of that world.



### `ParallelGeometry` class description

```{eval-rst}
.. currentmodule:: ParallelGeometry.Configuration

.. autoclass:: ParallelGeometry
   :show-inheritance:
   :members: attach, attach_physics, world_to_gdml
   :undoc-members:
```

