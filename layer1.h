#ifndef LAYER1_H
#define LAYER1_H

#include "kittysxchoppas.h"

void set_defaults(PlaybackApp * app);
void print_usage(int argc, char **argv);
GList *handle_wildcards (const gchar *location);
void create_ui(PlaybackApp *app);
void connect_bus_signals(PlaybackApp *app);
void play_cb(GtkButton * button, PlaybackApp *app);
void pause_cb(GtkButton * button, PlaybackApp *app);
void reset_app(PlaybackApp *app);

#endif /* LAYER1_H */
