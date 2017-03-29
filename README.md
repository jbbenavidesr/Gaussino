# General information

Two official branches are currently actively developed in Gauss in addition to the master:
- **Sim09**, for any change that has to be deployed for Sim09 production for physics studies with the current detector. Releases of the Gauss v49rX series, based on a frozen Gaudi/LHCb and  a fixed version of Pythia8 (+ tuning) and Geant4, are made from this branch.
- **Sim09-upgrade**, for any change that has to be deployed for physics quality productions for upgrade studies or to test in production technical changes that could cause incompatible results in Sim09 before deploying them there. Release of the Gauss v50rX series are made from this branch that is based on more recent versions of Gaudi/LHCb but on the same fixed version of Pythia8 and Geant4 as those of Sim09.
- Simulation specific development for the next Sim10 (and further) are made in the master branch and include new versions of Pythia8 and Geant4 10 as well as the latest version of Gaudi and LHCb.

The master and official branches are all protected, meaning that code cannot be pushed into them directly but only through merge requests. 
This allows us to check things work correctly before including them in the official branches for the next release.


# Contributions to the Gauss GitLab project

When you contribute new or modified code to Gauss ensure to start by getting a copy of the package(s) you need to modify from the branch you intend this code to be for. 
Due to the fact that the majority of people contributing code will want it to be deployed in Sim09, and you should specify this branch for your pull and merge requests. 

For example if you want to contribute a new generator levels cut for Sim09, once you have your satellite project, you can just check out the package from the Sim09 branch
```
git lb-checkout Gauss/Sim09 Gen/GenCuts
```
You can then work on the changes you want to make and commit them to your local copy as much as you want.
You do not need to put entries in the release notes of the package any more but you should put meaningful message when you commit to your local copy as they will be propagated to the remote repository.

Once you are happy with your changes you can push them to a branch in the Gauss remote repository.
The name of the branch should start with the JIRA ticket you created in the LHCBGAUSS Jira (LHCBGAUSS-1345 in the example below) 
```
git lb-push Gauss LHCBGAUSS-1345
```
and can optionally be followed by one more human readable word separated with a “.” (.CutForUpsilon), e.g:
```
git lb-push Gauss LHCBGAUSS-1345.CutForUpsilon
```
You must create the Jira task with a short description of the issue before pushing to the remote Gauss repository, and we recommend you do so early on in the process as we can provide general suggestions and comments before hand.

Once the branch has been pushed to the repository other people can look at the code and contribute to the development by using the same branch.

When you are ready for the code to be tested with the next Gauss candidate release for a given branch you have to create a merge request.
For Gauss you do not need to start with WIP the title of the merge request’s. You can start it with the jira ticket (LHCBGAUSS-1345 in the example above). If you do not do so put the Jira ticket in the description.
This will introduce a direct link between the Jira task and the merge request that will later be carried on in the release notes and web documentation.

When creating the merge request you need to select the branch you want this to be applied to. The default is master so be sure to change it to either Sim09 or Sim09-upgrade if you need to. In case you want the code to be also propagated to those you should add a comment to this extent into the merge request. The Gauss manager will take care of doing so to start with and give instructions on how to do so yourself once we are all more GitLab proficient.

# Testing and reviewing the code you submitted
In order to test the code provided for new releases we have various nightly slots. In particular for Gauss we have setup two dedicated nightly slots: 
- **lhcb-sim09**, for the continuous build of the next Gauss v49rX release
- **lhcb-sim09-upgrade**, with the same scope for the next Gauss v50rX

Gauss in lhcb-sim09 is built from the head of the Sim09 GitLab branch,
while the one in lhcb-sim09-upgrade is built from the Sim09-upgrade branch.

_All merge requests for a given branch are automatically included in the corresponding nightly build_ to ensure they can compile correctly. In case the code of the merge request does not compile it will be exclude from future builds until a fix is provided.

The person asking for a merge request should verify that the code provides the expected results by running Gauss from the corresponding nightly build, e.g. for Sim09 by doing:
```
lb-run --nightly lhcb-sim09 Gauss/Sim09 gaudirun.py MyOptions.py
```
In parallel someone will be asked to review the code to ensure someone else an idea of what it does and it fits within the Gauss framework and the underlying LHCb and Gaudi software.

If changes to the code are necessary due to compilation problems, unexpected test results, review comments, they should be introduce in the branch in which the code has been submitted (LHCBGAUSS-1234 in the example above)


