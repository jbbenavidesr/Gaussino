# General information

Two official branches are currently actively developed in Gauss in addition to the master:
- **Sim09**, for any change that has to be deployed for Sim09 production for physics studies with the current detector. Releases of the Gauss v49rX series, based on a frozen Gaudi/LHCb and  a fixed version of Pythia8 (+ tuning) and Geant4, are made from this branch.
- **Sim09-upgrade**, for any change that has to be deployed for physics quality productions for upgrade studies or to test in production technical changes that could cause incompatible results in Sim09 before deploying them there. Release of the Gauss v50rX series are made from this branch that is based on more recent versions of Gaudi/LHCb but on the same fixed version of Pythia8 and Geant4 as those of Sim09.
- Simulation specific development for the next Sim10 (and further) are made in the master branch and include new versions of Pythia8 and Geant4 10 as well as the latest version of Gaudi and LHCb.

The master and official branches are all protected, meaning that code cannot be pushed into them directly but only through merge requests. 
This allows us to check things work correctly before including them in the official branches for the next release.


