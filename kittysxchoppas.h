#ifndef KITTYSXCHOPPAS_H
#define KITTYSXCHOPPAS_H

#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC (playback_debug);
#define GST_CAT_DEFAULT (playback_debug)


/* Copied from gst-plugins-base/gst/playback/gstplay-enum.h */
typedef enum
{
  GST_PLAY_FLAG_VIDEO = (1 << 0),
  GST_PLAY_FLAG_AUDIO = (1 << 1),
  GST_PLAY_FLAG_TEXT = (1 << 2),
  GST_PLAY_FLAG_VIS = (1 << 3),
  GST_PLAY_FLAG_SOFT_VOLUME = (1 << 4),
  GST_PLAY_FLAG_NATIVE_AUDIO = (1 << 5),
  GST_PLAY_FLAG_NATIVE_VIDEO = (1 << 6),
  GST_PLAY_FLAG_DOWNLOAD = (1 << 7),
  GST_PLAY_FLAG_BUFFERING = (1 << 8),
  GST_PLAY_FLAG_DEINTERLACE = (1 << 9),
  GST_PLAY_FLAG_SOFT_COLORBALANCE = (1 << 10),
  GST_PLAY_FLAG_FORCE_FILTERS = (1 << 11),
} GstPlayFlags;


/* configuration */

#define FILL_INTERVAL 100
//#define UPDATE_INTERVAL 500
//#define UPDATE_INTERVAL 100
#define UPDATE_INTERVAL 40

/* number of milliseconds to play for after a seek */
#define SCRUB_TIME 100

/* timeout for gst_element_get_state() after a seek */
#define SEEK_TIMEOUT 40 * GST_MSECOND

#define DEFAULT_VIDEO_HEIGHT 300

/* the state to go to when stop is pressed */
#define STOP_STATE      GST_STATE_READY

#define N_GRAD 1000.0


struct timestamp {
  long hours;
  long minutes;
  long seconds;
  long fractional_seconds;
};

/* we keep an array of the visualisation entries so that we can easily switch
 * with the combo box index. */
typedef struct
{
  GstElementFactory *factory;
} VisEntry;



typedef struct
{
  /* GTK widgets */
  GtkWidget *window;
  GtkWidget *video_combo, *audio_combo, *text_combo, *vis_combo;
  GtkWidget *video_window;

  GtkWidget *vis_checkbox, *video_checkbox, *audio_checkbox;
  GtkWidget *text_checkbox, *mute_checkbox, *volume_spinbutton;
  GtkWidget *soft_volume_checkbox, *native_audio_checkbox,
      *native_video_checkbox;
  GtkWidget *download_checkbox, *buffering_checkbox, *deinterlace_checkbox;
  GtkWidget *soft_colorbalance_checkbox;
  GtkWidget *video_sink_entry, *audio_sink_entry, *text_sink_entry;
  GtkWidget *buffer_size_entry, *buffer_duration_entry;
  GtkWidget *ringbuffer_maxsize_entry, *connection_speed_entry;
  GtkWidget *av_offset_entry, *subtitle_encoding_entry;
  GtkWidget *subtitle_fontdesc_button;

  GtkWidget *seek_format_combo, *seek_position_label, *seek_duration_label;
  GtkWidget *seek_entry;

  GtkWidget *seek_scale, *statusbar;
  guint status_id;

  GtkWidget *step_format_combo, *step_amount_spinbutton, *step_rate_spinbutton;
  GtkWidget *shuttle_scale;

  GtkWidget *contrast_scale, *brightness_scale, *hue_scale, *saturation_scale;

  struct
  {
    GstNavigationCommand cmd;
    GtkWidget *button;
  } navigation_buttons[14];

  guintptr embed_xid;

  /* GStreamer pipeline */
  GstElement *pipeline;

  GstElement *navigation_element;
  GstElement *colorbalance_element;
  GstElement *overlay_element;

  /* Settings */
  gboolean accurate_seek;
  gboolean keyframe_seek;
  gboolean loop_seek;
  gboolean flush_seek;
  gboolean scrub;
  gboolean play_scrub;
  gboolean skip_seek;
  gdouble rate;
  gboolean snap_before;
  gboolean snap_after;

  /* From commandline parameters */
  gboolean stats;
  gboolean verbose;
  const gchar *pipeline_spec;
  gint pipeline_type;
  GList *paths, *current_path;
  GList *sub_paths, *current_sub_path;

  gchar *audiosink_str, *videosink_str;

  /* Internal state */
  gint64 position, duration;

  gboolean is_live;
  gboolean buffering;
  GstBufferingMode mode;
  gint64 buffering_left;
  GstState state;
  guint update_id;
  guint seek_timeout_id;        /* Used for scrubbing in paused */
  gulong changed_id;
  guint fill_id;

  gboolean need_streams;
  gint n_video, n_audio, n_text;

  GMutex state_mutex;

  GArray *vis_entries;          /* Array of VisEntry structs */

  gboolean shuttling;
  gdouble shuttle_rate;
  gdouble play_rate;

  const GstFormatDefinition *seek_format;
  GList *formats;

  gint64 marker_a_position;
  gint64 marker_b_position;
  guint8 number_cuts;
  GtkWidget *marker_a_display, *marker_b_display;
  GtkWidget *back_ten_button, *forward_ten_button;
  GtkWidget *frame_accurate_checkbox;
  
} PlaybackApp;


static void clear_streams (PlaybackApp * app);
static void find_interface_elements (PlaybackApp * app);
static void volume_notify_cb (GstElement * pipeline, GParamSpec * arg,
    PlaybackApp * app);
static void mute_notify_cb (GstElement * pipeline, GParamSpec * arg,
    PlaybackApp * app);


static void video_sink_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void text_sink_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void audio_sink_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void buffer_size_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void buffer_duration_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void ringbuffer_maxsize_activate_cb (GtkEntry * entry,
    PlaybackApp * app);
static void connection_speed_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void av_offset_activate_cb (GtkEntry * entry, PlaybackApp * app);
static void subtitle_encoding_activate_cb (GtkEntry * entry, PlaybackApp * app);
static gchar *generate_frame_accurate_cut_command(
    char *input_path, char *output_path, double start_position,
    double end_position
);
static gchar *generate_keyframe_cut_command(
    char *input_path, char *output_path, double start_position,
    double end_position
);

static void
set_marker_a_cb (GtkButton * button, PlaybackApp * app);
static void
set_marker_b_cb (GtkButton * button, PlaybackApp * app);
static void
cut_cb (GtkButton * button, PlaybackApp * app);
static gchar *get_output_path(PlaybackApp *app);
static void update_marker_labels(PlaybackApp *app);
static void
seek_back_frame_cb (GtkButton * button, PlaybackApp * app);
static void
seek_forward_frame_cb (GtkButton * button, PlaybackApp * app);
static void
seek_ten_cb (GtkButton * button, PlaybackApp * app);


struct timestamp convert_ns_value(long ns_value, int precision);


#endif /* KITTYSXCHOPPAS_H */
