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
import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'Gaussino'
copyright = '2017-2022, CERN for the benefit of the LHCb and FCC collaborations'
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
    'sphinx.ext.autosectionlabel',
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

html_logo = 'images/gaussino_logo.png'

html_theme_options = {
    'logo_only': True,
    'style_nav_header_background': '#343131',
}

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

html_context['display_lower_left'] = True

# visible versions
versions = [
    'master',
    'v0r0',
    'FASTSIM',
]

current_version = os.getenv("DOCS_VERSION", "master")

# tell the theme which version we're currently on ('current_version' affects
# the lower-left rtd menu and 'version' affects the logo-area version)
html_context['current_version'] = current_version
html_context['version'] = current_version

# POPULATE LINKS TO OTHER VERSIONS
html_context['versions'] = list()

# Auto-generated header anchors
myst_heading_anchors = 3

todo_include_todos = True

autosectionlabel_prefix_document = True

for version in versions:
    html_context['versions'].append((version, '/' + version + '/'))


def hide_non_private(app, what, name, obj, skip, options):
    """Filters out configurable properties. Not the best way of doing
    this, as it filters all public members that start with a capital
    letter. Maybe a better way of doing this should be enforced.
    """
    if ('members' in options and name and name[0].isupper()
            and name in options['members']):
        return True
    if name in ["configurables", "_properties"]:
        return True
    return None


def setup(app):
    app.connect('autodoc-skip-member', hide_non_private)
