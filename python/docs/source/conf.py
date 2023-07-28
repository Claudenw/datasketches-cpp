# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import sys
import os 
#PATH = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../datasketches'))
sys.path.insert(0, os.path.abspath("../../datasketches"))
sys.path.insert(0, os.path.abspath("../../src"))
#print("Adding path:\n", PATH)
print("*****")
#sys.path.insert(0, PATH) #os.path.join(PATH, "cppyml"))
# sys.path.insert(0, os.path.join(PATH, "cppyml"))
# sys.path.insert(0, os.path.abspath('.'))
for sp in sys.path:
    print(sp)

project = 'datasketches'
copyright = '2023, charlie'
author = 'charlie'
release = '0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["sphinx.ext.autodoc","sphinx.ext.autosummary"]

templates_path = ['_templates']
exclude_patterns = []

autodoc_default_options = {
    'members': True,
    'member-order': 'bysource',
    'special-members': '__init__',
    'undoc-members': True,
    'exclude-members': '__weakref__'
}


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
