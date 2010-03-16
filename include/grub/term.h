/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003,2005,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRUB_TERM_HEADER
#define GRUB_TERM_HEADER	1

/* Internal codes used by GRUB to represent terminal input.  */
#define GRUB_TERM_LEFT		2
#define GRUB_TERM_RIGHT		6
#define GRUB_TERM_UP		16
#define GRUB_TERM_DOWN		14
#define GRUB_TERM_HOME		1
#define GRUB_TERM_END		5
#define GRUB_TERM_DC		4
#define GRUB_TERM_PPAGE		7
#define GRUB_TERM_NPAGE		3
#define GRUB_TERM_ESC		'\e'
#define GRUB_TERM_TAB		'\t'
#define GRUB_TERM_BACKSPACE	8

#ifndef ASM_FILE

#include <grub/err.h>
#include <grub/symbol.h>
#include <grub/types.h>
#include <grub/handler.h>
#include <grub/unicode.h>

/* These are used to represent the various color states we use.  */
typedef enum
  {
    /* The color used to display all text that does not use the
       user defined colors below.  */
    GRUB_TERM_COLOR_STANDARD,
    /* The user defined colors for normal text.  */
    GRUB_TERM_COLOR_NORMAL,
    /* The user defined colors for highlighted text.  */
    GRUB_TERM_COLOR_HIGHLIGHT
  }
grub_term_color_state;

/* Flags for representing the capabilities of a terminal.  */
/* Some notes about the flags:
   - These flags are used by higher-level functions but not terminals
   themselves.
   - If a terminal is dumb, you may assume that only putchar, getkey and
   checkkey are called.
   - Some fancy features (setcolorstate, setcolor and setcursor) can be set
   to NULL.  */

/* Set when input characters shouldn't be echoed back.  */
#define GRUB_TERM_NO_ECHO	        (1 << 0)
/* Set when the editing feature should be disabled.  */
#define GRUB_TERM_NO_EDIT	        (1 << 1)
/* Set when the terminal cannot do fancy things.  */
#define GRUB_TERM_DUMB		        (1 << 2)
/* Which encoding does terminal expect stream to be.  */
#define GRUB_TERM_CODE_TYPE_MASK	        ((1 << 5) | (1 << 4) | (1 << 3))
/* Only ASCII characters accepted.  */
#define GRUB_TERM_CODE_TYPE_ASCII	        0
/* Expects VGA characters (ASCII + pseudographics).  */
#define GRUB_TERM_CODE_TYPE_VGA	                (1 << 3)
/* UTF-8 stream in logical order. Usually used for terminals
   which just forward the stream to another computer.  */
#define GRUB_TERM_CODE_TYPE_UTF8_LOGICAL	(1 << 4)
/* UCS-4 in visual order.  */
#define GRUB_TERM_CODE_TYPE_UCS4_VISUAL	        ((1 << 4) | (1 << 3))
#define GRUB_TERM_CODE_TYPE_UTF8_VISUAL	        ((1 << 5) | (1 << 4))


/* Bitmasks for modifier keys returned by grub_getkeystatus.  */
#define GRUB_TERM_STATUS_SHIFT	(1 << 0)
#define GRUB_TERM_STATUS_CTRL	(1 << 1)
#define GRUB_TERM_STATUS_ALT	(1 << 2)


/* Unicode characters for fancy graphics.  */
#define GRUB_TERM_DISP_LEFT	0x2190
#define GRUB_TERM_DISP_UP	0x2191
#define GRUB_TERM_DISP_RIGHT	0x2192
#define GRUB_TERM_DISP_DOWN	0x2193
#define GRUB_TERM_DISP_HLINE	0x2501
#define GRUB_TERM_DISP_VLINE	0x2503
#define GRUB_TERM_DISP_UL	0x250F
#define GRUB_TERM_DISP_UR	0x2513
#define GRUB_TERM_DISP_LL	0x2517
#define GRUB_TERM_DISP_LR	0x251B


/* Menu-related geometrical constants.  */

/* The number of lines of "GRUB version..." at the top.  */
#define GRUB_TERM_INFO_HEIGHT	1

/* The number of columns/lines between messages/borders/etc.  */
#define GRUB_TERM_MARGIN	1

/* The number of columns of scroll information.  */
#define GRUB_TERM_SCROLL_WIDTH	1

/* The Y position of the top border.  */
#define GRUB_TERM_TOP_BORDER_Y	(GRUB_TERM_MARGIN + GRUB_TERM_INFO_HEIGHT \
                                 + GRUB_TERM_MARGIN)

/* The X position of the left border.  */
#define GRUB_TERM_LEFT_BORDER_X	GRUB_TERM_MARGIN

/* The number of lines of messages at the bottom.  */
#define GRUB_TERM_MESSAGE_HEIGHT	8

/* The Y position of the first entry.  */
#define GRUB_TERM_FIRST_ENTRY_Y	(GRUB_TERM_TOP_BORDER_Y + 1)

struct grub_term_input
{
  /* The next terminal.  */
  struct grub_term_input *next;

  /* The terminal name.  */
  const char *name;

  /* Initialize the terminal.  */
  grub_err_t (*init) (void);

  /* Clean up the terminal.  */
  grub_err_t (*fini) (void);

  /* Check if any input character is available.  */
  int (*checkkey) (void);

  /* Get a character.  */
  int (*getkey) (void);

  /* Get keyboard modifier status.  */
  int (*getkeystatus) (void);
};
typedef struct grub_term_input *grub_term_input_t;

struct grub_term_output
{
  /* The next terminal.  */
  struct grub_term_output *next;

  /* The terminal name.  */
  const char *name;

  /* Initialize the terminal.  */
  grub_err_t (*init) (void);

  /* Clean up the terminal.  */
  grub_err_t (*fini) (void);

  /* Put a character. C is encoded in Unicode.  */
  void (*putchar) (const struct grub_unicode_glyph *c);

  /* Get the number of columns occupied by a given character C. C is
     encoded in Unicode.  */
  grub_ssize_t (*getcharwidth) (const struct grub_unicode_glyph *c);

  /* Get the screen size. The return value is ((Width << 8) | Height).  */
  grub_uint16_t (*getwh) (void);

  /* Get the cursor position. The return value is ((X << 8) | Y).  */
  grub_uint16_t (*getxy) (void);

  /* Go to the position (X, Y).  */
  void (*gotoxy) (grub_uint8_t x, grub_uint8_t y);

  /* Clear the screen.  */
  void (*cls) (void);

  /* Set the current color to be used */
  void (*setcolorstate) (grub_term_color_state state);

  /* Set the normal color and the highlight color. The format of each
     color is VGA's.  */
  void (*setcolor) (grub_uint8_t normal_color, grub_uint8_t highlight_color);

  /* Get the normal color and the highlight color. The format of each
     color is VGA's.  */
  void (*getcolor) (grub_uint8_t *normal_color, grub_uint8_t *highlight_color);

  /* Turn on/off the cursor.  */
  void (*setcursor) (int on);

  /* Update the screen.  */
  void (*refresh) (void);

  /* The feature flags defined above.  */
  grub_uint32_t flags;
};
typedef struct grub_term_output *grub_term_output_t;

extern struct grub_term_output *EXPORT_VAR(grub_term_outputs_disabled);
extern struct grub_term_input *EXPORT_VAR(grub_term_inputs_disabled);
extern struct grub_term_output *EXPORT_VAR(grub_term_outputs);
extern struct grub_term_input *EXPORT_VAR(grub_term_inputs);

static inline void
grub_term_register_input (const char *name __attribute__ ((unused)),
			  grub_term_input_t term)
{
  if (grub_term_inputs)
    grub_list_push (GRUB_AS_LIST_P (&grub_term_inputs_disabled),
		    GRUB_AS_LIST (term));
  else
    {
      /* If this is the first terminal, enable automatically.  */
      if (! term->init || term->init () == GRUB_ERR_NONE)
	grub_list_push (GRUB_AS_LIST_P (&grub_term_inputs), GRUB_AS_LIST (term));
    }
}

static inline void
grub_term_register_output (const char *name __attribute__ ((unused)),
			   grub_term_output_t term)
{
  if (grub_term_outputs)
    grub_list_push (GRUB_AS_LIST_P (&grub_term_outputs_disabled),
		    GRUB_AS_LIST (term));
  else
    {
      /* If this is the first terminal, enable automatically.  */
      if (! term->init || term->init () == GRUB_ERR_NONE)
	grub_list_push (GRUB_AS_LIST_P (&grub_term_outputs),
			GRUB_AS_LIST (term));
    }
}

static inline void
grub_term_unregister_input (grub_term_input_t term)
{
  grub_list_remove (GRUB_AS_LIST_P (&grub_term_inputs), GRUB_AS_LIST (term));
  grub_list_remove (GRUB_AS_LIST_P (&grub_term_inputs_disabled),
		    GRUB_AS_LIST (term));
}

static inline void
grub_term_unregister_output (grub_term_output_t term)
{
  grub_list_remove (GRUB_AS_LIST_P (&grub_term_outputs), GRUB_AS_LIST (term));
  grub_list_remove (GRUB_AS_LIST_P (&(grub_term_outputs_disabled)),
		    GRUB_AS_LIST (term));
}

#define FOR_ACTIVE_TERM_INPUTS(var) for (var = grub_term_inputs; var; var = var->next)
#define FOR_DISABLED_TERM_INPUTS(var) for (var = grub_term_inputs_disabled; var; var = var->next)
#define FOR_ACTIVE_TERM_OUTPUTS(var) for (var = grub_term_outputs; var; var = var->next)
#define FOR_DISABLED_TERM_OUTPUTS(var) for (var = grub_term_outputs_disabled; var; var = var->next)

void grub_putcode (grub_uint32_t code, struct grub_term_output *term);
int EXPORT_FUNC(grub_getkey) (void);
int EXPORT_FUNC(grub_checkkey) (void);
int EXPORT_FUNC(grub_getkeystatus) (void);
void EXPORT_FUNC(grub_cls) (void);
void EXPORT_FUNC(grub_setcolorstate) (grub_term_color_state state);
void EXPORT_FUNC(grub_refresh) (void);
void grub_puts_terminal (const char *str, struct grub_term_output *term);
grub_uint16_t *grub_term_save_pos (void);
void grub_term_restore_pos (grub_uint16_t *pos);

static inline unsigned grub_term_width (struct grub_term_output *term)
{
  return ((term->getwh()&0xFF00)>>8);
}

static inline unsigned grub_term_height (struct grub_term_output *term)
{
  return (term->getwh()&0xFF);
}

/* The width of the border.  */
static inline unsigned
grub_term_border_width (struct grub_term_output *term)
{
  return grub_term_width (term) - GRUB_TERM_MARGIN * 3 - GRUB_TERM_SCROLL_WIDTH;
}

/* The max column number of an entry. The last "-1" is for a
   continuation marker.  */
static inline int
grub_term_entry_width (struct grub_term_output *term)
{
  return grub_term_border_width (term) - 2 - GRUB_TERM_MARGIN * 2 - 1;
}

/* The height of the border.  */

static inline unsigned
grub_term_border_height (struct grub_term_output *term)
{
  return grub_term_height (term) - GRUB_TERM_TOP_BORDER_Y
    - GRUB_TERM_MESSAGE_HEIGHT;
}

/* The number of entries shown at a time.  */
static inline int
grub_term_num_entries (struct grub_term_output *term)
{
  return grub_term_border_height (term) - 2;
}

static inline int
grub_term_cursor_x (struct grub_term_output *term)
{
  return (GRUB_TERM_LEFT_BORDER_X + grub_term_border_width (term) 
	  - GRUB_TERM_MARGIN - 1);
}

static inline grub_uint16_t
grub_term_getxy (struct grub_term_output *term)
{
  return term->getxy ();
}

static inline void
grub_term_refresh (struct grub_term_output *term)
{
  if (term->refresh)
    term->refresh ();
}

static inline void
grub_term_gotoxy (struct grub_term_output *term, grub_uint8_t x, grub_uint8_t y)
{
  term->gotoxy (x, y);
}

static inline void 
grub_term_setcolorstate (struct grub_term_output *term, 
			 grub_term_color_state state)
{
  if (term->setcolorstate)
    term->setcolorstate (state);
}

  /* Set the normal color and the highlight color. The format of each
     color is VGA's.  */
static inline void 
grub_term_setcolor (struct grub_term_output *term,
		    grub_uint8_t normal_color, grub_uint8_t highlight_color)
{
  if (term->setcolor)
    term->setcolor (normal_color, highlight_color);
}

/* Turn on/off the cursor.  */
static inline void 
grub_term_setcursor (struct grub_term_output *term, int on)
{
  if (term->setcursor)
    term->setcursor (on);
}

static inline void 
grub_term_cls (struct grub_term_output *term)
{
  if (term->cls)
    (term->cls) ();
  else
    {
      grub_putcode ('\n', term);
      grub_term_refresh (term);
    }
}

static inline grub_ssize_t 
grub_term_getcharwidth (struct grub_term_output *term,
			const struct grub_unicode_glyph *c)
{
  if (term->getcharwidth)
    return term->getcharwidth (c);
  else
    return 1;
}

static inline void 
grub_term_getcolor (struct grub_term_output *term, 
		    grub_uint8_t *normal_color, grub_uint8_t *highlight_color)
{
  if (term->getcolor)
    term->getcolor (normal_color, highlight_color);
  else
    {
      *normal_color = 0x07;
      *highlight_color = 0x07;
    }
}

extern void (*EXPORT_VAR (grub_newline_hook)) (void);

struct grub_term_autoload
{
  struct grub_term_autoload *next;
  char *name;
  char *modname;
};

extern struct grub_term_autoload *grub_term_input_autoload;
extern struct grub_term_autoload *grub_term_output_autoload;

static inline void
grub_print_spaces (struct grub_term_output *term, int number_spaces)
{
  while (--number_spaces >= 0)
    grub_putcode (' ', term);
}


/* For convenience.  */
#define GRUB_TERM_ASCII_CHAR(c)	((c) & 0xff)

#endif /* ! ASM_FILE */

#endif /* ! GRUB_TERM_HEADER */
