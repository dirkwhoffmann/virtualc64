# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import sys
sys.path.append("/opt/local/bin/")

project = 'VirtualC64'
copyright = '2024, Dirk W. Hoffmann'
author = 'Dirk W. Hoffmann'
release = '5.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.githubpages',
    'sphinx.ext.imgmath',
    'sphinx.ext.todo',
    'myst_parser',
    'breathe'
]

breathe_projects = {"myproject": "/Users/hoff/Retro/virtualc64/Docs/Doxygen/xml"}
breathe_default_project = "myproject"

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
source_parsers = {
    '.md': 'recommonmark.parser.CommonMarkParser',
}
source_suffix = ['.rst', '.md']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'insipid'
html_theme_options = {
    'initial_sidebar_visibility_threshold': 0,
    'breadcrumbs': False,
}
html_logo = "images/icon.png"
html_title = "VirtualC64 5.0"



