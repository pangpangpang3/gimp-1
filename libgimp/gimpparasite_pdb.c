/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimpparasite_pdb.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is autogenerated by pdbgen.pl */

#include "gimp.h"

GimpParasite *
gimp_parasite_find (gchar *name)
{
  GParam *return_vals;
  gint nreturn_vals;
  GimpParasite *parasite = NULL;

  return_vals = gimp_run_procedure ("gimp_parasite_find",
				    &nreturn_vals,
				    PARAM_STRING, name,
				    PARAM_END);

  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    parasite = parasite_copy (&return_vals[1].data.d_parasite);

  gimp_destroy_params (return_vals, nreturn_vals);

  return parasite;
}

void
gimp_parasite_attach (GimpParasite *parasite)
{
  GParam *return_vals;
  gint nreturn_vals;

  return_vals = gimp_run_procedure ("gimp_parasite_attach",
				    &nreturn_vals,
				    PARAM_PARASITE, parasite,
				    PARAM_END);

  gimp_destroy_params (return_vals, nreturn_vals);
}

void
gimp_parasite_detach (gchar *name)
{
  GParam *return_vals;
  gint nreturn_vals;

  return_vals = gimp_run_procedure ("gimp_parasite_detach",
				    &nreturn_vals,
				    PARAM_STRING, name,
				    PARAM_END);

  gimp_destroy_params (return_vals, nreturn_vals);
}

void
gimp_parasite_list (gint   *num_parasites,
		    gchar **parasites)
{
  GParam *return_vals;
  gint nreturn_vals;
  gint i;

  return_vals = gimp_run_procedure ("gimp_parasite_list",
				    &nreturn_vals,
				    PARAM_END);

  *num_parasites = 0;
  *parasites = NULL;
  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    {
      *num_parasites = return_vals[1].data.d_int32;
      parasites = g_new (gchar *, *num_parasites);
      for (i = 0; i < *num_parasites; i++)
	parasites[i] = g_strdup (return_vals[2].data.d_stringarray[i]);
    }

  gimp_destroy_params (return_vals, nreturn_vals);
}

GimpParasite *
gimp_drawable_parasite_find (gint32  drawable_ID,
			     gchar  *name)
{
  GParam *return_vals;
  gint nreturn_vals;
  GimpParasite *parasite = NULL;

  return_vals = gimp_run_procedure ("gimp_drawable_parasite_find",
				    &nreturn_vals,
				    PARAM_DRAWABLE, drawable_ID,
				    PARAM_STRING, name,
				    PARAM_END);

  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    parasite = parasite_copy (&return_vals[1].data.d_parasite);

  gimp_destroy_params (return_vals, nreturn_vals);

  return parasite;
}

void
gimp_drawable_parasite_attach (gint32        drawable_ID,
			       GimpParasite *parasite)
{
  GParam *return_vals;
  gint nreturn_vals;

  return_vals = gimp_run_procedure ("gimp_drawable_parasite_attach",
				    &nreturn_vals,
				    PARAM_DRAWABLE, drawable_ID,
				    PARAM_PARASITE, parasite,
				    PARAM_END);

  gimp_destroy_params (return_vals, nreturn_vals);
}

void
gimp_drawable_parasite_detach (gint32  drawable_ID,
			       gchar  *name)
{
  GParam *return_vals;
  gint nreturn_vals;

  return_vals = gimp_run_procedure ("gimp_drawable_parasite_detach",
				    &nreturn_vals,
				    PARAM_DRAWABLE, drawable_ID,
				    PARAM_STRING, name,
				    PARAM_END);

  gimp_destroy_params (return_vals, nreturn_vals);
}

void
gimp_drawable_parasite_list (gint32  drawable_ID,
			     gint   *num_parasites,
			     gchar **parasites)

{
  GParam *return_vals;
  gint nreturn_vals;
  gint i;

  return_vals = gimp_run_procedure ("gimp_drawable_parasite_list",
				    &nreturn_vals,
				    PARAM_DRAWABLE, drawable_ID,
				    PARAM_END);

  *num_parasites = 0;
  *parasites = NULL;
  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    {
      *num_parasites = return_vals[1].data.d_int32;
      parasites = g_new (gchar *, *num_parasites);
      for (i = 0; i < *num_parasites; i++)
	parasites[i] = g_strdup (return_vals[2].data.d_stringarray[i]);
    }

  gimp_destroy_params (return_vals, nreturn_vals);
}

GimpParasite *
gimp_image_parasite_find (gint32  image_ID,
			  gchar  *name)
{
  GParam *return_vals;
  gint nreturn_vals;
  GimpParasite *parasite = NULL;

  return_vals = gimp_run_procedure ("gimp_image_parasite_find",
				    &nreturn_vals,
				    PARAM_IMAGE, image_ID,
				    PARAM_STRING, name,
				    PARAM_END);

  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    parasite = parasite_copy (&return_vals[1].data.d_parasite);

  gimp_destroy_params (return_vals, nreturn_vals);

  return parasite;
}

void
gimp_image_parasite_attach (gint32        image_ID,
			    GimpParasite *parasite)
{
  GParam *return_vals;
  gint nreturn_vals;

  return_vals = gimp_run_procedure ("gimp_image_parasite_attach",
				    &nreturn_vals,
				    PARAM_IMAGE, image_ID,
				    PARAM_PARASITE, parasite,
				    PARAM_END);

  gimp_destroy_params (return_vals, nreturn_vals);
}

void
gimp_image_parasite_detach (gint32  image_ID,
			    gchar  *name)
{
  GParam *return_vals;
  gint nreturn_vals;

  return_vals = gimp_run_procedure ("gimp_image_parasite_detach",
				    &nreturn_vals,
				    PARAM_IMAGE, image_ID,
				    PARAM_STRING, name,
				    PARAM_END);

  gimp_destroy_params (return_vals, nreturn_vals);
}

void
gimp_image_parasite_list (gint32  image_ID,
			  gint   *num_parasites,
			  gchar **parasites)
{
  GParam *return_vals;
  gint nreturn_vals;
  gint i;

  return_vals = gimp_run_procedure ("gimp_image_parasite_list",
				    &nreturn_vals,
				    PARAM_IMAGE, image_ID,
				    PARAM_END);

  *num_parasites = 0;
  *parasites = NULL;
  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    {
      *num_parasites = return_vals[1].data.d_int32;
      parasites = g_new (gchar *, *num_parasites);
      for (i = 0; i < *num_parasites; i++)
	parasites[i] = g_strdup (return_vals[2].data.d_stringarray[i]);
    }

  gimp_destroy_params (return_vals, nreturn_vals);
}
