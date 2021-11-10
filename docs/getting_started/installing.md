# Running Gaussino

Please note that Gaussino is an experiment-independent package. If you are looking for a concrete experiment implementation, here is the list:
- Gauss-on-Gaussino (LHCb Experiment)

## Running from the nighlties

Currently it is only possible to build Gaussino with the help of nightly builds.

You can check the nighlty build status by visiting https://lhcb-nightlies.web.cern.ch/nightly/. The nightly slot currently available for Gaussino is ```lhcb-gaussino```. You can either install the latest build or one from the past in case the current one is failed. 

### Configuring the environment 

1. Make sure you're using LbEnv on lxplus or similar. You can run this to be sure:

```
source /cvmfs/lhcb.cern.ch/lib/LbEnv
```

2. Configure the platform

```
lb-set-platform x86_64_v2-centos7-gcc10-opt
```

### Building from source

```{eval-rst}
.. note::
    This is the recommended way for now.
```

1. Add the nightlies path to ```CMAKE_PREFIX_PATH``` directly from the destination

    ```
    export CMAKE_PREFIX_PATH=/cvmfs/lhcbdev.cern.ch/nightlies/lhcb-gaussino/Today:${CMAKE_PREFIX_PATH}
    ```

    OR by fetching the nightlies locally

    ```
    lbn-install --verbose --platforms=x86_64_v2-centos7-gcc10-opt --projects=GitCondDB,Gaudi,Geant4,DBASE,Detector,LHCb,Run2Support lhcb-gaussino Today
    export CMAKE_PREFIX_PATH=$PWD:$PWD/GitCondDB/InstallArea/x86_64_v2-centos7-gcc10-opt:${CMAKE_PREFIX_PATH}
    ```

2. Get vtune amplifier
    ```
    export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:/cvmfs/projects.cern.ch/intelsw/psxe/linux/x86_64/2019/vtune_amplifier 
    ```

3. Clone the repository and apply all pending merge requests with a label ```lhcb-gaussino```.

    ```
    git clone ssh://git@gitlab.cern.ch:7999/Gaussino/Gaussino.git
    cd Gaussino
    ```

4. Configure, build and install:
    ```
    cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -B build
    make -C build -j4 install
    ``` 

5. Run

    ```
    ./build/run gaudirun.py your_options.py
    ```

### Working on a satellite project

```{eval-rst}
.. warning::
    This is not working yet.
```

1. Get a skeleton of your project by typing:

    ```bash
    lb-dev --nightly lhcb-gaussino/Today Gaussino/HEAD -c x86_64-centos7-gcc9-opt
    ```
2. Additional configuration.

    ```bash
    cd GaussinoDev_HEAD
    ```

    ```bash
    git lb-use Gaussino https://gitlab.cern.ch/Gaussino/Gaussino
    ```

3. Configure the package you'd like to develop. Here ```Sim/Gaussino```

    ```bash
    git lb-checkout Gaussino/master Sim/Gaussino
    ```

4. Build.

    ```
    make
    ```

### Working on a satellite project with nightlies locally

```{eval-rst}
.. warning::
    This is not working yet.
```

This might be useful if you want a local copy of the projects in the nighlt slot or if you're looking for a nightly slot that is no longer present in ```/cvmfs/lhcbdev.cern.ch/nightlies/lhcb-gaussino```


```bash
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

```Today```` nighlty slot will be used in further steps.  

1. Create a base directory for your nighlties. 

    ```bash
    mkdir gaussino_base
    ```

2. Install the nightlies in your base directory.

    ```bash
    lbn-install --verbose --projects Gaussino --platforms x86_64-centos7-gcc9-opt --dest gaussino_base/lhcb-gaussino/Today lhcb-gaussino Today
    ```

3. Use the nightlies in your base directory to work on a satellite project.

    ```bash
    lb-dev --nightly-base gaussino_base --nightly lhcb-gaussino/Today Gaussino/HEAD -c x86_64-centos7-gcc9-opt
    ```
4. Additional configuration.

    ```bash
    cd GaussinoDev_HEAD
    ```

    ```bash
    git lb-use Gaussino https://gitlab.cern.ch/Gaussino/Gaussino
    ```

5. Configure the package you'd like to develop. Here ```Sim/Gaussino```

    ```bash
    git lb-checkout Gaussino/master Sim/Gaussino
    ```

6. Build.

    ```
    make
    ```
