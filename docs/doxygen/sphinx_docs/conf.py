project = 'csalt-lib'
copyright = '2025, Jonathan A. Webb'
author = 'Jonathan A. Webb'
release = '0.0.2'

# -- General configuration ---------------------------------------------------

extensions = [
    "sphinx.ext.todo",
    "sphinx.ext.viewcode",
    "sphinx.ext.autodoc",
    "sphinx.ext.autosummary",
    "sphinx.ext.githubpages",
    "sphinx_copybutton",
    "breathe",
]

templates_path = ["_templates"]
exclude_patterns = []

# Sphinx behavior
autodoc_member_order = "groupwise"
autodoc_default_options = {
    "members": True,
    "show-inheritance": True,
}
autosummary_generate = True
todo_include_todos = True

# -- Breathe / Doxygen configuration -----------------------------------------

from pathlib import Path

HERE = Path(__file__).parent.resolve()
DOXYGEN_XML_DIR = HERE.parent / "build" / "xml"

breathe_default_project = "csalt"
breathe_projects = {
    "csalt": str(DOXYGEN_XML_DIR),
}

# Tell Breathe that .c files are implementation files so it prefers header
# declarations when resolving .. doxygenfunction:: directives.
breathe_implementation_filename_extensions = [".c", ".cc", ".cpp"]

# Map file extensions to the C domain
breathe_domain_by_extension = {
    "h": "c",
    "c": "c",
}

# Optional sanity check
if not DOXYGEN_XML_DIR.exists():
    print(
        f"[conf.py] WARNING: Doxygen XML not found at {DOXYGEN_XML_DIR}. "
        "Run 'doxygen Doxyfile' before building Sphinx."
    )

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_theme_options = {
    "navigation_depth": 4,
}

