# Working with Gauss-on-Gaussino [TO BE MOVED]
```{eval-rst}
.. attention::
    This section will be moved to a new documentation website dedicated to Gauss.
```

## Using the LHCb nightly build system

### Building Gauss and Gaussino from source

If you wish to work with Gauss (LHCb simulation framework) based on Gaussino's core functionalities, follow the recipe in the section dedicated to Gaussino (see: [](./gaussino.md#working-with-gaussino)) and go back to the directory where your local copy of the nighly slot resides. 

```{eval-rst}
.. tip::
    If you do not have any changes in Gaussino, you can use Gaussino directly from the nightlies. Just follow the same recipe up to the section `Fetch the nightlies locally <./gaussino.md#fetch-the-nightlies-locally>`_ and add Gaussino to the list of copied projects:

    .. code-block:: shell

        lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11-opt --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,Gaussino, GaussinoExtLibs lhcb-gaussino Today

We really encourage you to use `lbn-install` as it generates a local copy of the nightly build directly on your machine and will create the most stable environment for your developments.  
```


#### Clone Gauss and prepare your local development branch
```shell
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Gauss
cd Gauss
```

Gauss-on-Gaussino is develped on a dedicated branch: `Futurev5`. So your development has to start from this branch. 

```shell
git checkout Futurev5
git checkout -b your_local_dev_branch
```
The rest is very similar to the development in Gaussino:

```{eval-rst}
.. attention::
    Don't forget to merge all pending merge requests with a label ``lhcb-gaussino`` (or any other MR that was picked up in the nightly)!
```

```{eval-rst}
.. hint::
    The list of open merge requests with the label ``lhcb-gaussino`` can be acquired automatically with this command:

    .. code-block:: shell-session
    
        curl -s "https://gitlab.cern.ch/api/v4/projects/2606/merge_requests?state=opened&labels=lhcb-gaussino" | jq '.[].iid'
```

In order to get the list of pending merge requests check the checkout report by clicking on the **black arrow** next to the **Gauss project** on the [website](https://lhcb-nightlies.web.cern.ch/nightly/lhcb-gaussino). You will get a list of the MR ids that are needed to work with this build of Gauss. For example, if the MRs are Gauss!800 and Gauss!845, you can do the following:

```shell
git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
git merge --no-edit 800 845
```


#### Configure, build and install Gauss-on-Gaussino

```shell
lb-project-init .
make -j4 install
``` 

#### Run Gauss

```shell
./build.x86_64_v2-centos7-gcc11-opt/run gaudirun.py your_options.py
```

### Example 1 building Gauss only

Below you will find a summary of the commands that should cover the majority of the use cases.

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
lb-set-platform x86_64_v2-centos7-gcc11-opt
export LCG_VERSION=101x
lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11-opt --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,GaussinoExtLibs,Gaussino lhcb-gaussino Today
cd lhcb-gaussino/Today
lb-set-workspace .
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Gauss
cd Gauss
git checkout Futurev5
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
```

### Example 2 building Gauss and Gaussino

Below you will find a summary of the commands that should cover the majority of the use cases in which we need to modify both Gauss & Gaussino.

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
lb-set-platform x86_64_v2-centos7-gcc11-opt
export LCG_VERSION=101x
lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11-opt --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,GaussinoExtLibs,Gaussino lhcb-gaussino Today
cd lhcb-gaussino/Today
lb-set-workspace .
git clone ssh://git@gitlab.cern.ch:7999/Gaussino/Gaussino.git
cd Gaussino
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 23 45
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Gauss
cd Gauss
git checkout Futurev5
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
```

### Example 3 working with DD4hep and Detector from the nightlies

Below you will find a summary of the commands needed to work with DD4hep/Detector that is taken from the nightlies. There should be ne need to change anything in Gaussino, so we will only build Gauss.

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
lb-set-platform x86_64_v2-centos7-gcc11+dd4hep-opt
export LCG_VERSION=101x
lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11+dd4hep-opt --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,GaussinoExtLibs,Gaussino lhcb-gaussino Today
cd lhcb-gaussino/Today
lb-set-workspace .
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Gauss
cd Gauss
git checkout Futurev5
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
```

### Example 4 working with DD4hep and Detector built locally

Below you will find a summary of the commands needed to work with DD4hep/Detector that is built locally. In this case, we have to rebuild all the projects downstream with respect to the Detector.

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
lb-set-platform x86_64_v2-centos7-gcc11+dd4hep-opt
export LCG_VERSION=101x
lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11+dd4hep-opt --projects=Gaudi,Geant4,DBASE lhcb-gaussino Today
cd lhcb-gaussino/Today
lb-set-workspace .
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Detector.git
cd Detector
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/lhcb/LHCb.git
cd LHCb
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Run2Support.git
cd Run2Support
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/Gaussino/GaussinoExtLibs.git
cd GaussinoExtLibs
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 23 45
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/Gaussino/Gaussino.git
cd Gaussino
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 23 45
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Gauss
cd Gauss
git checkout Futurev5
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
```

### Example 5: working with fast simulations with Geant4 10.7

Below you will find a summary of the commands needed to work with fast simulations with Geant4 10.7. In this example we will also build Gaussino as there might be some changes required in the generic fast simulation interface.

```shell
source /cvmfs/lhcb.cern.ch/lib/LbEnv
lb-set-platform x86_64_v2-centos7-gcc11-opt
export LCG_VERSION=101x
lbn-install --verbose --platforms=x86_64_v2-centos7-gcc11-opt --projects=Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support,GaussinoExtLibs
lhcb-gaussino-fastsim Today
cd lhcb-gaussino-fastsim/Today
lb-set-workspace .
git clone ssh://git@gitlab.cern.ch:7999/Gaussino/Gaussino.git
cd Gaussino
git checkout FASTSIM
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 23 45
lb-project-init .
make -j4 install
cd ..
git clone ssh://git@gitlab.cern.ch:7999/lhcb/Gauss
cd Gauss
git checkout FASTSIM
git checkout -b your_local_dev_branch
# check the MRs!
# git fetch && git fetch origin '+refs/merge-requests/*/head:refs/remotes/*'
# git merge --no-edit 800 845
lb-project-init .
make -j4 install
```
