/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2002,2003,2004,2010  Free Software Foundation, Inc.
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

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/err.h>
#include <grub/legacy_parse.h>

struct legacy_command
{
  const char *name;
  const char *map;
  unsigned argc;
  enum arg_type {
    TYPE_VERBATIM,
    TYPE_FORCE_OPTION,
    TYPE_NOAPM_OPTION,
    TYPE_TYPE_OR_NOMEM_OPTION,
    TYPE_OPTION,
    TYPE_FILE,
    TYPE_FILE_NO_CONSUME,
    TYPE_PARTITION,
    TYPE_BOOL,
    TYPE_INT,
    TYPE_REST_VERBATIM
  } argt[4];
  enum {
    FLAG_IGNORE_REST = 1,
    FLAG_SUFFIX      = 2
  } flags;
  const char *shortdesc;
  const char *longdesc;
};

struct legacy_command legacy_commands[] =
  {
    {"blocklist", "blocklist '%s'\n", 1, {TYPE_FILE}, 0, "FILE",
     "Print the blocklist notation of the file FILE."},
    {"boot", "boot\n", 0, {}, 0, 0,
     "Boot the OS/chain-loader which has been loaded."},
    /* FIXME: bootp unsupported.  */
    {"cat", "cat '%s'\n", 1, {TYPE_FILE}, 0, "FILE",
     "Print the contents of the file FILE."},
    {"chainloader", "chainloader %s '%s'\n", 2, {TYPE_FORCE_OPTION, TYPE_FILE},
     0, "[--force] FILE",
     "Load the chain-loader FILE. If --force is specified, then load it"
     " forcibly, whether the boot loader signature is present or not."},
    {"cmp", "cmp '%s' '%s'\n", 2, {TYPE_FILE, TYPE_FILE}, FLAG_IGNORE_REST,
     "FILE1 FILE2",
     "Compare the file FILE1 with the FILE2 and inform the different values"
     " if any."},
    {"color", "legacy_color '%s' '%s'\n", 2, {TYPE_VERBATIM, TYPE_VERBATIM},
     FLAG_IGNORE_REST, "NORMAL [HIGHLIGHT]",
     "Change the menu colors. The color NORMAL is used for most"
     " lines in the menu, and the color HIGHLIGHT is used to highlight the"
     " line where the cursor points. If you omit HIGHLIGHT, then the"
     " inverted color of NORMAL is used for the highlighted line."
     " The format of a color is \"FG/BG\". FG and BG are symbolic color names."
     " A symbolic color name must be one of these: black, blue, green,"
     " cyan, red, magenta, brown, light-gray, dark-gray, light-blue,"
     " light-green, light-cyan, light-red, light-magenta, yellow and white."
     " But only the first eight names can be used for BG. You can prefix"
     " \"blink-\" to FG if you want a blinking foreground color."},
    {"configfile", "legacy_configfile '%s'\n", 1, {TYPE_FILE}, 0, "FILE",
     "Load FILE as the configuration file."},
    {"debug",
     "if [ -z \"$debug\" ]; then set debug=all; else set debug=; fi\n",
     0, {}, 0, 0, "Turn on/off the debug mode."},
    {"default",
     "set default='%s'; if [ x\"$default\" = xsaved ]; then load_env; "
     "set default=\"$saved_entry\"; fi\n", 1, {TYPE_VERBATIM}, 0, 
     "[NUM | `saved']",
     "Set the default entry to entry number NUM (if not specified, it is"
     " 0, the first entry) or the entry number saved by savedefault."},
    /* FIXME: dhcp unsupported.  */
    /* FIXME: displayapm unsupported.  */
    {"displaymem", "lsmmap\n", 0, {}, 0, 0, 
     "Display what GRUB thinks the system address space map of the"
     " machine is, including all regions of physical RAM installed."},
    /* NOTE: embed unsupported.  */
    {"fallback", "set fallback='%s'\n", 1, {TYPE_VERBATIM}, 0, "NUM...",
     "Go into unattended boot mode: if the default boot entry has any"
     " errors, instead of waiting for the user to do anything, it"
     " immediately starts over using the NUM entry (same numbering as the"
     " `default' command). This obviously won't help if the machine"
     " was rebooted by a kernel that GRUB loaded."},
    {"find", "search -sf '%s'\n", 1, {TYPE_FILE}, 0, "FILENAME",
     "Search for the filename FILENAME in all of partitions and print the list of"
     " the devices which contain the file."},
    /* FIXME: fstest unsupported.  */
    /* FIXME: geometry unsupported.  */
    {"halt", "halt %s\n", 1, {TYPE_NOAPM_OPTION}, 0, "[--no-apm]",
     "Halt your system. If APM is available on it, turn off the power using"
     " the APM BIOS, unless you specify the option `--no-apm'."},
    /* FIXME: help unsupported.  */    /* NUL_TERMINATE */
    {"hiddenmenu", "if sleep -i $timeout; then timeout=0; else timeout=-1; fi\n",
     0, {}, FLAG_SUFFIX, "", "Hide the menu."},
    {"hide", "parttool '%s' hidden+\n", 1, {TYPE_PARTITION}, 0, "PARTITION",
     "Hide PARTITION by setting the \"hidden\" bit in"
     " its partition type code."},
    /* FIXME: ifconfig unsupported.  */
    /* FIXME: impsprobe unsupported.  */
    {"initrd", "legacy_initrd '%s' %s\n", 2, {TYPE_FILE_NO_CONSUME,
					      TYPE_REST_VERBATIM}, 0,
     "FILE [ARG ...]",
     "Load an initial ramdisk FILE for a Linux format boot image and set the"
     " appropriate parameters in the Linux setup area in memory."},
    /* NOTE: install unsupported.  */
    /* FIXME: ioprobe unsupported.  */
    /* FIXME: really support --no-mem-option.  */
    {"kernel", "legacy_kernel %s %s '%s' %s\n", 4, {TYPE_TYPE_OR_NOMEM_OPTION,
						    TYPE_TYPE_OR_NOMEM_OPTION,
						    TYPE_FILE_NO_CONSUME,
						    TYPE_REST_VERBATIM}, 0,
     "[--no-mem-option] [--type=TYPE] FILE [ARG ...]",
     "Attempt to load the primary boot image from FILE. The rest of the"
     " line is passed verbatim as the \"kernel command line\".  Any modules"
     " must be reloaded after using this command. The option --type is used"
     " to suggest what type of kernel to be loaded. TYPE must be either of"
     " \"netbsd\", \"freebsd\", \"openbsd\", \"linux\", \"biglinux\" and"
     " \"multiboot\". The option --no-mem-option tells GRUB not to pass a"
     " Linux's mem option automatically."},
    /* FIXME: lock is unsupported. */
    {"makeactive", "parttool \"$root\" boot+\n", 0, {}, 0, 0,
     "Set the active partition on the root disk to GRUB's root device."
     " This command is limited to _primary_ PC partitions on a hard disk."},
    {"map", "drivemap '%s' '%s'\n", 2, {TYPE_PARTITION, TYPE_PARTITION},
     FLAG_IGNORE_REST, "TO_DRIVE FROM_DRIVE",
     "Map the drive FROM_DRIVE to the drive TO_DRIVE. This is necessary"
     " when you chain-load some operating systems, such as DOS, if such an"
     " OS resides at a non-first drive."},
    /* NOTE: md5crypt unsupported since GRUB has not enough entropy and this
       hash shouldn't be used anymore.  */
    {"module", "legacy_initrd '%s' %s\n", 1, {TYPE_FILE_NO_CONSUME,
					      TYPE_REST_VERBATIM}, 0,
     "FILE [ARG ...]",
     "Load a boot module FILE for a Multiboot format boot image (no"
     " interpretation of the file contents is made, so users of this"
     " command must know what the kernel in question expects). The"
     " rest of the line is passed as the \"module command line\", like"
     " the `kernel' command."},
    {"modulenounzip", "legacy_initrd_nounzip '%s' %s\n", 1,
     {TYPE_FILE_NO_CONSUME, TYPE_REST_VERBATIM}, 0,
     "FILE [ARG ...]",
     "The same as `module', except that automatic decompression is"
     " disabled."},
    /* FIXME: allow toggle.  */
    {"pager", "set pager=%d\n", 1, {TYPE_BOOL}, 0, "[FLAG]",
     "Toggle pager mode with no argument. If FLAG is given and its value"
     " is `on', turn on the mode. If FLAG is `off', turn off the mode."},
    /* FIXME: partnew unsupported.  */
    {"parttype", "parttool '%s' type=%s\n", 2, {TYPE_PARTITION, TYPE_INT}, 0,
     "PART TYPE", "Change the type of the partition PART to TYPE."},
    /* FIXME: support config file reloading.  */
    /* FIXME: support usage in menuentry.  */
    {"password", "if [ \"$superusers\" = "" ]; then superusers=legacy; fi;\n"
     "legacy_password %s '%s' %s", 3, {TYPE_OPTION, TYPE_VERBATIM,
				       TYPE_FILE}, FLAG_IGNORE_REST,
     "[--md5] PASSWD [FILE]",
     "If used in the first section of a menu file, disable all"
     " interactive editing control (menu entry editor and"
     " command line). If the password PASSWD is entered, it loads the"
     " FILE as a new config file and restarts the GRUB Stage 2. If you"
     " omit the argument FILE, then GRUB just unlocks privileged"
     " instructions.  You can also use it in the script section, in"
     " which case it will ask for the password, before continuing."
     " The option --md5 tells GRUB that PASSWD is encrypted with"
     " md5crypt."},
    /* NOTE: GRUB2 has a design principle of not eternally waiting for user
       input. 60 seconds should be enough.
     */
    {"pause", "echo %s; if ! sleep -i 60; then return; fi\n", 1,
     {TYPE_REST_VERBATIM}, 0,
     "[MESSAGE ...]", "Print MESSAGE, then wait until a key is pressed."},
    /* FIXME: rarp unsupported.  */
    {"read", "read_dword %s\n", 1, {TYPE_INT}, 0, "ADDR",
     "Read a 32-bit value from memory at address ADDR and"
     " display it in hex format."},
    {"reboot", "reboot\n", 0, {}, 0, 0, "Reboot your system."},
    /* FIXME: Support HDBIAS.  */
    /* FIXME: Support printing.  */
    {"root", "set root='%s'; set legacy_hdbias='%s'\n",
     2, {TYPE_PARTITION, TYPE_INT}, 0,
     "[DEVICE [HDBIAS]]",
     "Set the current \"root device\" to the device DEVICE, then"
     " attempt to mount it to get the partition size (for passing the"
     " partition descriptor in `ES:ESI', used by some chain-loaded"
     " bootloaders), the BSD drive-type (for booting BSD kernels using"
     " their native boot format), and correctly determine "
     " the PC partition where a BSD sub-partition is located. The"
     " optional HDBIAS parameter is a number to tell a BSD kernel"
     " how many BIOS drive numbers are on controllers before the current"
     " one. For example, if there is an IDE disk and a SCSI disk, and your"
     " FreeBSD root partition is on the SCSI disk, then use a `1' for HDBIAS."},
    {"rootnoverify", "set root='%s'; set legacy_hdbias='%s'\n",
     2, {TYPE_PARTITION, TYPE_INT}, 0,
     "[DEVICE [HDBIAS]]",
     "Similar to `root', but don't attempt to mount the partition. This"
     " is useful for when an OS is outside of the area of the disk that"
     " GRUB can read, but setting the correct root device is still"
     " desired. Note that the items mentioned in `root' which"
     " derived from attempting the mount will NOT work correctly."},
    /* FIXME: support saving NUM and fallback.  */
    {"savedefault", "saved_entry=${chosen}; save_env saved_entry\n", 0, {}, 0,
     "[NUM | `fallback']",
     "Save the current entry as the default boot entry if no argument is"
     " specified. If a number is specified, this number is saved. If"
     " `fallback' is used, next fallback entry is saved."},
    {"serial", "serial %s\n", 1, {TYPE_REST_VERBATIM}, 0, 
     "[--unit=UNIT] [--port=PORT] [--speed=SPEED] [--word=WORD] "
     "[--parity=PARITY] [--stop=STOP] [--device=DEV]",
     "Initialize a serial device. UNIT is a digit that specifies which serial"
     " device is used (e.g. 0 == COM1). If you need to specify the port number,"
     " set it by --port. SPEED is the DTE-DTE speed. WORD is the word length,"
     " PARITY is the type of parity, which is one of `no', `odd' and `even'."
     " STOP is the length of stop bit(s). The option --device can be used only"
     " in the grub shell, which specifies the file name of a tty device. The"
     " default values are COM1, 9600, 8N1."},
    /* FIXME: setkey unsupported.  */    /* NUL_TERMINATE */
    /* FIXME: setup unsupported.  */
    /* FIXME: terminal unsupported.  */    /* NUL_TERMINATE */
    /* FIXME: terminfo unsupported.  */    /* NUL_TERMINATE */
    {"testload", "cat '%s'\n", 1, {TYPE_FILE}, 0, "FILE",
     "Read the entire contents of FILE in several different ways and"
     " compares them, to test the filesystem code. "
     " If this test succeeds, then a good next"
     " step is to try loading a kernel."},
    /* FIXME: testvbe unsupported.  */
    /* FIXME: tftpserver unsupported.  */
    {"timeout", "set timeout=%s\n", 1, {TYPE_INT}, 0, "SEC",
     "Set a timeout, in SEC seconds, before automatically booting the"
     " default entry (normally the first entry defined)."},
    /* title is handled separately. */
    {"unhide", "parttool '%s' hidden-\n", 1, {TYPE_PARTITION}, 0, "PARTITION",
     "Unhide PARTITION by clearing the \"hidden\" bit in its"
     " partition type code."},
    /* FIXME: uppermem unsupported.  */
    {"uuid", "search -u '%s'\n", 1, {TYPE_VERBATIM}, 0, "UUID",
     "Find root by UUID"},
    /* FIXME: support MODE.  */
    {"vbeprobe", "vbeinfo\n", 0, {}, 0, "[MODE]",
     "Probe VBE information. If the mode number MODE is specified, show only"
     " the information about only the mode."}
  };

char *
grub_legacy_escape (const char *in, grub_size_t len)
{
  const char *ptr;
  char *ret, *outptr;
  int overhead = 0;
  for (ptr = in; ptr < in + len && *ptr; ptr++)
    if (*ptr == '\'' || *ptr == '\\')
      overhead++;
  ret = grub_malloc (ptr - in + overhead + 1);
  if (!ret)
    return NULL;
  outptr = ret;
  for (ptr = in; ptr < in + len && *ptr; ptr++)
    {
      if (*ptr == '\'' || *ptr == '\\')
	*outptr++ = '\\';
      
      *outptr++ = *ptr;
    }
  *outptr++ = 0;
  return ret;
}

static char *
adjust_file (const char *in, grub_size_t len)
{
  const char *comma, *ptr, *rest;
  char *ret, *outptr;
  int overhead = 0;
  int part;
  if (in[0] != '(')
    return grub_legacy_escape (in, len);
  for (ptr = in + 1; ptr < in + len && *ptr && *ptr != ')'
	 && *ptr != ','; ptr++)
    if (*ptr == '\'' || *ptr == '\\')
      overhead++;
  comma = ptr;
  if (*comma != ',')
    return grub_legacy_escape (in, len);
  part = grub_strtoull (comma + 1, (char **) &rest, 0);
  for (ptr = rest; ptr < in + len && *ptr; ptr++)
    if (*ptr == '\'' || *ptr == '\\')
      overhead++;

  /* 30 is enough for any number.  */
  ret = grub_malloc (ptr - in + overhead + 30);
  if (!ret)
    return NULL;

  outptr = ret;
  for (ptr = in; ptr < in + len && ptr <= comma; ptr++)
    {
      if (*ptr == '\'' || *ptr == '\\')
	*outptr++ = '\\';
      
      *outptr++ = *ptr;
    }
  grub_snprintf (outptr, 30, "%d", part + 1);
  while (*outptr)
    outptr++;
  for (ptr = rest; ptr < in + len; ptr++)
    {
      if (*ptr == '\'' || *ptr == '\\')
	*outptr++ = '\\';
      
      *outptr++ = *ptr;
    }
  *outptr = 0;
  return ret;
}

static int
check_option (const char *a, char *b, grub_size_t len)
{
  if (grub_strlen (b) != len)
    return 0;
  return grub_strncmp (a, b, len) == 0;
}

static int
is_option (enum arg_type opt, const char *curarg, grub_size_t len)
{
  switch (opt)
    {
    case TYPE_NOAPM_OPTION:
      return check_option (curarg, "--no-apm", len);
    case TYPE_FORCE_OPTION:
      return check_option (curarg, "--force", len);
    case TYPE_TYPE_OR_NOMEM_OPTION:
      return check_option (curarg, "--type=netbsd", len)
	|| check_option (curarg, "--type=freebsd", len)
	|| check_option (curarg, "--type=openbsd", len)
	|| check_option (curarg, "--type=linux", len)
	|| check_option (curarg, "--type=biglinux", len)
	|| check_option (curarg, "--type=multiboot", len)
	|| check_option (curarg, "--no-mem-option", len);
    case TYPE_OPTION:
      return (len >= 2 && curarg[0] == '-' && curarg[1] == '-');
    default:
      return 0;
    } 
}

char *
grub_legacy_parse (const char *buf, char **entryname, int *suffix)
{
  const char *ptr;
  const char *cmdname;
  unsigned i, cmdnum;

  *suffix = 0;

  for (ptr = buf; *ptr && grub_isspace (*ptr); ptr++);
  if (!*ptr || *ptr == '#')
    {
      char *ret;
      int len = grub_strlen (buf);
      ret = grub_malloc (len + 2);
      grub_memcpy (ret, buf, len);
      ret[len] = '\n';
      ret[len + 1] = 0;
      return ret;
    }

  cmdname = ptr;
  for (ptr = buf; *ptr && !grub_isspace (*ptr) && *ptr != '='; ptr++);

  if (entryname && grub_strncmp ("title", cmdname, ptr - cmdname) == 0
      && ptr - cmdname == sizeof ("title") - 1)
    {
      const char *ptr2;
      for (; grub_isspace (*ptr) || *ptr == '='; ptr++);
      ptr2 = ptr + grub_strlen (ptr);
      while (ptr2 > ptr && grub_isspace (*(ptr2 - 1)))
	ptr2--;
      *entryname = grub_strndup (ptr, ptr2 - ptr);
      return NULL;
    }

  if (grub_strncmp ("lock", cmdname, ptr - cmdname) == 0
      && ptr - cmdname == sizeof ("lock") - 1)
    {
      /* FIXME */
    }

  for (cmdnum = 0; cmdnum < ARRAY_SIZE (legacy_commands); cmdnum++)
    if (grub_strncmp (legacy_commands[cmdnum].name, cmdname, ptr - cmdname) == 0
	&& legacy_commands[cmdnum].name[ptr - cmdname] == 0)
      break;
  if (cmdnum == ARRAY_SIZE (legacy_commands))
    return grub_xasprintf ("# Unsupported legacy command: %s\n", buf);

  *suffix = !!(legacy_commands[cmdnum].flags & FLAG_SUFFIX);

  for (; grub_isspace (*ptr) || *ptr == '='; ptr++);

  char *args[ARRAY_SIZE (legacy_commands[0].argt)];
  grub_memset (args, 0, sizeof (args));

  {
    unsigned j = 0;
    int hold_arg = 0;
    const char *curarg = NULL; 
    for (i = 0; i < legacy_commands[cmdnum].argc + hold_arg; i++)
      {
 	grub_size_t curarglen;
	if (hold_arg)
	  {
	    ptr = curarg;
	    hold_arg = 0;
	  }
	for (; grub_isspace (*ptr); ptr++);
	curarg = ptr;
	for (; *ptr && !grub_isspace (*ptr); ptr++);
	if (i != legacy_commands[cmdnum].argc - 1
	    || (legacy_commands[cmdnum].flags & FLAG_IGNORE_REST))
	  curarglen = ptr - curarg;
	else
	  {
	    curarglen = grub_strlen (curarg);
	    while (curarglen > 0 && grub_isspace (curarg[curarglen - 1]))
	      curarglen--;
	  }
	if (*ptr)
	  ptr++;
	switch (legacy_commands[cmdnum].argt[i])
	  {
	  case TYPE_FILE_NO_CONSUME:
	    hold_arg = 1;
	  case TYPE_PARTITION:
	  case TYPE_FILE:
	    args[j++] = adjust_file (curarg, curarglen);
	    break;

	  case TYPE_REST_VERBATIM:
	    {
	      char *outptr, *outptr0;
	      int overhead = 3;
	      ptr = curarg;
	      while (*ptr)
		{
		  for (; *ptr && grub_isspace (*ptr); ptr++);
		  for (; *ptr && !grub_isspace (*ptr); ptr++)
		    if (*ptr == '\\' || *ptr == '\'')
		      overhead++;
		  if (*ptr)
		    ptr++;
		  overhead += 3;
		}
	      outptr0 = args[j++] = grub_malloc (overhead + (ptr - curarg));
	      if (!outptr0)
		return NULL;
	      ptr = curarg;
	      outptr = outptr0;
	      while (*ptr)
		{
		  for (; *ptr && grub_isspace (*ptr); ptr++);
		  if (outptr != outptr0)
		    *outptr++ = ' ';
		  *outptr++ = '\'';
		  for (; *ptr && !grub_isspace (*ptr); ptr++)
		    {
		      if (*ptr == '\\' || *ptr == '\'')
			*outptr++ = '\\';
		      *outptr++ = *ptr;
		    }
		  *outptr++ = '\'';
		  if (*ptr)
		    ptr++;
		}
	      *outptr++ = 0;
	    }
	    break;

	  case TYPE_VERBATIM:
	    args[j++] = grub_legacy_escape (curarg, curarglen);
	    break;
	  case TYPE_FORCE_OPTION:
	  case TYPE_NOAPM_OPTION:
	  case TYPE_TYPE_OR_NOMEM_OPTION:
	  case TYPE_OPTION:
	    if (is_option (legacy_commands[cmdnum].argt[i], curarg, curarglen))
	      {
		args[j++] = grub_strndup (curarg, curarglen);
		break;
	      }
	    args[j++] = grub_strdup ("");
	    hold_arg = 1;
	    break;
	  case TYPE_INT:
	    {
	      const char *brk;
	      int base = 10;
	      brk = curarg;
	      if (curarglen < 1)
		args[j++] = grub_strdup ("0");
	      if (brk[0] == '0' && brk[1] == 'x')
		base = 16;
	      else if (brk[0] == '0')
		base = 8;
	      for (; *brk && brk < curarg + curarglen; brk++)
		{
		  if (base == 8 &&  (*brk == '8' || *brk == '9'))
		    break;
		  if (grub_isdigit (*brk))
		    continue;
		  if (base != 16)
		    break;
		  if (!(*brk >= 'a' && *brk <= 'f')
		      && !(*brk >= 'A' && *brk <= 'F'))
		    break;
		}
	      if (brk == curarg)
		args[j++] = grub_strdup ("0");
	      else
		args[j++] = grub_strndup (curarg, brk - curarg);
	    }
	    break;
	  case TYPE_BOOL:
	    if (curarglen == 2 && curarg[0] == 'o' && curarg[1] == 'n')
	      args[j++] = grub_strdup ("1");
	    else
	      args[j++] = grub_strdup ("0");
	    break;
	  }
      }
  }

  {
    char *ret = grub_xasprintf (legacy_commands[cmdnum].map, args[0], args[1],
				args[2], args[3]);
    grub_free (args[0]);
    grub_free (args[1]);
    grub_free (args[2]);
    grub_free (args[3]);
    return ret;
  }
}
