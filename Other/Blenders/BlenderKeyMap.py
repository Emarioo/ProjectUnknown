keyconfig_version = (3, 2, 14)
keyconfig_data = \
[("Text",
  {"space_type": 'TEXT_EDITOR', "region_type": 'WINDOW'},
  {"items":
   [("wm.context_cycle_int",
     {"type": 'WHEELUPMOUSE', "value": 'PRESS', "ctrl": True},
     {"properties":
      [("data_path", 'space_data.font_size'),
       ("reverse", False),
       ],
      },
     ),
    ("wm.context_cycle_int",
     {"type": 'WHEELDOWNMOUSE', "value": 'PRESS', "ctrl": True},
     {"properties":
      [("data_path", 'space_data.font_size'),
       ("reverse", True),
       ],
      },
     ),
    ("wm.context_cycle_int",
     {"type": 'NUMPAD_PLUS', "value": 'PRESS', "ctrl": True, "repeat": True},
     {"properties":
      [("data_path", 'space_data.font_size'),
       ("reverse", False),
       ],
      },
     ),
    ("wm.context_cycle_int",
     {"type": 'NUMPAD_MINUS', "value": 'PRESS', "ctrl": True, "repeat": True},
     {"properties":
      [("data_path", 'space_data.font_size'),
       ("reverse", True),
       ],
      },
     ),
    ("text.new", {"type": 'N', "value": 'PRESS', "alt": True}, None),
    ("text.open", {"type": 'O', "value": 'PRESS', "alt": True}, None),
    ("text.reload", {"type": 'R', "value": 'PRESS', "alt": True}, None),
    ("text.save", {"type": 'S', "value": 'PRESS', "alt": True}, None),
    ("text.save_as", {"type": 'S', "value": 'PRESS', "shift": True, "ctrl": True, "alt": True}, None),
    ("text.run_script", {"type": 'P', "value": 'PRESS', "alt": True}, None),
    ("text.cut", {"type": 'X', "value": 'PRESS', "ctrl": True}, None),
    ("text.copy", {"type": 'C', "value": 'PRESS', "ctrl": True}, None),
    ("text.paste", {"type": 'V', "value": 'PRESS', "ctrl": True, "repeat": True}, None),
    ("text.cut", {"type": 'DEL', "value": 'PRESS', "shift": True}, None),
    ("text.copy", {"type": 'INSERT', "value": 'PRESS', "ctrl": True}, None),
    ("text.paste", {"type": 'INSERT', "value": 'PRESS', "shift": True, "repeat": True}, None),
    ("text.duplicate_line", {"type": 'D', "value": 'PRESS', "shift": True, "ctrl": True, "repeat": True}, None),
    ("text.select_all", {"type": 'A', "value": 'PRESS', "ctrl": True}, None),
    ("text.select_line", {"type": 'A', "value": 'PRESS', "shift": True, "ctrl": True}, None),
    ("text.select_word", {"type": 'LEFTMOUSE', "value": 'DOUBLE_CLICK'}, None),
    ("text.move_lines",
     {"type": 'UP_ARROW', "value": 'PRESS', "shift": True, "ctrl": True, "repeat": True},
     {"properties":
      [("direction", 'UP'),
       ],
      },
     ),
    ("text.move_lines",
     {"type": 'DOWN_ARROW', "value": 'PRESS', "shift": True, "ctrl": True, "repeat": True},
     {"properties":
      [("direction", 'DOWN'),
       ],
      },
     ),
    ("text.indent_or_autocomplete", {"type": 'TAB', "value": 'PRESS', "repeat": True}, None),
    ("text.unindent", {"type": 'TAB', "value": 'PRESS', "shift": True, "repeat": True}, None),
    ("text.comment_toggle", {"type": 'D', "value": 'PRESS', "ctrl": True}, None),
    ("text.move",
     {"type": 'HOME', "value": 'PRESS'},
     {"properties":
      [("type", 'LINE_BEGIN'),
       ],
      },
     ),
    ("text.move",
     {"type": 'END', "value": 'PRESS'},
     {"properties":
      [("type", 'LINE_END'),
       ],
      },
     ),
    ("text.move",
     {"type": 'E', "value": 'PRESS', "ctrl": True},
     {"properties":
      [("type", 'LINE_END'),
       ],
      },
     ),
    ("text.move",
     {"type": 'E', "value": 'PRESS', "shift": True, "ctrl": True},
     {"properties":
      [("type", 'LINE_END'),
       ],
      },
     ),
    ("text.move",
     {"type": 'LEFT_ARROW', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_CHARACTER'),
       ],
      },
     ),
    ("text.move",
     {"type": 'RIGHT_ARROW', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'NEXT_CHARACTER'),
       ],
      },
     ),
    ("text.move",
     {"type": 'LEFT_ARROW', "value": 'PRESS', "ctrl": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_WORD'),
       ],
      },
     ),
    ("text.move",
     {"type": 'RIGHT_ARROW', "value": 'PRESS', "ctrl": True, "repeat": True},
     {"properties":
      [("type", 'NEXT_WORD'),
       ],
      },
     ),
    ("text.move",
     {"type": 'UP_ARROW', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_LINE'),
       ],
      },
     ),
    ("text.move",
     {"type": 'DOWN_ARROW', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'NEXT_LINE'),
       ],
      },
     ),
    ("text.move",
     {"type": 'PAGE_UP', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_PAGE'),
       ],
      },
     ),
    ("text.move",
     {"type": 'PAGE_DOWN', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'NEXT_PAGE'),
       ],
      },
     ),
    ("text.move",
     {"type": 'HOME', "value": 'PRESS', "ctrl": True},
     {"properties":
      [("type", 'FILE_TOP'),
       ],
      },
     ),
    ("text.move",
     {"type": 'END', "value": 'PRESS', "ctrl": True},
     {"properties":
      [("type", 'FILE_BOTTOM'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'HOME', "value": 'PRESS', "shift": True},
     {"properties":
      [("type", 'LINE_BEGIN'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'END', "value": 'PRESS', "shift": True},
     {"properties":
      [("type", 'LINE_END'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'LEFT_ARROW', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_CHARACTER'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'RIGHT_ARROW', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'NEXT_CHARACTER'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'LEFT_ARROW', "value": 'PRESS', "shift": True, "ctrl": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_WORD'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'RIGHT_ARROW', "value": 'PRESS', "shift": True, "ctrl": True, "repeat": True},
     {"properties":
      [("type", 'NEXT_WORD'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'UP_ARROW', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_LINE'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'DOWN_ARROW', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'NEXT_LINE'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'PAGE_UP', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_PAGE'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'PAGE_DOWN', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'NEXT_PAGE'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'HOME', "value": 'PRESS', "shift": True, "ctrl": True},
     {"properties":
      [("type", 'FILE_TOP'),
       ],
      },
     ),
    ("text.move_select",
     {"type": 'END', "value": 'PRESS', "shift": True, "ctrl": True},
     {"properties":
      [("type", 'FILE_BOTTOM'),
       ],
      },
     ),
    ("text.delete",
     {"type": 'DEL', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'NEXT_CHARACTER'),
       ],
      },
     ),
    ("text.delete",
     {"type": 'BACK_SPACE', "value": 'PRESS', "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_CHARACTER'),
       ],
      },
     ),
    ("text.delete",
     {"type": 'BACK_SPACE', "value": 'PRESS', "shift": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_CHARACTER'),
       ],
      },
     ),
    ("text.delete",
     {"type": 'DEL', "value": 'PRESS', "ctrl": True, "repeat": True},
     {"properties":
      [("type", 'NEXT_WORD'),
       ],
      },
     ),
    ("text.delete",
     {"type": 'BACK_SPACE', "value": 'PRESS', "ctrl": True, "repeat": True},
     {"properties":
      [("type", 'PREVIOUS_WORD'),
       ],
      },
     ),
    ("text.overwrite_toggle", {"type": 'INSERT', "value": 'PRESS'}, None),
    ("text.scroll_bar", {"type": 'LEFTMOUSE', "value": 'PRESS'}, None),
    ("text.scroll_bar", {"type": 'MIDDLEMOUSE', "value": 'PRESS'}, None),
    ("text.scroll", {"type": 'MIDDLEMOUSE', "value": 'PRESS'}, None),
    ("text.scroll", {"type": 'TRACKPADPAN', "value": 'ANY'}, None),
    ("text.selection_set", {"type": 'LEFTMOUSE', "value": 'CLICK_DRAG'}, None),
    ("text.cursor_set", {"type": 'LEFTMOUSE', "value": 'PRESS'}, None),
    ("text.selection_set", {"type": 'LEFTMOUSE', "value": 'PRESS', "shift": True}, None),
    ("text.scroll",
     {"type": 'WHEELUPMOUSE', "value": 'PRESS'},
     {"properties":
      [("lines", -1),
       ],
      },
     ),
    ("text.scroll",
     {"type": 'WHEELDOWNMOUSE', "value": 'PRESS'},
     {"properties":
      [("lines", 1),
       ],
      },
     ),
    ("text.line_break", {"type": 'RET', "value": 'PRESS', "repeat": True}, None),
    ("text.line_break", {"type": 'NUMPAD_ENTER', "value": 'PRESS', "repeat": True}, None),
    ("text.line_number", {"type": 'TEXTINPUT', "value": 'ANY', "any": True, "repeat": True}, None),
    ("wm.call_menu",
     {"type": 'RIGHTMOUSE', "value": 'PRESS'},
     {"properties":
      [("name", 'TEXT_MT_context_menu'),
       ],
      },
     ),
    ("text.insert", {"type": 'TEXTINPUT', "value": 'ANY', "any": True, "repeat": True}, None),
    ],
   },
  ),
 ]


if __name__ == "__main__":
    # Only add keywords that are supported.
    from bpy.app import version as blender_version
    keywords = {}
    if blender_version >= (2, 92, 0):
        keywords["keyconfig_version"] = keyconfig_version
    import os
    from bl_keymap_utils.io import keyconfig_import_from_data
    keyconfig_import_from_data(
        os.path.splitext(os.path.basename(__file__))[0],
        keyconfig_data,
        **keywords,
    )
