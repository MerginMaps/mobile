# see https://cmake-format.readthedocs.io/en/latest/configuration.html

# ----------------------------------
# Options affecting listfile parsing
# ----------------------------------
with section("parse"):
  additional_commands = { 'qt_add_qml_module': 
    { 'pargs': [1, '+'],
      'kwargs': { 'URI': 1,
                  'VERSION': 1,
                  'QML_FILES': '*',
                  'RESOURCE_PREFIX': 1
                }
    }
  }


# -----------------------------
# Options effecting formatting.
# -----------------------------


with section("format"): 
  # How wide to allow formatted cmake files
  line_width = 90

  # How many spaces to tab for indent
  tab_size = 2

  # If true, separate flow control names from their parentheses with a space
  separate_ctrl_name_with_space = True

  # If true, separate function names from parentheses with a space
  separate_fn_name_with_space = False

  # If a statement is wrapped to more than one line, than dangle the closing
  # parenthesis on its own line.
  dangle_parens = True
  
  # Format command names consistently as 'lower' or 'upper' case
  command_case = 'lower'
  
  # Format keywords consistently as 'lower' or 'upper' case
  keyword_case = 'upper'

  # If a positional argument group contains more than this many arguments, then
  # force it to a vertical layout.
  max_pargs_hwrap = 3
