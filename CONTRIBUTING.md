# Contributions to the Gauss GitLab project

When you contribute new or modified code to Gauss ensure to start by getting a copy of the package(s) you need to modify from the branch you 
intend this code to be for. 
Due to the fact that the majority of people contributing code will want it to be deployed in Sim09, and you should specify this branch for 
your pull and merge requests. 

For example if you want to contribute a new generator levels cut for Sim09, once you have your satellite project, you can just check out the 
package from the Sim09 branch
```
git lb-checkout Gauss/Sim09 Gen/GenCuts
```
You can then work on the changes you want to make and commit them to your local copy as much as you want.
You do not need to put entries in the release notes of the package any more but you should put meaningful message when you commit to your local 
copy as they will be propagated to the remote repository.

Once you are happy with your changes you can push them to a branch in the Gauss remote repository.
It is recommended you create a Jira task in the LHCBGAUSS Jira (LHCBGAUSS-NNNN in the example below) with a short description of the issue 
before pushing to the remote Gauss repository, and we suggest
you do so early on in the process as we can provide general suggestions and comments before hand.

The name of the branch you commit your code to should start with the JIRA ticket you created you task to which this code is related to
and be followed by one more human readable word separated with a “.” (.CutForUpsilon), e.g:
```
git lb-push Gauss LHCBGAUSS-NNNN.WhatTheCodeIsFor
```

Once the branch has been pushed to the repository other people can look at the code and contribute to the development by using the same branch.

When you are ready for the code to be tested with the next Gauss candidate release for a given branch you have to create a merge request.
For Gauss you do not need to start with WIP the title of the merge request’s. Please put a meaningull tile and the Jira ticket in the description,
e.g. this refers to / closes / see / details in LHCBGAUSS-NNNN
This will introduce a direct link between the Jira task and the merge request that will later be carried on in the release notes 
and web documentation.

When creating the merge request you need to first *select the branch you want this to be applied to*. The default is master so be sure to change
it to either Sim09 or Sim09-upgrade if you need to. 
In case you want the code to be also propagated to those you should add a comment to this extent into the merge request. 
The Gauss manager will take care of doing so.

# Testing and reviewing the code you submitted
In order to test the code provided for new releases we have various nightly slots. In particular for Gauss we have setup two dedicated nightly slots: 
- **lhcb-sim09**, for the continuous build of the next Gauss v49rX release
- **lhcb-sim09-upgrade**, with the same scope for the next Gauss v50rX
- **lhcb-gauss-dev, for the continuos build of the master branch and the bext Gauss v52rX (Sim10 preparation)

Gauss in lhcb-sim09 is built from the head of the Sim09 GitLab branch,
while the one in lhcb-sim09-upgrade is built from the Sim09-upgrade branch,
and the one in lhcb-gauss-dev is built from master

_All merge requests for a given branch are automatically included in the corresponding nightly build_ to ensure they can compile correctly. 
In case the code of the merge request does not compile it will be exclude from future builds until a fix is provided.

The person asking for a merge request should verify that the code provides the expected results by running Gauss from the corresponding 
nightly build, e.g. for Sim09 by doing:
```
lb-run --nightly lhcb-sim09 Gauss/Sim09 gaudirun.py MyOptions.py
```
In parallel someone will be asked to review the code to ensure someone else an idea of what it does and it fits within the Gauss framework 
and the underlying LHCb and Gaudi software.

If changes to the code are necessary due to compilation problems, unexpected test results, review comments, they should be introduce in the 
branch in which the code has been submitted (LHCBGAUSS-NNNN.WhatTheCodeIsFor in the example above)


