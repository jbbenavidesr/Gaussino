# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'Gaussino'
copyright = '2021, LHCb Collaboration'
author = 'LHCb Collaboration'

# The full version, including alpha/beta/rc tags
release = 'v0r1'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx_rtd_theme',
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx.ext.viewcode',
    'sphinx.ext.todo',
    'myst_parser',
    'sphinx.ext.graphviz',
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

master_doc = 'index'

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

html_logo = 'gaussinologo.png'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Global file metadata
html_context = {
    'display_gitlab': True,
    'gitlab_host': 'gitlab.cern.ch',
    'gitlab_user': 'Gaussino',
    'gitlab_repo': 'Gaussino',
    'gitlab_version': 'master/docs/',
}

# Napoleon settings
napoleon_google_docstring = True
napoleon_numpy_docstring = True

# -- Custom Versioning settings -----------------------------------------------
# Implemented manually as READTHEDOCS handle them once already hosted

website_root = 'gaussino'
html_context['display_lower_left'] = True

# visible versions
versions = [
    'master',
    'v0r1',
]

# from git import Repo
# repo = Repo(search_parent_directories=True)
# current_version = repo.active_branch.name
current_version = 'master'

# tell the theme which version we're currently on ('current_version' affects
# the lower-left rtd menu and 'version' affects the logo-area version)
html_context['current_version'] = current_version
html_context['version'] = current_version

# POPULATE LINKS TO OTHER VERSIONS
html_context['versions'] = list()

for version in versions:
    html_context['versions'].append((version,
                                     '/' + website_root + '/' + version + '/'))

from importlib import import_module
from pprint import pformat
from docutils.parsers.rst import Directive
from docutils import nodes
from sphinx import addnodes


class PrettyDictionaryDirective(Directive):
    """Makes the dictionaries prettier"""
    required_arguments = 4

    def run(self):
        module_name = self.arguments[0]
        class_name = self.arguments[1]
        member_name = self.arguments[2]
        name_to_display = self.arguments[3]

        member = getattr(import_module(module_name), class_name)
        member = getattr(member, member_name)
        code = pformat(member, 2)

        literal = nodes.literal_block(code, code)
        literal['language'] = 'python'

        return [
            addnodes.desc_name(text=name_to_display),
            addnodes.desc_content('', literal)
        ]


def setup(app):
    app.add_directive('pretty-dict', PrettyDictionaryDirective)
