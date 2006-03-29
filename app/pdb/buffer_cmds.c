/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"

#include <string.h>

#include <glib-object.h>

#include "pdb-types.h"
#include "procedural_db.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"
#include "core/gimpbuffer.h"
#include "core/gimpcontainer-filter.h"
#include "core/gimpcontainer.h"
#include "gimp-intl.h"

static ProcRecord buffers_get_list_proc;
static ProcRecord buffer_rename_proc;
static ProcRecord buffer_delete_proc;
static ProcRecord buffer_get_width_proc;
static ProcRecord buffer_get_height_proc;
static ProcRecord buffer_get_bytes_proc;
static ProcRecord buffer_get_image_type_proc;

void
register_buffer_procs (Gimp *gimp)
{
  ProcRecord *procedure;

  /*
   * buffers_get_list
   */
  procedure = procedural_db_init_proc (&buffers_get_list_proc, 1, 2);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("filter",
                                                      "filter",
                                                      "An optional regular expression used to filter the list",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("num-buffers",
                                                    "num buffers",
                                                    "The number of buffers",
                                                    0, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_STRINGARRAY,
                                  g_param_spec_pointer ("buffer-list",
                                                        "buffer list",
                                                        "The list of buffer names",
                                                        GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * buffer_rename
   */
  procedure = procedural_db_init_proc (&buffer_rename_proc, 2, 1);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("buffer-name",
                                                      "buffer name",
                                                      "The buffer name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("new-name",
                                                      "new name",
                                                      "The buffer's new name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_STRING,
                                  gimp_param_spec_string ("real-name",
                                                          "real name",
                                                          "The real name given to the buffer",
                                                          FALSE, FALSE,
                                                          NULL,
                                                          GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * buffer_delete
   */
  procedure = procedural_db_init_proc (&buffer_delete_proc, 1, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("buffer-name",
                                                      "buffer name",
                                                      "The buffer name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * buffer_get_width
   */
  procedure = procedural_db_init_proc (&buffer_get_width_proc, 1, 1);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("buffer-name",
                                                      "buffer name",
                                                      "The buffer name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("width",
                                                    "width",
                                                    "The buffer width",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * buffer_get_height
   */
  procedure = procedural_db_init_proc (&buffer_get_height_proc, 1, 1);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("buffer-name",
                                                      "buffer name",
                                                      "The buffer name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("height",
                                                    "height",
                                                    "The buffer height",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * buffer_get_bytes
   */
  procedure = procedural_db_init_proc (&buffer_get_bytes_proc, 1, 1);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("buffer-name",
                                                      "buffer name",
                                                      "The buffer name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("bytes",
                                                    "bytes",
                                                    "The buffer bpp",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * buffer_get_image_type
   */
  procedure = procedural_db_init_proc (&buffer_get_image_type_proc, 1, 1);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("buffer-name",
                                                      "buffer name",
                                                      "The buffer name",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (procedure,
                                  GIMP_PDB_INT32,
                                  g_param_spec_enum ("image-type",
                                                     "image type",
                                                     "The buffer image type: { GIMP_RGB (0), GIMP_GRAY (1), GIMP_INDEXED (2) }",
                                                     GIMP_TYPE_IMAGE_BASE_TYPE,
                                                     GIMP_RGB,
                                                     GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

}

static Argument *
buffers_get_list_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *filter;
  gint32 num_buffers = 0;
  gchar **buffer_list = NULL;

  filter = (gchar *) args[0].value.pdb_pointer;
  if (filter && !g_utf8_validate (filter, -1, NULL))
    success = FALSE;

  if (success)
    {
      buffer_list = gimp_container_get_filtered_name_array (gimp->named_buffers,
                                                            filter, &num_buffers);
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_int = num_buffers;
      return_vals[2].value.pdb_pointer = buffer_list;
    }

  return return_vals;
}

static ProcRecord buffers_get_list_proc =
{
  "gimp-buffers-get-list",
  "gimp-buffers-get-list",
  "Retrieve a complete listing of the available buffers.",
  "This procedure returns a complete listing of available named buffers.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffers_get_list_invoker } }
};

static Argument *
buffer_rename_invoker (ProcRecord   *proc_record,
                       Gimp         *gimp,
                       GimpContext  *context,
                       GimpProgress *progress,
                       Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *buffer_name;
  gchar *new_name;
  gchar *real_name = NULL;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  new_name = (gchar *) args[1].value.pdb_pointer;
  if (new_name == NULL || !g_utf8_validate (new_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      if (buffer && strlen (new_name))
        {
          gimp_object_set_name (GIMP_OBJECT (buffer), new_name);
          real_name = g_strdup (gimp_object_get_name (GIMP_OBJECT (buffer)));
        }
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_pointer = real_name;

  return return_vals;
}

static ProcRecord buffer_rename_proc =
{
  "gimp-buffer-rename",
  "gimp-buffer-rename",
  "Renames a named buffer.",
  "This procedure renames a named buffer.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffer_rename_invoker } }
};

static Argument *
buffer_delete_invoker (ProcRecord   *proc_record,
                       Gimp         *gimp,
                       GimpContext  *context,
                       GimpProgress *progress,
                       Argument     *args)
{
  gboolean success = TRUE;
  gchar *buffer_name;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      if (buffer)
        success = gimp_container_remove (gimp->named_buffers, GIMP_OBJECT (buffer));
      else
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord buffer_delete_proc =
{
  "gimp-buffer-delete",
  "gimp-buffer-delete",
  "Deletes a named buffer.",
  "This procedure deletes a named buffer.",
  "David Gowers <neota@softhome.net>",
  "David Gowers <neota@softhome.net>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffer_delete_invoker } }
};

static Argument *
buffer_get_width_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *buffer_name;
  gint32 width = 0;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      if (buffer)
        width = gimp_buffer_get_width (buffer);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_int = width;

  return return_vals;
}

static ProcRecord buffer_get_width_proc =
{
  "gimp-buffer-get-width",
  "gimp-buffer-get-width",
  "Retrieves the specified buffer's width.",
  "This procedure retrieves the specified named buffer's width.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffer_get_width_invoker } }
};

static Argument *
buffer_get_height_invoker (ProcRecord   *proc_record,
                           Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *buffer_name;
  gint32 height = 0;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      if (buffer)
        height = gimp_buffer_get_height (buffer);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_int = height;

  return return_vals;
}

static ProcRecord buffer_get_height_proc =
{
  "gimp-buffer-get-height",
  "gimp-buffer-get-height",
  "Retrieves the specified buffer's height.",
  "This procedure retrieves the specified named buffer's height.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffer_get_height_invoker } }
};

static Argument *
buffer_get_bytes_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *buffer_name;
  gint32 bytes = 0;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      if (buffer)
        bytes = gimp_buffer_get_bytes (buffer);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_int = bytes;

  return return_vals;
}

static ProcRecord buffer_get_bytes_proc =
{
  "gimp-buffer-get-bytes",
  "gimp-buffer-get-bytes",
  "Retrieves the specified buffer's bytes.",
  "This procedure retrieves the specified named buffer's bytes.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffer_get_bytes_invoker } }
};

static Argument *
buffer_get_image_type_invoker (ProcRecord   *proc_record,
                               Gimp         *gimp,
                               GimpContext  *context,
                               GimpProgress *progress,
                               Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *buffer_name;
  gint32 image_type = 0;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      if (buffer)
        image_type = gimp_buffer_get_image_type (buffer);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_int = image_type;

  return return_vals;
}

static ProcRecord buffer_get_image_type_proc =
{
  "gimp-buffer-get-image-type",
  "gimp-buffer-get-image-type",
  "Retrieves the specified buffer's image type.",
  "This procedure retrieves the specified named buffer's image type.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { buffer_get_image_type_invoker } }
};
