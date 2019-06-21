#include <string.h>
#include <gtk/gtk.h>
#include "kittysxchoppas.h"
#include "layer1.h"


void
set_defaults (PlaybackApp * app)
{
  memset (app, 0, sizeof (PlaybackApp));

  app->flush_seek = TRUE;
  app->scrub = TRUE;
  app->rate = 1.0;

  app->position = app->duration = -1;
  app->state = GST_STATE_NULL;

  app->need_streams = TRUE;

  g_mutex_init (&app->state_mutex);

  app->play_rate = 1.0;

  app->marker_a_position = 0;
  app->marker_a_position = 0;
  app->number_cuts = 0;
}

void
print_usage (int argc, char **argv)
{
  gint i;

  g_print ("usage: %s <type> <filename>\n", argv[0]);
  g_print ("   possible types:\n");

  for (i = 0; i < G_N_ELEMENTS (pipelines); i++) {
    g_print ("     %d = %s\n", i, pipelines[i].name);
  }
}

/* Return GList of paths described in location string */
GList *
handle_wildcards (const gchar * location)
{
  GList *res = NULL;
  gchar *path = g_path_get_dirname (location);
  gchar *pattern = g_path_get_basename (location);
  GPatternSpec *pspec = g_pattern_spec_new (pattern);
  GDir *dir = g_dir_open (path, 0, NULL);
  const gchar *name;

  g_print ("matching %s from %s\n", pattern, path);

  if (!dir) {
    g_print ("opening directory %s failed\n", path);
    goto out;
  }

  while ((name = g_dir_read_name (dir)) != NULL) {
    if (g_pattern_match_string (pspec, name)) {
      res = g_list_append (res, g_strjoin ("/", path, name, NULL));
      g_print ("  found clip %s\n", name);
    }
  }

  g_dir_close (dir);
out:
  g_pattern_spec_free (pspec);
  g_free (pattern);
  g_free (path);

  return res;
}
