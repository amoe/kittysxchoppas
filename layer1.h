#ifndef LAYER1_H
#define LAYER1_H

#include "kittysxchoppas.h"

void set_defaults(PlaybackApp * app);
void
print_usage (int argc, char **argv);

GList *
handle_wildcards (const gchar * location);

#endif /* LAYER1_H */
