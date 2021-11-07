# Contributing

The future framework (for LHCb) consists of two components, Gaussino and Gauss. Gaussino contains all core functionality which should be independent of LHCb. It should incorporate all framework related issues and define all necessary interfaces and implement the interplay between them. Gauss itself is based on Gaussino and should only contain the specific implementations of the interfaces defined in Gaussino and the necessary configuration to setup the application. Changes should be introduced via merge requests targetting the `master` branch. Adding the label `lhcb-gaussino` will include them in the nightly build.

## Documenting

1. Add a `Documentation` label to your MR. This will activate the gitlab CI pipeline.

2. Edit or add new markdown (`.md`) or reStructured Text (`.rst`) files in the `docs/` repository and make sure they are listed in the `index.rst` file. You can mix both by creating a markdown file and then putting rst in the following format:

```
```{eval-rst}
Your reStructured Text in markdown.
```{}
```

3. Your documentation should be accessible at `https://lhcbdoc.web.cern.ch/lhcbdoc/gaussino/name-of-your-branch/index.html`

4. You can test the documentation locally by running `gitlab-runner exec shell build-docs`. Standard `make docs` will not work, as we need to add python paths of other projects.
