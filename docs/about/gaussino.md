# Gaussino

## Short description

The LHCb simulation team decided to move all the experiment-independent components from the simulation software and place it in a separate project, called Gaussino, as a core simulation framework. Gaussino can also work as a standalone application. Gaussino follows the Gaudi's inter-event-based parallelism of the event loop, in which algorithms are scheduled in a way that guarantees thread-safety. Gaussino communicates with Geant4 objects by creating corresponding factories that act as Gaudi tools. In the event loop, Gaudi places the generated event on the top of a FIFO queue. Geant4 worker threads then take the event from the queue and perform the simulation following their own multi-threading scheme.

## Further reading

You can find more information about Gaussino in the following papers:

```{eval-rst}
.. [GaussinoPaper1] D. Muller and B. G. Siddi, `Gaussino - a Gaudi-Based Core Simulation Framework <https://ieeexplore.ieee.org/document/9060074>`_, 

.. [GaussinoPaper2] D. Muller, `Adopting new technologies in the LHCb Gauss simulation framework <https://www.epj-conferences.org/articles/epjconf/abs/2019/19/epjconf_chep2018_02004/epjconf_chep2018_02004.html>`_

.. [GaussinoPaper3] M. Mazurek, G.Corti, D. Muller, `New Simulation Software Technologies at the LHCb Experiment at CERN <http://www.cai.sk/ojs/index.php/cai/article/view/2021_4_815>`_
```
