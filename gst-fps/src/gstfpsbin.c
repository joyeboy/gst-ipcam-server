/**
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2009 Nguyen Thanh Trung <nguyenthanh.trung@nomovok.com>
 * Copyright (C) 2009 Dam Quang Tuan <damquang.tuan@nomovok.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-fpsbin
 *
 * A GstBin that can change video frame rate run-time.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m videotestsrc ! fpsbin framerate='5/1' ! autovideosink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>

#include "gstfpsbin.h"

GST_DEBUG_CATEGORY_STATIC(gst_fps_bin_debug);
#define GST_CAT_DEFAULT gst_fps_bin_debug

/* Filter signals and args */
enum {
	/* FILL ME */
	LAST_SIGNAL
};

/** values for fps property */
#define FPS_MAX_NUMERATOR		G_MAXINT
#define FPS_MAX_DENOMINATOR		1
#define FPS_MIN_NUMERATOR		0
#define FPS_MIN_DENOMINATOR		1
#define FPS_DEFAULT_NUMERATOR	30
#define FPS_DEFAULT_DENOMINATOR	1

/**
 * properties list
 */
enum {
	PROP_0,
	PROP_FRAMERATE
};

/**
 * the capabilities of the inputs and outputs.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS("ANY")
		);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS("ANY")
		);

GST_BOILERPLATE(GstFpsBin, gst_fps_bin, GstBin,
		GST_TYPE_BIN);

static void gst_fps_bin_set_property(GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec);
static void gst_fps_bin_get_property(GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec);

static void gst_fps_bin_set_framerate(GstFpsBin * fps_bin, guint numerator, guint demonirator);

/** support for set_fps functions */
static void gst_fps_bin_stop_pipeline(GstFpsBin * fps_bin);
static void gst_fps_bin_start_pipeline(GstFpsBin * fps_bin);

/** GObject vmethod implementations */

/**
 * init base object for fps_bin
 * This function is generated automatically using gst-template
 */
static void gst_fps_bin_base_init(gpointer gclass) {
	GstElementClass *element_class = GST_ELEMENT_CLASS(gclass);

	gst_element_class_set_details_simple(element_class,
			"FpsBin",
			"Filter/Bin/Video",
			"Runtime adjust video fps bin",
			"Nguyen Thanh Trung <nguyenthanh.trung@nomovok.com> & Dam Quang Tuan <damquang.tuan@nomovok.com>");

	gst_element_class_add_pad_template(element_class,
			gst_static_pad_template_get(&src_factory));
	gst_element_class_add_pad_template(element_class,
			gst_static_pad_template_get(&sink_factory));
}

/**
 * initialize the fpsbin's class
 * autogenerated by gst-template
 */
static void gst_fps_bin_class_init(GstFpsBinClass * klass) {
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_fps_bin_set_property;
	gobject_class->get_property = gst_fps_bin_get_property;

	// install fps property
	g_object_class_install_property(gobject_class, PROP_FRAMERATE,
			gst_param_spec_fraction("framerate", "Framerate", "frame rate value",
			FPS_MIN_NUMERATOR, FPS_MIN_DENOMINATOR,
			FPS_MAX_NUMERATOR, FPS_MAX_DENOMINATOR,
			FPS_DEFAULT_NUMERATOR, FPS_DEFAULT_DENOMINATOR,
			G_PARAM_READWRITE));
}

/**
 * initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 * auto-generate by gst-template
 *
 * @param fps_bin GstFpsBin* the bin that we want to use
 * @param gclass GstFpsBinClass* class variable of our bin
 */
static void gst_fps_bin_init(GstFpsBin * fps_bin, GstFpsBinClass * gclass) {
	GstPad *pad;

	// init child elements
	fps_bin->videorate = gst_element_factory_make("videorate", "fps_bin_videorate");
	g_return_if_fail(fps_bin->videorate != NULL);

	fps_bin->capsfilter = gst_element_factory_make("capsfilter", "fps_bin_capsfilter");
	g_return_if_fail(fps_bin->capsfilter != NULL);

	// link them together
	gst_bin_add_many(GST_BIN(fps_bin), fps_bin->videorate, fps_bin->capsfilter, NULL);
	gst_element_link(fps_bin->videorate, fps_bin->capsfilter);

	// ghost the pads to our bin
	// sink pad
	pad = gst_element_get_static_pad(fps_bin->videorate, "sink");
	fps_bin->sinkpad = gst_ghost_pad_new("sink", pad);
	gst_object_unref(pad);
	gst_pad_set_active(fps_bin->sinkpad, TRUE);
	gst_element_add_pad(GST_ELEMENT(fps_bin), fps_bin->sinkpad);

	// src pad
	pad = gst_element_get_static_pad(fps_bin->capsfilter, "src");
	fps_bin->srcpad = gst_ghost_pad_new("src", pad);
	gst_object_unref(pad);
	gst_pad_set_active(fps_bin->srcpad, TRUE);
	gst_element_add_pad(GST_ELEMENT(fps_bin), fps_bin->srcpad);

	// default value for fps
	fps_bin->numerator = 30;
	fps_bin->denominator = 1;
}

/**
 * Set the properties for our FpsBin object.
 * This method will be called automatically inside g_object_set*() functions.
 *
 * @param object GObject* pointer to our FpsBin
 * @param prop_id guint the id of the property that we want to read
 * @param value GValue* pointer to value we will set to our property
 * @param pspec GParamSpec* some description for our property
 */
static void gst_fps_bin_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
	GstFpsBin *filter = GST_FPSBIN(object);

	switch (prop_id) {
		case PROP_FRAMERATE:
		{
			gint numerator = 0, denominator = 0;
			numerator = gst_value_get_fraction_numerator(value);
			denominator = gst_value_get_fraction_denominator(value);
			gst_fps_bin_set_framerate(filter, numerator, denominator);
		}
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

/**
 * get the properties method for our FpsBin object.
 * This method will be called inside g_object_get() or g_object_get_property()
 *
 * @param object GObject* our object to get properties
 * @param prop_id guint the id of property we want to get
 * @param value GValue* not NULL GValue variable to store property value
 * @param pspec GParamSpec* some description for our property
 */
static void gst_fps_bin_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
	GstFpsBin *filter = GST_FPSBIN(object);

	switch (prop_id) {
		case PROP_FRAMERATE:
			gst_value_set_fraction(value, filter->numerator, filter->denominator);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
	}
}

/**
 * set the framerate value for our bin.
 * The framerate value will be applied at run time
 * Gstreamer use fraction for storing framerate value, so we have 2 values here: numerator and denominator
 *
 * @param fps_bin GstFpsBin* fps_bin element
 * @param numerator guint numerator value of framerate
 * @param denominator guint denominator value of framerate
 */
static void gst_fps_bin_set_framerate(GstFpsBin* fps_bin, guint numerator, guint denominator) {
	GstCaps * caps = gst_caps_new_simple("video/x-raw-yuv",
			"framerate", GST_TYPE_FRACTION, numerator, denominator,
			NULL);
	gst_fps_bin_stop_pipeline(fps_bin);
	g_object_set(fps_bin->capsfilter, "caps", caps, NULL);
	gst_fps_bin_start_pipeline(fps_bin);
	fps_bin->numerator = numerator;
	fps_bin->denominator = denominator;
	gst_caps_unref(caps);
}

/**
 * stop the pipeline if needed
 * We need to stop pipeline and then start it again for renegotiation process
 *
 * @param fps_bin GstFpsBin* our bin to get the pipeline
 */
static void gst_fps_bin_stop_pipeline(GstFpsBin* fps_bin) {
	GstElement * pipeline = GST_ELEMENT(gst_element_get_parent(fps_bin));
	if (pipeline != NULL) {
		gst_element_set_state(pipeline, GST_STATE_READY);
	}
}

/**
 * start the pipeline if needed
 *
 * @param fps_bin GstFpsBin* our bin to get the pipeline
 */
static void gst_fps_bin_start_pipeline(GstFpsBin* fps_bin) {
	GstElement * pipeline = GST_ELEMENT(gst_element_get_parent(fps_bin));
	if (pipeline != NULL) {
		gst_element_set_state(pipeline, GST_STATE_PLAYING);
	}
}

/**
 * entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 *
 * @param fpsbin GstPlugin* our plugin
 * @return TRUE if everything is ok, FALSE otherwise
 */
static gboolean fpsbin_init(GstPlugin * fpsbin) {
	/* debug category for fltering log messages
	 *
	 */
	GST_DEBUG_CATEGORY_INIT(gst_fps_bin_debug, "fpsbin",
			0, "runtime adjust video framerate");

	return gst_element_register(fpsbin, "fpsbin", GST_RANK_NONE,
			GST_TYPE_FPSBIN);
}

/**
 * gstreamer looks for this structure to register fpsbins
 */
GST_PLUGIN_DEFINE(
		GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		"fpsbin",
		"runtime adjust video framerate",
		fpsbin_init,
		VERSION,
		"LGPL",
		"GStreamer",
		"http://gstreamer.net/"
		)
