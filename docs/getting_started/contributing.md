# Contributing

Gaussino contains all the core functionalities which should be independent of any experiment. It should incorporate all the framework related issues, define all the necessary interfaces and implement the interplay between them.
## Developing Gaussino

Please make sure that you followed the recipe described in [](./installation.md#installation) and make sure that you work on your local development branch.

Changes should be introduced via merge requests targeting the `master` branch. If you wish to open a MR with your local branch (the name of the branch should start with your username, e.g. `username-some-changes`), then you can directly push the changes to the branch in the remote repository:

```shell
git add file1 file2
git commit -m "The description of your commit"
git push origin username-some-changes
```

In gitlab, open a MR targeting the `master` branch.

## Testing your MR in the nightly system

Adding a label with the name of nightly slot in which you wish to test your MR, will include it in the next build of this slot. For example, if you add `lhcb-gaussino` label, your MR will be included in the next nightly build of `lhcb-gaussino`.

## Documentation

1. Add a `Python Documentation` label to your MR. This will activate the gitlab CI pipeline.

2. Edit or add new markdown (`.md`) or reStructured Text (`.rst`) files in the `docs/` repository and make sure they are listed in the `index.rst` file. You can mix both by creating a markdown file and then putting rst in the following format:

```
```{eval-rst}
Your reStructured Text in markdown.
```{}
```

3. Your documentation should be accessible at `https://gaussino.docs.cern.ch/name-of-your-branch/index.html`

4. You can test the documentation locally by running `gitlab-runner exec shell build-docs`.