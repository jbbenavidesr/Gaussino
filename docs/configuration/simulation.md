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
