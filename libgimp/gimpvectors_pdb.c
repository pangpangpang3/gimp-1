/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpvectors_pdb.c
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

#include "config.h"

#include <string.h>

#include "gimp.h"

/**
 * gimp_vectors_new:
 * @image_ID: The image.
 * @name: the name of the new vector object.
 *
 * Creates a new empty vectors object.
 *
 * Creates a new empty vectors object. Needs to be added to an image
 * using gimp_image_add_vectors().
 *
 * Returns: the current vector object, 0 if no vector exists in the image.
 *
 * Since: GIMP 2.4
 */
gint32
gimp_vectors_new (gint32       image_ID,
                  const gchar *name)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint32 vectors_ID = -1;

  return_vals = gimp_run_procedure ("gimp-vectors-new",
                                    &nreturn_vals,
                                    GIMP_PDB_IMAGE, image_ID,
                                    GIMP_PDB_STRING, name,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    vectors_ID = return_vals[1].data.d_vectors;

  gimp_destroy_params (return_vals, nreturn_vals);

  return vectors_ID;
}

/**
 * gimp_vectors_get_image:
 * @vectors_ID: The vectors object.
 *
 * Returns the vectors objects image.
 *
 * Returns the vectors objects image.
 *
 * Returns: The vectors image.
 *
 * Since: GIMP 2.4
 */
gint32
gimp_vectors_get_image (gint32 vectors_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint32 image_ID = -1;

  return_vals = gimp_run_procedure ("gimp-vectors-get-image",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    image_ID = return_vals[1].data.d_image;

  gimp_destroy_params (return_vals, nreturn_vals);

  return image_ID;
}

/**
 * gimp_vectors_get_name:
 * @vectors_ID: The vectors object.
 *
 * Gets the name of the vectors object.
 *
 * Gets the name of the vectors object.
 *
 * Returns: The name of the vectors object.
 *
 * Since: GIMP 2.4
 */
gchar *
gimp_vectors_get_name (gint32 vectors_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gchar *name = NULL;

  return_vals = gimp_run_procedure ("gimp-vectors-get-name",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    name = g_strdup (return_vals[1].data.d_string);

  gimp_destroy_params (return_vals, nreturn_vals);

  return name;
}

/**
 * gimp_vectors_set_name:
 * @vectors_ID: The vectors object.
 * @name: the new name of the path.
 *
 * Sets the name of the vectors object.
 *
 * Sets the name of the vectors object.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_set_name (gint32       vectors_ID,
                       const gchar *name)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-set-name",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_STRING, name,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_get_visible:
 * @vectors_ID: The vectors object.
 *
 * Gets the visibility of the vectors object.
 *
 * Gets the visibility of the vectors object.
 *
 * Returns: TRUE if the path is visible, FALSE otherwise.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_get_visible (gint32 vectors_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean visible = FALSE;

  return_vals = gimp_run_procedure ("gimp-vectors-get-visible",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    visible = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return visible;
}

/**
 * gimp_vectors_set_visible:
 * @vectors_ID: The vectors object.
 * @visible: Whether the path is visible.
 *
 * Sets the visibility of the vectors object.
 *
 * Sets the visibility of the vectors object.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_set_visible (gint32   vectors_ID,
                          gboolean visible)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-set-visible",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, visible,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_get_linked:
 * @vectors_ID: The vectors object.
 *
 * Gets the linked state of the vectors object.
 *
 * Gets the linked state of the vectors object.
 *
 * Returns: TRUE if the path is linked, FALSE otherwise.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_get_linked (gint32 vectors_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean linked = FALSE;

  return_vals = gimp_run_procedure ("gimp-vectors-get-linked",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    linked = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return linked;
}

/**
 * gimp_vectors_set_linked:
 * @vectors_ID: The vectors object.
 * @linked: Whether the path is linked.
 *
 * Sets the linked state of the vectors object.
 *
 * Sets the linked state of the vectors object.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_set_linked (gint32   vectors_ID,
                         gboolean linked)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-set-linked",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, linked,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_get_tattoo:
 * @vectors_ID: The vectors object.
 *
 * Get the tattoo of the vectors object.
 *
 * Get the tattoo state of the vectors object.
 *
 * Returns: The vectors tattoo.
 *
 * Since: GIMP 2.4
 */
gint
gimp_vectors_get_tattoo (gint32 vectors_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint tattoo = 0;

  return_vals = gimp_run_procedure ("gimp-vectors-get-tattoo",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    tattoo = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return tattoo;
}

/**
 * gimp_vectors_set_tattoo:
 * @vectors_ID: The vectors object.
 * @tattoo: the new tattoo.
 *
 * Set the tattoo of the vectors object.
 *
 * Set the tattoo of the vectors object.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_set_tattoo (gint32 vectors_ID,
                         gint   tattoo)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-set-tattoo",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, tattoo,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_get_strokes:
 * @vectors_ID: The vectors object.
 * @num_strokes: The number of strokes returned.
 *
 * List the strokes associated with the passed path.
 *
 * Returns an Array with the stroke-IDs associated with the passed
 * path.
 *
 * Returns: List of the strokes belonging to the path.
 *
 * Since: GIMP 2.4
 */
gint *
gimp_vectors_get_strokes (gint32  vectors_ID,
                          gint   *num_strokes)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint *stroke_ids = NULL;

  return_vals = gimp_run_procedure ("gimp-vectors-get-strokes",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_END);

  *num_strokes = 0;

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      *num_strokes = return_vals[1].data.d_int32;
      stroke_ids = g_new (gint32, *num_strokes);
      memcpy (stroke_ids,
              return_vals[2].data.d_int32array,
              *num_strokes * sizeof (gint32));
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return stroke_ids;
}

/**
 * gimp_vectors_stroke_get_length:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @precision: The precision used for the approximation.
 *
 * Measure the length of the given stroke.
 *
 * Measure the length of the given stroke.
 *
 * Returns: The length (in pixels) of the given stroke.
 *
 * Since: GIMP 2.4
 */
gdouble
gimp_vectors_stroke_get_length (gint32  vectors_ID,
                                gint    stroke_id,
                                gdouble precision)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gdouble length = 0.0;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-get-length",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, precision,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    length = return_vals[1].data.d_float;

  gimp_destroy_params (return_vals, nreturn_vals);

  return length;
}

/**
 * gimp_vectors_stroke_get_point_at_dist:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @dist: The given distance.
 * @precision: The precision used for the approximation.
 * @x_point: The x position of the point.
 * @y_point: The y position of the point.
 * @slope: The slope (dy / dx) at the specified point.
 * @valid: Indicator for the validity of the returned data.
 *
 * Get point at a specified distance along the stroke.
 *
 * This will return the x,y position of a point at a given distance
 * along the stroke. The distance will be obtained by first digitizing
 * the curve internally and then walking along the curve. For a closed
 * stroke the start of the path is the first point on the path that was
 * created. This might not be obvious. If the stroke is not long
 * enough, a \"valid\" flag will be FALSE.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_stroke_get_point_at_dist (gint32    vectors_ID,
                                       gint      stroke_id,
                                       gdouble   dist,
                                       gdouble   precision,
                                       gdouble  *x_point,
                                       gdouble  *y_point,
                                       gdouble  *slope,
                                       gboolean *valid)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-get-point-at-dist",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, dist,
                                    GIMP_PDB_FLOAT, precision,
                                    GIMP_PDB_END);

  *x_point = 0.0;
  *y_point = 0.0;
  *slope = 0.0;
  *valid = FALSE;

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  if (success)
    {
      *x_point = return_vals[1].data.d_float;
      *y_point = return_vals[2].data.d_float;
      *slope = return_vals[3].data.d_float;
      *valid = return_vals[4].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_remove_stroke:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 *
 * remove the stroke from a vectors object.
 *
 * Remove the stroke from a vectors object.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_remove_stroke (gint32 vectors_ID,
                            gint   stroke_id)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-remove-stroke",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_stroke_close:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 *
 * closes the specified stroke.
 *
 * Closes the specified stroke.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_stroke_close (gint32 vectors_ID,
                           gint   stroke_id)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-close",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_stroke_translate:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @off_x: Offset in x direction.
 * @off_y: Offset in y direction.
 *
 * translate the given stroke.
 *
 * Translate the given stroke.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_stroke_translate (gint32 vectors_ID,
                               gint   stroke_id,
                               gint   off_x,
                               gint   off_y)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-translate",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_INT32, off_x,
                                    GIMP_PDB_INT32, off_y,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_stroke_scale:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @scale_x: Scale factor in x direction.
 * @scale_y: Scale factor in y direction.
 *
 * scales the given stroke.
 *
 * Scale the given stroke.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_stroke_scale (gint32  vectors_ID,
                           gint    stroke_id,
                           gdouble scale_x,
                           gdouble scale_y)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-scale",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, scale_x,
                                    GIMP_PDB_FLOAT, scale_y,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_stroke_get_points:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @num_points: The number of floats returned.
 * @controlpoints: List of the control points for the stroke (x0, y0, x1, y1, ...).
 * @closed: Whether the stroke is closed or not.
 *
 * returns the control points of a stroke.
 *
 * returns the control points of a stroke. The interpretation of the
 * coordinates returned depends on the type of the stroke. For Gimp 2.4
 * this is always a bezier stroke, where the coordinates are the
 * control points.
 *
 * Returns: type of the stroke (always bezier for now).
 *
 * Since: GIMP 2.4
 */
gint
gimp_vectors_stroke_get_points (gint32     vectors_ID,
                                gint       stroke_id,
                                gint      *num_points,
                                gdouble  **controlpoints,
                                gboolean  *closed)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint type = 0;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-get-points",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_END);

  *num_points = 0;

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      type = return_vals[1].data.d_int32;
      *num_points = return_vals[2].data.d_int32;
      *controlpoints = g_new (gdouble, *num_points);
      memcpy (*controlpoints,
              return_vals[3].data.d_floatarray,
              *num_points * sizeof (gdouble));
      *closed = return_vals[4].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return type;
}

/**
 * gimp_vectors_stroke_interpolate:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @precision: The precision used for the approximation.
 * @num_coords: The number of floats returned.
 * @closed: Whether the stroke is closed or not.
 *
 * returns polygonal approximation of the stroke.
 *
 * returns polygonal approximation of the stroke.
 *
 * Returns: List of the coords along the path (x0, y0, x1, y1, ...).
 *
 * Since: GIMP 2.4
 */
gdouble *
gimp_vectors_stroke_interpolate (gint32    vectors_ID,
                                 gint      stroke_id,
                                 gdouble   precision,
                                 gint     *num_coords,
                                 gboolean *closed)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gdouble *coords = NULL;

  return_vals = gimp_run_procedure ("gimp-vectors-stroke-interpolate",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, precision,
                                    GIMP_PDB_END);

  *num_coords = 0;

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      *num_coords = return_vals[1].data.d_int32;
      coords = g_new (gdouble, *num_coords);
      memcpy (coords,
              return_vals[2].data.d_floatarray,
              *num_coords * sizeof (gdouble));
      *closed = return_vals[3].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return coords;
}

/**
 * gimp_vectors_bezier_stroke_new_moveto:
 * @vectors_ID: The vectors object.
 * @x0: The x-coordinate of the moveto.
 * @y0: The y-coordinate of the moveto.
 *
 * Adds a bezier stroke with a single moveto to the vectors object.
 *
 * Adds a bezier stroke with a single moveto to the vectors object.
 *
 * Returns: The resulting stroke.
 *
 * Since: GIMP 2.4
 */
gint
gimp_vectors_bezier_stroke_new_moveto (gint32  vectors_ID,
                                       gdouble x0,
                                       gdouble y0)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint stroke_id = 0;

  return_vals = gimp_run_procedure ("gimp-vectors-bezier-stroke-new-moveto",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_FLOAT, x0,
                                    GIMP_PDB_FLOAT, y0,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    stroke_id = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return stroke_id;
}

/**
 * gimp_vectors_bezier_stroke_lineto:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @x0: The x-coordinate of the lineto.
 * @y0: The y-coordinate of the lineto.
 *
 * Extends a bezier stroke with a lineto.
 *
 * Extends a bezier stroke with a lineto.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_bezier_stroke_lineto (gint32  vectors_ID,
                                   gint    stroke_id,
                                   gdouble x0,
                                   gdouble y0)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-bezier-stroke-lineto",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, x0,
                                    GIMP_PDB_FLOAT, y0,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_bezier_stroke_conicto:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @x0: The x-coordinate of the control point.
 * @y0: The y-coordinate of the control point.
 * @x1: The x-coordinate of the end point.
 * @y1: The y-coordinate of the end point.
 *
 * Extends a bezier stroke with a conic bezier spline.
 *
 * Extends a bezier stroke with a conic bezier spline. Actually a cubic
 * bezier spline gets added that realizes the shape of a conic bezier
 * spline.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_bezier_stroke_conicto (gint32  vectors_ID,
                                    gint    stroke_id,
                                    gdouble x0,
                                    gdouble y0,
                                    gdouble x1,
                                    gdouble y1)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-bezier-stroke-conicto",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, x0,
                                    GIMP_PDB_FLOAT, y0,
                                    GIMP_PDB_FLOAT, x1,
                                    GIMP_PDB_FLOAT, y1,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_bezier_stroke_cubicto:
 * @vectors_ID: The vectors object.
 * @stroke_id: The stroke ID.
 * @x0: The x-coordinate of the first control point.
 * @y0: The y-coordinate of the first control point.
 * @x1: The x-coordinate of the second control point.
 * @y1: The y-coordinate of the second control point.
 * @x2: The x-coordinate of the end point.
 * @y2: The y-coordinate of the end point.
 *
 * Extends a bezier stroke with a cubic bezier spline.
 *
 * Extends a bezier stroke with a cubic bezier spline.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_bezier_stroke_cubicto (gint32  vectors_ID,
                                    gint    stroke_id,
                                    gdouble x0,
                                    gdouble y0,
                                    gdouble x1,
                                    gdouble y1,
                                    gdouble x2,
                                    gdouble y2)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-bezier-stroke-cubicto",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, stroke_id,
                                    GIMP_PDB_FLOAT, x0,
                                    GIMP_PDB_FLOAT, y0,
                                    GIMP_PDB_FLOAT, x1,
                                    GIMP_PDB_FLOAT, y1,
                                    GIMP_PDB_FLOAT, x2,
                                    GIMP_PDB_FLOAT, y2,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_vectors_bezier_stroke_new_ellipse:
 * @vectors_ID: The vectors object.
 * @x0: The x-coordinate of the center.
 * @y0: The y-coordinate of the center.
 * @radius_x: The radius in x direction.
 * @radius_y: The radius in y direction.
 * @angle: The angle the x-axis of the ellipse (radians, counterclockwise).
 *
 * Adds a bezier stroke describing an ellipse the vectors object.
 *
 * Adds a bezier stroke describing an ellipse the vectors object.
 *
 * Returns: The resulting stroke.
 *
 * Since: GIMP 2.4
 */
gint
gimp_vectors_bezier_stroke_new_ellipse (gint32  vectors_ID,
                                        gdouble x0,
                                        gdouble y0,
                                        gdouble radius_x,
                                        gdouble radius_y,
                                        gdouble angle)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint stroke_id = 0;

  return_vals = gimp_run_procedure ("gimp-vectors-bezier-stroke-new-ellipse",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_FLOAT, x0,
                                    GIMP_PDB_FLOAT, y0,
                                    GIMP_PDB_FLOAT, radius_x,
                                    GIMP_PDB_FLOAT, radius_y,
                                    GIMP_PDB_FLOAT, angle,
                                    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    stroke_id = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return stroke_id;
}

/**
 * gimp_vectors_to_selection:
 * @vectors_ID: The vectors object to render to the selection.
 * @operation: The desired operation with current selection.
 * @antialias: Antialias selection.
 * @feather: Feather selection.
 * @feather_radius_x: Feather radius x.
 * @feather_radius_y: Feather radius y.
 *
 * Transforms the specified vectors object into a selection
 *
 * This procedure renders the desired vectors object into the current
 * selection of the image the vectors object belongs to.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_vectors_to_selection (gint32         vectors_ID,
                           GimpChannelOps operation,
                           gboolean       antialias,
                           gboolean       feather,
                           gdouble        feather_radius_x,
                           gdouble        feather_radius_y)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-vectors-to-selection",
                                    &nreturn_vals,
                                    GIMP_PDB_VECTORS, vectors_ID,
                                    GIMP_PDB_INT32, operation,
                                    GIMP_PDB_INT32, antialias,
                                    GIMP_PDB_INT32, feather,
                                    GIMP_PDB_FLOAT, feather_radius_x,
                                    GIMP_PDB_FLOAT, feather_radius_y,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}
