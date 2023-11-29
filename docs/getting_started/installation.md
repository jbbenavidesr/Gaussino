# Installation

Please note that Gaussino is an experiment-independent package. If you are looking for a concrete experiment implementation, here is the list:
- [Gauss-on-Gaussino in the LHCb Experiment](https://lhcb-gauss.docs.cern.ch/master/index.html)

```{eval-rst}
.. attention::

    **Gaussino currently builds only in the LHCb environment!** Feel free to test in another setup.

    In the examples below, we are going to use nightly build system in LHCb. You can check the status of nightly build by visiting the `LHCb nightly webpage <https://lhcb-nightlies.web.cern.ch/nightly/>`_. There are a few nightly slots currently available for Gaussino:

    -  `lhcb-gaussino <https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/>`_ a stable slot with the most recent tags: *Gaussino/v0rX* and *Gauss/v60rX*,
    -  `lhcb-gaussino-dev <https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/>`_ a nightly slot with all the pending developments tested with *Gaussino/master* and *Gauss/Futurev5*,
    -  `lhcb-dd4hep <https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/>`_ the nightly slot used for the integration of DD4hep sub-detectors tested with the most recent tags: *Gaussino/v0rX* and *Gauss/v60rX*,
    -  `lhcb-gaussino-fastsim <https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino-fastsim/>`_ nightly slot dedicated to fast simulation developments with Geant4 10.7,

    Currently, Gaussino is built in each nightly slots on 3 platforms:

    - **x86_64_v2-centos7-gcc11-opt** release build with **DetDesc & DD4Hep** geometry,
    - **x86_64_v2-centos7-gcc11-dbg** debug build with **DetDesc & DD4Hep** geometry,
    - **x86_64_v2-centos7-gcc11+detdesc-opt** release build with **DetDesc only** geometry,
```

## Building full Gaussino (LHCb)

### Using a local copy of the nightly build

We're going to create a local copy of the projects used in the nightly slot. Gaussino requires Gaudi, Detector, LHCb, Run2Support and GaussinoExtLibs. LHCb and Run2Support are used when building Gaussino in the LHCb stack.

Make sure that you are using LbEnv on lxplus or similar. You can run this to be sure:

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
```

The following will make sure that you have the most recent platform.

```shell
lb-set-platform x86_64_v2-centos7-gcc11-opt
export LCG_VERSION=103
```

Fetch the nightlies locally

```shell
lbn-install --verbose --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,GaussinoExtLibs lhcb-gaussino Today
cd lhcb-gaussino/Today
lb-set-workspace .
```

```{eval-rst}
.. attention::
    The ``Today`` version of the nightly slot might not always be available. Please, always check directly on the website if the build of Gaussino & Gauss was successful (cells in the `table <https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/>`_ should be either orange or green, but not red!). Always use the latest version that was successful (``yesterday``, ``1222`` etc.).
```

Clone, configure, build and install Gaussino:

```shell
git clone ssh://git@gitlab.cern.ch:7999/Gaussino/Gaussino.git
cd Gaussino
lb-project-init .
make -j4 install
```

## Building a subset of Gaussino packages (LHCb)

The example below shows you how to work and build a subset of the packages of Gaussino. We will use `lb-dev` for this purpose. We are going to need one of the nightly slots mentioned in the previous section.

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
lb-dev --nightly lhcb-gaussino/Today Gaussino/HEAD
cd GaussinoDev_HEAD
git lb-use Gaussino https://gitlab.cern.ch/Gaussino/Gaussino
```

Configure and build the package you'd like to modify. Below you will find an example for `Sim/Gaussino`.

```shell
git lb-checkout Gaussino/master Sim/Gaussino
make
```
