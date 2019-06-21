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
