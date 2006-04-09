/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * plug-ins-help.c
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

#include "config.h"

#include <string.h>

#include <glib-object.h>

#include "plug-in-types.h"

#include "core/gimp.h"

#include "plug-ins-help.h"


typedef struct _PlugInHelpDomain PlugInHelpDomain;

struct _PlugInHelpDomain
{
  gchar *prog_name;
  gchar *domain_name;
  gchar *domain_uri;
};


void
plug_ins_help_exit (Gimp *gimp)
{
  GSList *list;

  g_return_if_fail (GIMP_IS_GIMP (gimp));

  for (list = gimp->plug_in_help_domains; list; list = list->next)
    {
      PlugInHelpDomain *domain = list->data;

      g_free (domain->prog_name);
      g_free (domain->domain_name);
      g_free (domain->domain_uri);
      g_free (domain);
    }

  g_slist_free (gimp->plug_in_help_domains);
  gimp->plug_in_help_domains = NULL;
}

void
plug_ins_help_domain_add (Gimp        *gimp,
                          const gchar *prog_name,
                          const gchar *domain_name,
                          const gchar *domain_uri)
{
  PlugInHelpDomain *domain;

  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (prog_name != NULL);
  g_return_if_fail (domain_name != NULL);

  domain = g_new (PlugInHelpDomain, 1);

  domain->prog_name   = g_strdup (prog_name);
  domain->domain_name = g_strdup (domain_name);
  domain->domain_uri  = g_strdup (domain_uri);

  gimp->plug_in_help_domains = g_slist_prepend (gimp->plug_in_help_domains,
                                                domain);

#ifdef VERBOSE
  g_print ("added help domain \"%s\" for base uri \"%s\"\n",
           domain->domain_name ? domain->domain_name : "(null)",
           domain->domain_uri  ? domain->domain_uri  : "(null)");
#endif
}

const gchar *
plug_ins_help_domain (Gimp         *gimp,
                      const gchar  *prog_name,
                      const gchar **domain_uri)
{
  GSList *list;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  if (domain_uri)
    *domain_uri = NULL;

  /*  A NULL prog_name is GIMP itself, return the default domain  */
  if (! prog_name)
    return NULL;

  for (list = gimp->plug_in_help_domains; list; list = list->next)
    {
      PlugInHelpDomain *domain = list->data;

      if (domain && domain->prog_name &&
          ! strcmp (domain->prog_name, prog_name))
        {
          if (domain_uri && domain->domain_uri)
            *domain_uri = domain->domain_uri;

          return domain->domain_name;
        }
    }

  return NULL;
}

gint
plug_ins_help_domains (Gimp    *gimp,
                       gchar ***help_domains,
                       gchar ***help_uris)
{
  GSList *list;
  gint    n_domains;
  gint    i;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), 0);
  g_return_val_if_fail (help_domains != NULL, 0);
  g_return_val_if_fail (help_uris != NULL, 0);

  n_domains = g_slist_length (gimp->plug_in_help_domains);

  *help_domains = g_new0 (gchar *, n_domains);
  *help_uris    = g_new0 (gchar *, n_domains);

  for (list = gimp->plug_in_help_domains, i = 0; list; list = list->next, i++)
    {
      PlugInHelpDomain *domain = list->data;

      (*help_domains)[i] = g_strdup (domain->domain_name);
      (*help_uris)[i]    = g_strdup (domain->domain_uri);
    }

  return n_domains;
}
