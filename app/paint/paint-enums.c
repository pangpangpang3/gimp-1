
/* Generated data (by gimp-mkenums) */

#include "config.h"
#include <glib-object.h>
#include "libgimpbase/gimpbase.h"
#include "paint-enums.h"
#include "gimp-intl.h"

/* enumerations from "./paint-enums.h" */
GType
gimp_clone_align_mode_get_type (void)
{
  static const GEnumValue values[] =
  {
    { GIMP_CLONE_ALIGN_NO, "GIMP_CLONE_ALIGN_NO", "no" },
    { GIMP_CLONE_ALIGN_YES, "GIMP_CLONE_ALIGN_YES", "yes" },
    { GIMP_CLONE_ALIGN_REGISTERED, "GIMP_CLONE_ALIGN_REGISTERED", "registered" },
    { GIMP_CLONE_ALIGN_FIXED, "GIMP_CLONE_ALIGN_FIXED", "fixed" },
    { 0, NULL, NULL }
  };

  static const GimpEnumDesc descs[] =
  {
    { GIMP_CLONE_ALIGN_NO, N_("Non-aligned"), NULL },
    { GIMP_CLONE_ALIGN_YES, N_("Aligned"), NULL },
    { GIMP_CLONE_ALIGN_REGISTERED, N_("Registered"), NULL },
    { GIMP_CLONE_ALIGN_FIXED, N_("Fixed"), NULL },
    { 0, NULL, NULL }
  };

  static GType type = 0;

  if (! type)
    {
      type = g_enum_register_static ("GimpCloneAlignMode", values);
      gimp_enum_set_value_descriptions (type, descs);
    }

  return type;
}

GType
gimp_convolve_type_get_type (void)
{
  static const GEnumValue values[] =
  {
    { GIMP_BLUR_CONVOLVE, "GIMP_BLUR_CONVOLVE", "blur-convolve" },
    { GIMP_SHARPEN_CONVOLVE, "GIMP_SHARPEN_CONVOLVE", "sharpen-convolve" },
    { 0, NULL, NULL }
  };

  static const GimpEnumDesc descs[] =
  {
    { GIMP_BLUR_CONVOLVE, N_("Blur"), NULL },
    { GIMP_SHARPEN_CONVOLVE, N_("Sharpen"), NULL },
    { 0, NULL, NULL }
  };

  static GType type = 0;

  if (! type)
    {
      type = g_enum_register_static ("GimpConvolveType", values);
      gimp_enum_set_value_descriptions (type, descs);
    }

  return type;
}

GType
gimp_ink_blob_type_get_type (void)
{
  static const GEnumValue values[] =
  {
    { GIMP_INK_BLOB_TYPE_ELLIPSE, "GIMP_INK_BLOB_TYPE_ELLIPSE", "ellipse" },
    { GIMP_INK_BLOB_TYPE_SQUARE, "GIMP_INK_BLOB_TYPE_SQUARE", "square" },
    { GIMP_INK_BLOB_TYPE_DIAMOND, "GIMP_INK_BLOB_TYPE_DIAMOND", "diamond" },
    { 0, NULL, NULL }
  };

  static const GimpEnumDesc descs[] =
  {
    { GIMP_INK_BLOB_TYPE_ELLIPSE, "GIMP_INK_BLOB_TYPE_ELLIPSE", NULL },
    { GIMP_INK_BLOB_TYPE_SQUARE, "GIMP_INK_BLOB_TYPE_SQUARE", NULL },
    { GIMP_INK_BLOB_TYPE_DIAMOND, "GIMP_INK_BLOB_TYPE_DIAMOND", NULL },
    { 0, NULL, NULL }
  };

  static GType type = 0;

  if (! type)
    {
      type = g_enum_register_static ("GimpInkBlobType", values);
      gimp_enum_set_value_descriptions (type, descs);
    }

  return type;
}


/* Generated data ends here */

