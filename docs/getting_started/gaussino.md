# Working with Gaussino

Please note that Gaussino is an experiment-independent package. If you are looking for a concrete experiment implementation, here is the list:
- Gauss-on-Gaussino (LHCb Experiment, for now using the nightly build system)

## Using the LHCb nightly build system

### About the nightly system

You can check the nighlty build status by visiting https://lhcb-nightlies.web.cern.ch/nightly/. There are a few nightly slots currently available for Gaussino:
-  [```lhcb-gaussino```](https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/) standard slot for the majority of developments,
-  [```lhcb-dd4hep```](https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/) the nightly slot used for the integration of DD4hep sub-detectors,
-  [```lhcb-gaussino-fastsim```](https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino-fastsim/) nightly slot dedicated to fast simulation developments with Geant4 10.7,
-  [```lhcb-gaussino-prerelease```](https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino-prerelease/) the most stable version of Gaussino used to prepare beta releases with LHCb stack,

Gaussino is built in each nightly slots on 3 platforms:
- ```x86_64_v2-centos7-gcc11-opt``` release build with DetDesc geometry,
- ```x86_64_v2-centos7-gcc11-dbg``` debug build with DetDesc geometry,
- ```x86_64_v2-centos7-gcc11+dd4hep-opt``` release build with DD4Hep geometry,

### Configuring the environment 

Make sure that you are using LbEnv on lxplus or similar. You can run this to be sure:

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
```

### Configure the platform

The following will make sure that you have the most recent platform.

```shell
lb-set-platform x86_64_v2-centos7-gcc11-opt
export LCG_VERSION=101a
```

```{eval-rst}
.. tip::
    Use ``x86_64_v2-centos7-gcc11+dd4hep-opt`` on ``lhcb-dd4hep`` if you want to work with **Detector/DD4hep**.
```

### Building Gaussino from source

```{eval-rst}
.. note::
    This is the recommended way for now.
```

#### Fetch the nightlies locally

We're going to create a local copy of the projects used in the nightly slot. Gaussino requires Gaudi, Detector, LHCb, Run2Support and GaussinoExtLibs. LHCb and Run2Support are used when building Gaussino in the LHCb stack.
    

```shell
lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11-opt --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,GaussinoExtLibs lhcb-gaussino Today
cd lhcb-gaussino/Today
lb-set-workspace .
```

```{eval-rst}
.. tip::
    Use ``x86_64_v2-centos7-gcc11+dd4hep-opt`` on ``lhcb-dd4hep`` if you want to work with **Detector/DD4hep**.
```

```{eval-rst}
.. attention::
    The ``Today`` version of the nightly slot might not always be available. Please, always check directly on the website if the build of Gaussino & Gauss was successful (cells in the `table <https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino/>`_ should be either orange or green, but not red!). Always use the latest version that was successful (``yesterday``, ``1222`` etc.). 
```

#### Clone Gaussino and prepare your local development branch

```shell
git clone ssh://git@gitlab.cern.ch:7999/Gaussino/Gaussino.git
cd Gaussino
git checkout -b your_local_dev_branch
```

```{eval-rst}
.. attention::
    Don't forget to merge all pending merge requests with a label ``lhcb-gaussino`` (or any other MR that was picked up in the nightly)!
```

In order to get the list of pending merge requests check the checkout report by clicking on the **black arrow** next to the **Gaussino project** on the [website](https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino). You will get a list of the MR ids that are needed to work with this build of Gaussino. For example, if the MRs are Gaussino!24 and Gaussino!51, you can do the following:

```shell
git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
git merge --no-edit 24 51
```

#### Configure, build and install Gaussino

```shell
lb-project-init .
make -j4 install
``` 

#### Run Gaussino

```shell
./build.x86_64_v2-centos7-gcc11-opt/run gaudirun.py your_options.py
```

### Working on a satellite project

```{eval-rst}
.. error::
    This is not working yet and has to be updated.
```

#### Get a skeleton of your project

```shell
lb-dev --nightly lhcb-gaussino/Today Gaussino/HEAD -c x86_64-centos7-gcc9-opt
```
#### Additional configuration.

```shell
cd GaussinoDev_HEAD
```

```shell
git lb-use Gaussino https://gitlab.cern.ch/Gaussino/Gaussino
```

#### Configure the package
Conifure the package you'd like to develop. Here `Sim/Gaussino`

```shell
git lb-checkout Gaussino/master Sim/Gaussino
```

#### Build

```shell
make
```

### Working on a satellite project with nightlies locally

```{eval-rst}
.. error::
    This is not working yet and has to be updated.
```

This might be useful if you want a local copy of the projects in the nighlt slot or if you're looking for a nightly slot that is no longer present in `/cvmfs/lhcbdev.cern.ch/nightlies/lhcb-gaussino`


```shell
ll /cvmfs/lhcbdev.cern.ch/nightlies/lhcb-gaussino
```

Here is an exemplary list of nighlty slots:

```
total 5.0K
drwxr-xr-x. 12 cvmfs cvmfs 224 Apr 24 01:44 931
drwxr-xr-x. 12 cvmfs cvmfs 224 Apr 26 01:05 932
drwxr-xr-x. 12 cvmfs cvmfs 224 Apr 27 01:43 933
drwxr-xr-x. 12 cvmfs cvmfs 224 Apr 29 01:20 934
drwxr-xr-x. 12 cvmfs cvmfs 224 Apr 30 02:30 935
drwxr-xr-x. 12 cvmfs cvmfs 224 May  2 02:53 936
lrwxrwxrwx.  1 cvmfs cvmfs   3 Apr 30 01:11 Fri -> 935
lrwxrwxrwx.  1 cvmfs cvmfs   3 May  2 00:10 latest -> 936
lrwxrwxrwx.  1 cvmfs cvmfs   3 May  2 00:10 Sun -> 936
lrwxrwxrwx.  1 cvmfs cvmfs   3 Apr 29 00:35 Thu -> 934
```

`Today` nighlty slot will be used in further steps.  

#### Create a base directory for your nighlties

```shell
mkdir gaussino_base
```

#### Install the nightlies in your base directory

```shell
lbn-install --verbose --projects Gaussino --platforms x86_64-centos7-gcc9-opt --dest gaussino_base/lhcb-gaussino/Today lhcb-gaussino Today
```

#### Use the nightlies in your base directory

```shell
lb-dev --nightly-base gaussino_base --nightly lhcb-gaussino/Today Gaussino/HEAD -c x86_64-centos7-gcc9-opt
```

#### Additional configuration

```shell
cd GaussinoDev_HEAD
```

```shell
git lb-use Gaussino https://gitlab.cern.ch/Gaussino/Gaussino
```

#### Configure the package
Configure the package you'd like to develop. Here ```Sim/Gaussino```

```shell
git lb-checkout Gaussino/master Sim/Gaussino
```

#### Build

```shell
make
```
