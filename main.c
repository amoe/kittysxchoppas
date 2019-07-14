#include <gtk/gtk.h>
#include <gst/video/videooverlay.h>
#include <stdlib.h>
#include "layer1.h"
#include "kittysxchoppas.h"


int main (int argc, char **argv) {
    PlaybackApp app;
    GOptionEntry options[] = {
        {"stats", 's', 0, G_OPTION_ARG_NONE, &app.stats,
         "Show pad stats", NULL},
        {"verbose", 'v', 0, G_OPTION_ARG_NONE, &app.verbose,
         "Verbose properties", NULL},
        {NULL}
    };
    GOptionContext *ctx;
    GError *err = NULL;

    set_defaults (&app);

    ctx = g_option_context_new ("- playback testing in gsteamer");
    g_option_context_add_main_entries (ctx, options, NULL);
    g_option_context_add_group (ctx, gst_init_get_option_group ());
    g_option_context_add_group (ctx, gtk_get_option_group (TRUE));

    if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
        g_print ("Error initializing: %s\n", err->message);
        exit (1);
    }

    GST_DEBUG_CATEGORY_INIT (playback_debug, "playback-test", 0,
                             "playback example");

    if (argc < 2) {
        print_usage (argc, argv);
        exit (-1);
    }

    app.pipeline_type = 0;

    if (app.pipeline_type < 0 || app.pipeline_type >= G_N_ELEMENTS (pipelines)) {
        print_usage (argc, argv);
        exit (-1);
    }

    app.pipeline_spec = argv[1];

    if (g_path_is_absolute (app.pipeline_spec) &&
        (g_strrstr (app.pipeline_spec, "*") != NULL ||
         g_strrstr (app.pipeline_spec, "?") != NULL)) {
        app.paths = handle_wildcards (app.pipeline_spec);
    } else {
        app.paths = g_list_prepend (app.paths, g_strdup (app.pipeline_spec));
    }

    if (!app.paths) {
        g_print ("opening %s failed\n", app.pipeline_spec);
        exit (-1);
    }

    app.current_path = app.paths;

    if (argc > 2 && argv[2]) {
        if (g_path_is_absolute (argv[2]) &&
            (g_strrstr (argv[2], "*") != NULL ||
             g_strrstr (argv[2], "?") != NULL)) {
            app.sub_paths = handle_wildcards (argv[2]);
        } else {
            app.sub_paths = g_list_prepend (app.sub_paths, g_strdup (argv[2]));
        }

        if (!app.sub_paths) {
            g_print ("opening %s failed\n", argv[2]);
            exit (-1);
        }

        app.current_sub_path = app.sub_paths;
    }

    pipelines[app.pipeline_type].func (&app, app.current_path->data);
    g_assert (app.pipeline);

    create_ui (&app);

    /* show the gui. */
    gtk_widget_show_all (app.window);

    /* realize window now so that the video window gets created and we can
     * obtain its XID before the pipeline is started up and the videosink
     * asks for the XID of the window to render onto */
    gtk_widget_realize (app.window);

#if defined (GDK_WINDOWING_X11) || defined (GDK_WINDOWING_WIN32) || defined (GDK_WINDOWING_QUARTZ)
    /* we should have the XID now */
    g_assert (app.embed_xid != 0);

    if (app.pipeline_type == 0) {
        gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (app.pipeline),
                                             app.embed_xid);
    }
#endif

    if (app.verbose) {
        g_signal_connect (app.pipeline, "deep_notify",
                          G_CALLBACK (gst_object_default_deep_notify), NULL);
    }

    connect_bus_signals (&app);

    gtk_widget_grab_focus(app.seek_scale);

    play_cb(NULL, &app);
    pause_cb(NULL, &app);
  
    gtk_main ();

    g_print ("NULL pipeline\n");
    gst_element_set_state (app.pipeline, GST_STATE_NULL);

    reset_app (&app);

    return 0;
}
