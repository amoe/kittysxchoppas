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

void
connect_bus_signals (PlaybackApp * app)
{
  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (app->pipeline));

#if defined (GDK_WINDOWING_X11) || defined (GDK_WINDOWING_WIN32) || defined (GDK_WINDOWING_QUARTZ)
  if (app->pipeline_type != 0) {
    /* handle prepare-xwindow-id element message synchronously, but only for non-playbin */
    gst_bus_set_sync_handler (bus, (GstBusSyncHandler) bus_sync_handler, app,
        NULL);
  }
#endif

  gst_bus_add_signal_watch_full (bus, G_PRIORITY_HIGH);
  gst_bus_enable_sync_message_emission (bus);

  g_signal_connect (bus, "message::state-changed",
      G_CALLBACK (msg_state_changed), app);
  g_signal_connect (bus, "message::segment-done", G_CALLBACK (msg_segment_done),
      app);
  g_signal_connect (bus, "message::async-done", G_CALLBACK (msg_async_done),
      app);

  g_signal_connect (bus, "message::new-clock", G_CALLBACK (message_received),
      app);
  g_signal_connect (bus, "message::clock-lost", G_CALLBACK (msg_clock_lost),
      app);
  g_signal_connect (bus, "message::error", G_CALLBACK (message_received), app);
  g_signal_connect (bus, "message::warning", G_CALLBACK (message_received),
      app);
  g_signal_connect (bus, "message::eos", G_CALLBACK (msg_eos), app);
  g_signal_connect (bus, "message::tag", G_CALLBACK (message_received), app);
  g_signal_connect (bus, "message::element", G_CALLBACK (message_received),
      app);
  g_signal_connect (bus, "message::segment-done", G_CALLBACK (message_received),
      app);
  g_signal_connect (bus, "message::buffering", G_CALLBACK (msg_buffering), app);
//  g_signal_connect (bus, "message::step-done", G_CALLBACK (msg_step_done),
//      app);
  g_signal_connect (bus, "message::step-start", G_CALLBACK (msg_step_done),
      app);
  g_signal_connect (bus, "sync-message::step-done",
      G_CALLBACK (msg_sync_step_done), app);
  g_signal_connect (bus, "message", G_CALLBACK (msg), app);

  gst_object_unref (bus);
}
void
play_cb (GtkButton * button, PlaybackApp * app)
{
  GstStateChangeReturn ret;

  if (app->state != GST_STATE_PLAYING) {
    g_print ("PLAY pipeline\n");
    gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->status_id);

    if (app->pipeline_type == 0) {
      video_sink_activate_cb (GTK_ENTRY (app->video_sink_entry), app);
      audio_sink_activate_cb (GTK_ENTRY (app->audio_sink_entry), app);
      text_sink_activate_cb (GTK_ENTRY (app->text_sink_entry), app);
      buffer_size_activate_cb (GTK_ENTRY (app->buffer_size_entry), app);
      buffer_duration_activate_cb (GTK_ENTRY (app->buffer_duration_entry), app);
      ringbuffer_maxsize_activate_cb (GTK_ENTRY (app->ringbuffer_maxsize_entry),
          app);
      connection_speed_activate_cb (GTK_ENTRY (app->connection_speed_entry),
          app);
      av_offset_activate_cb (GTK_ENTRY (app->av_offset_entry), app);
      subtitle_encoding_activate_cb (GTK_ENTRY (app->subtitle_encoding_entry),
          app);
    }

    ret = gst_element_set_state (app->pipeline, GST_STATE_PLAYING);
    switch (ret) {
      case GST_STATE_CHANGE_FAILURE:
        goto failed;
      case GST_STATE_CHANGE_NO_PREROLL:
        app->is_live = TRUE;
        break;
      default:
        break;
    }
    app->state = GST_STATE_PLAYING;
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->status_id,
        "Playing");
  }

  return;

failed:
  {
    g_print ("PLAY failed\n");
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->status_id,
        "Play failed");
  }
}

void
pause_cb (GtkButton * button, PlaybackApp * app)
{
  g_mutex_lock (&app->state_mutex);
  if (app->state != GST_STATE_PAUSED) {
    GstStateChangeReturn ret;

    gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->status_id);
    g_print ("PAUSE pipeline\n");
    ret = gst_element_set_state (app->pipeline, GST_STATE_PAUSED);
    switch (ret) {
      case GST_STATE_CHANGE_FAILURE:
        goto failed;
      case GST_STATE_CHANGE_NO_PREROLL:
        app->is_live = TRUE;
        break;
      default:
        break;
    }

    app->state = GST_STATE_PAUSED;
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->status_id,
        "Paused");
  }
  g_mutex_unlock (&app->state_mutex);

  return;

failed:
  {
    g_mutex_unlock (&app->state_mutex);
    g_print ("PAUSE failed\n");
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->status_id,
        "Pause failed");
  }
}

void
reset_app (PlaybackApp * app)
{
  g_free (app->audiosink_str);
  g_free (app->videosink_str);

  g_list_free (app->formats);

  g_mutex_clear (&app->state_mutex);

  if (app->overlay_element)
    gst_object_unref (app->overlay_element);
  if (app->navigation_element)
    gst_object_unref (app->navigation_element);

  g_list_foreach (app->paths, (GFunc) g_free, NULL);
  g_list_free (app->paths);
  g_list_foreach (app->sub_paths, (GFunc) g_free, NULL);
  g_list_free (app->sub_paths);

  g_print ("free pipeline\n");
  gst_object_unref (app->pipeline);
}
