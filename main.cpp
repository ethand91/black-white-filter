#include <gst/gst.h>
#include <glib.h>

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
    GstPad *sinkpad;
    GstElement *decoder = (GstElement *) data;

    sinkpad = gst_element_get_static_pad(decoder, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}

int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        g_printerr("Usage: %s <MP4 File>\n", argv[0]);

	return -1;
    }

    GstElement *pipeline, *source, *demuxer, *decoder, *conv, *filter, *sink;
    GstBus *bus;
    GstMessage *msg;
    GMainLoop *loop;

    gst_init(&argc, &argv);

    pipeline = gst_pipeline_new("video-black-white");
    source = gst_element_factory_make("filesrc", "source");
    demuxer = gst_element_factory_make("qtdemux", "demuxer");
    decoder = gst_element_factory_make("avdec_h264", "decoder");
    conv = gst_element_factory_make("videoconvert", "converter");
    filter = gst_element_factory_make("videobalance", "filter");
    sink = gst_element_factory_make("autovideosink", "sink");

    if (!pipeline || !source || !demuxer || !decoder || !conv || !filter || !sink) {
        g_printerr("Not all elements could be created.\n");

        return -1;
    }

    const char *video_file_path = argv[1];

    g_object_set(G_OBJECT(source), "location", video_file_path, NULL);
    g_object_set(G_OBJECT(filter), "saturation", 0.0, NULL);

    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, decoder, conv, filter, sink, NULL);

    gst_element_link(source, demuxer);
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), decoder);
    gst_element_link_many(decoder, conv, filter, sink, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (msg != NULL)
    {
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
