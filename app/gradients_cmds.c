/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2000 Spencer Kimball and Peter Mattis
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

#include "procedural_db.h"

#include "gimpcontext.h"
#include "gradient_header.h"

static ProcRecord gradients_get_list_proc;
static ProcRecord gradients_get_active_proc;
static ProcRecord gradients_set_active_proc;
static ProcRecord gradients_sample_uniform_proc;
static ProcRecord gradients_sample_custom_proc;

void
register_gradients_procs (void)
{
  procedural_db_register (&gradients_get_list_proc);
  procedural_db_register (&gradients_get_active_proc);
  procedural_db_register (&gradients_set_active_proc);
  procedural_db_register (&gradients_sample_uniform_proc);
  procedural_db_register (&gradients_sample_custom_proc);
}

static Argument *
gradients_get_list_invoker (Argument *args)
{
  gboolean success;
  Argument *return_args;
  gchar **gradients;
  gradient_t *grad;
  GSList *list;
  int i = 0;

  gradients = g_new (gchar *, num_gradients);

  success = (list = gradients_list) != NULL;

  while (list)
    {
      grad           = list->data;
      gradients[i++] = g_strdup (grad->name);
      list           = list->next;
    }

  return_args = procedural_db_return_args (&gradients_get_list_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = num_gradients;
      return_args[2].value.pdb_pointer = gradients;
    }

  return return_args;
}

static ProcArg gradients_get_list_outargs[] =
{
  {
    PDB_INT32,
    "num_gradients",
    "The number of loaded gradients"
  },
  {
    PDB_STRINGARRAY,
    "gradient_names",
    "The list of gradient names"
  }
};

static ProcRecord gradients_get_list_proc =
{
  "gimp_gradients_get_list",
  "Retrieve the list of loaded gradients.",
  "This procedure returns a list of the gradients that are currently loaded in the gradient editor. You can later use the gimp_gradients_set_active function to set the active gradient.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  PDB_INTERNAL,
  0,
  NULL,
  2,
  gradients_get_list_outargs,
  { { gradients_get_list_invoker } }
};

static Argument *
gradients_get_active_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;

  success = gimp_context_get_gradient (NULL) != NULL;

  return_args = procedural_db_return_args (&gradients_get_active_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (gimp_context_get_gradient (NULL)->name);

  return return_args;
}

static ProcArg gradients_get_active_outargs[] =
{
  {
    PDB_STRING,
    "name",
    "The name of the active gradient"
  }
};

static ProcRecord gradients_get_active_proc =
{
  "gimp_gradients_get_active",
  "Retrieve the name of the active gradient.",
  "This procedure returns the name of the active gradient in the gradient editor.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  PDB_INTERNAL,
  0,
  NULL,
  1,
  gradients_get_active_outargs,
  { { gradients_get_active_invoker } }
};

static Argument *
gradients_set_active_invoker (Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  gradient_t *gradient;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  if (success)
    {
      gradient = gradient_list_get_gradient (gradients_list, name);
    
      success = FALSE;
    
      if (gradient)
	{
	  gimp_context_set_gradient (NULL, gradient);
	  success = TRUE;
	}
    }

  return procedural_db_return_args (&gradients_set_active_proc, success);
}

static ProcArg gradients_set_active_inargs[] =
{
  {
    PDB_STRING,
    "name",
    "The name of the gradient to set"
  }
};

static ProcRecord gradients_set_active_proc =
{
  "gimp_gradients_set_active",
  "Sets the specified gradient as the active gradient.",
  "This procedure lets you set the specified gradient as the active or \"current\" one. The name is simply a string which corresponds to one of the loaded gradients in the gradient editor. If no matching gradient is found, this procedure will return an error. Otherwise, the specified gradient will become active and will be used for subsequent custom gradient operations.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  PDB_INTERNAL,
  1,
  gradients_set_active_inargs,
  0,
  NULL,
  { { gradients_set_active_invoker } }
};

static Argument *
gradients_sample_uniform_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gint32 i;
  gint32 array_length = 0;
  gdouble *color_samples = NULL;
  gradient_t *gradient;
  gdouble pos, delta;
  gdouble r, g, b, a;
  gdouble *pv;

  i = args[0].value.pdb_int;
  if (i < 2)
    success = FALSE;

  if (success)
    {
      pos = 0.0;
      delta = 1.0 / (i - 1);
    
      array_length = i * 4;
    
      pv = color_samples = g_new (gdouble, array_length);
    
      gradient = gimp_context_get_gradient (NULL);
    
      while (i--)
	{
	  gradient_get_color_at (gradient, pos, &r, &g, &b, &a);
    
	  *pv++ = r;
	  *pv++ = g;
	  *pv++ = b;
	  *pv++ = a;
    
	  pos += delta;
	}
    }

  return_args = procedural_db_return_args (&gradients_sample_uniform_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = array_length;
      return_args[2].value.pdb_pointer = color_samples;
    }

  return return_args;
}

static ProcArg gradients_sample_uniform_inargs[] =
{
  {
    PDB_INT32,
    "num_samples",
    "The number of samples to take"
  }
};

static ProcArg gradients_sample_uniform_outargs[] =
{
  {
    PDB_INT32,
    "array_length",
    "Length of the color_samples array (4 * num_samples)"
  },
  {
    PDB_FLOATARRAY,
    "color_samples",
    "Color samples: { R1, G1, B1, A1, ..., Rn, Gn, Bn, An }"
  }
};

static ProcRecord gradients_sample_uniform_proc =
{
  "gimp_gradients_sample_uniform",
  "Sample the active gradient in uniform parts.",
  "This procedure samples the active gradient from the gradient editor in the specified number of uniform parts. It returns a list of floating-point values which correspond to the RGBA values for each sample. The minimum number of samples to take is 2, in which case the returned colors will correspond to the { 0.0, 1.0 } positions in the gradient. For example, if the number of samples is 3, the procedure will return the colors at positions { 0.0, 0.5, 1.0 }.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  PDB_INTERNAL,
  1,
  gradients_sample_uniform_inargs,
  2,
  gradients_sample_uniform_outargs,
  { { gradients_sample_uniform_invoker } }
};

static Argument *
gradients_sample_custom_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gint32 i;
  gdouble *pos;
  gint32 array_length = 0;
  gdouble *color_samples = NULL;
  gradient_t *gradient;
  gdouble r, g, b, a;
  gdouble *pv;

  i = args[0].value.pdb_int;
  if (i <= 0)
    success = FALSE;

  pos = (gdouble *) args[1].value.pdb_pointer;

  if (success)
    {
      array_length = i * 4;
    
      pv = color_samples = g_new (gdouble, array_length);
    
      gradient = gimp_context_get_gradient (NULL);
    
      while (i--)
	{
	  gradient_get_color_at (gradient, *pos, &r, &g, &b, &a);
    
	  *pv++ = r;
	  *pv++ = g;
	  *pv++ = b;
	  *pv++ = a;
    
	  pos++;
	}
    }

  return_args = procedural_db_return_args (&gradients_sample_custom_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = array_length;
      return_args[2].value.pdb_pointer = color_samples;
    }

  return return_args;
}

static ProcArg gradients_sample_custom_inargs[] =
{
  {
    PDB_INT32,
    "num_samples",
    "The number of samples to take"
  },
  {
    PDB_FLOATARRAY,
    "positions",
    "The list of positions to sample along the gradient"
  }
};

static ProcArg gradients_sample_custom_outargs[] =
{
  {
    PDB_INT32,
    "array_length",
    "Length of the color_samples array (4 * num_samples)"
  },
  {
    PDB_FLOATARRAY,
    "color_samples",
    "Color samples: { R1, G1, B1, A1, ..., Rn, Gn, Bn, An }"
  }
};

static ProcRecord gradients_sample_custom_proc =
{
  "gimp_gradients_sample_custom",
  "Sample the active gradient in custom positions.",
  "This procedure samples the active gradient from the gradient editor in the specified number of points. The procedure will sample the gradient in the specified positions from the list. The left endpoint of the gradient corresponds to position 0.0, and the right endpoint corresponds to 1.0. The procedure returns a list of floating-point values which correspond to the RGBA values for each sample.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  PDB_INTERNAL,
  2,
  gradients_sample_custom_inargs,
  2,
  gradients_sample_custom_outargs,
  { { gradients_sample_custom_invoker } }
};
