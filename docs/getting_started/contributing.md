# Contributing

The future framework (for LHCb) consists of two components, Gaussino and Gauss. Gaussino contains all core functionality which should be independent of LHCb. It should incorporate all framework related issues and define all necessary interfaces and implement the interplay between them. Gauss itself is based on Gaussino and should only contain the specific implementations of the interfaces defined in Gaussino and the necessary configuration to setup the application. 

## Developing Gaussino

Please make sure that you followed the recipe described in [](./gaussino.md#working-with-gaussino) and make sure that you work on your local development branch.

Changes should be introduced via merge requests targeting the `master` branch. If you wish to open a MR with your local branch (the name of the branch should start with your username, e.g. `username-some-changes`), then you can directly push the changes to the branch in the remote repository:

```shell
git add file1 file2
git commit -m "The description of your commit"
git push origin username-some-changes
```  

In gitlab, open a MR targeting the `master` branch. Adding the `lhcb-gaussino` label will include your changes in the next nightly build of `lhcb-gaussino`.

## Developing Gauss-on-Gaussino

The instructions are almost the same as for Gaussino in the previous section: [](./contributing.md#developing-gaussino). The only difference is that the MR should be targeting `Futurev4` branch.

## Fast simulation developments with Geant4 10.7

All the developments related to fast simulations with Geant4 10.7 are tested in the `lhcb-gaussino-fastsim` nightly slot. We have a dedicated target branch in Gaussino and Gauss-on-Gaussino for these developments: `FASTSIM`. Therefore, you can follow the instructions as in [](./gaussino.md#working-with-gaussino), [](./gauss.md#working-with-gauss-on-gaussino-to-be-moved), [](./contributing.md#developing-gaussino) and [](./contributing.md#developing-gauss-on-gaussino) and change `lhcb-gaussino` to `lhcb-gaussino-fastsim` and `master` to `FASTSIM` (in Gauss: `Futurev4` to `FASTSIM`). 

## Documentation

1. Add a `Documentation` label to your MR. This will activate the gitlab CI pipeline.

2. Edit or add new markdown (`.md`) or reStructured Text (`.rst`) files in the `docs/` repository and make sure they are listed in the `index.rst` file. You can mix both by creating a markdown file and then putting rst in the following format:

```
```{eval-rst}
Your reStructured Text in markdown.
```{}
```

3. Your documentation should be accessible at `https://lhcbdoc.web.cern.ch/lhcbdoc/gaussino/name-of-your-branch/index.html`

4. You can test the documentation locally by running `gitlab-runner exec shell build-docs`. Standard `make docs` will not work, as we need to add python paths of other projects.
