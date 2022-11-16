#include "replay_server.c"

#include <stdio.h>

GST_DEBUG_CATEGORY(rtsp_streamer);

static GMainLoop *loop;
static gboolean quit = FALSE;
static gboolean cintr = FALSE;

static gboolean check_for_interrupt(gpointer data) {
    if (quit) {
        return FALSE;
    }

    if (cintr) {
        cintr = FALSE;

        quit = TRUE;
        g_main_loop_quit(loop);

        return FALSE;
    }
    return TRUE;
}

static void _intr_handler(int signum) {
    struct sigaction action;

    g_print("User Interrupted.. \n");

    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_DFL;

    sigaction(SIGINT, &action, NULL);

    cintr = TRUE;
}

static void _intr_setup() {
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = _intr_handler;

    sigaction(SIGINT, &action, NULL);
}

static int rtsp_port;
static gchar *input_file;

GOptionEntry entries[] = {
    {"rtsp_port", 'p', 0, G_OPTION_ARG_INT, &rtsp_port, "rtsp_port", NULL},
    {"input_file", 'i', 0, G_OPTION_ARG_FILENAME, &input_file, "input_file", NULL},
    {NULL},
};

int main(int argc, char *argv[]) {
    int return_value = 0;

    GOptionContext *ctx = g_option_context_new("file to rtsp streamer");
    GOptionGroup *group = g_option_group_new("abc", NULL, NULL, NULL, NULL);
    g_option_group_add_entries(group, entries);

    g_option_context_set_main_group(ctx, group);
    g_option_context_add_group(ctx, gst_init_get_option_group());

    GError *error = NULL;
    if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
        g_print("%s\n", error->message);
        return -1;
    }

    if (!input_file) {
        g_print("No input_file provided, use -i video.mp4\n");
        return -1;
    }

    if (!gst_uri_is_valid(input_file)) {
        g_printerr(
            "Unrecognised command line argument '%s'.\n"
            "Please pass an URI or file as argument!\n",
            input_file);
        return -1;
    }

    if (!rtsp_port) {
        rtsp_port = 8554;
    }

    loop = g_main_loop_new(NULL, FALSE);

    GstRTSPServer *server = gst_rtsp_server_new();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_replay_new(input_file, -1);

    char port_str[64] = {0};
    sprintf(port_str, "%d", rtsp_port);
    g_object_set(server, "service", port_str, NULL);

    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);

    g_object_unref(mounts);

    gst_rtsp_server_attach(server, NULL);

    g_print("\n *** rtspstreamer: Launched RTSP Streaming at rtsp://localhost:%d/test ***\n\n",
            rtsp_port);

    _intr_setup();
    g_timeout_add(400, check_for_interrupt, NULL);
    g_main_loop_run(loop);

    return return_value;
}
