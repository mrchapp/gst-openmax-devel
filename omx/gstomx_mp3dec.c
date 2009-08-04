/*
 * Copyright (C) 2007-2009 Nokia Corporation.
 *
 * Author: Felipe Contreras <felipe.contreras@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "gstomx_mp3dec.h"
#include "gstomx.h"

#include <string.h> /* for memset */

#include <omx/TIDspOmx.h>

GSTOMX_BOILERPLATE (GstOmxMp3Dec, gst_omx_mp3dec, GstOmxBaseAudioDec, GST_OMX_BASE_AUDIODEC_TYPE);

static GstCaps *
generate_src_template (void)
{
    GstCaps *caps;

    caps = gst_caps_new_simple ("audio/x-raw-int",
                                "endianness", G_TYPE_INT, G_BYTE_ORDER,
                                "width", G_TYPE_INT, 16,
                                "depth", G_TYPE_INT, 16,
                                "rate", GST_TYPE_INT_RANGE, 8000, 96000,
                                "signed", G_TYPE_BOOLEAN, TRUE,
                                "channels", GST_TYPE_INT_RANGE, 1, 2,
                                NULL);

    return caps;
}

static GstCaps *
generate_sink_template (void)
{
    GstCaps *caps;

    caps = gst_caps_new_simple ("audio/mpeg",
                                "mpegversion", G_TYPE_INT, 1,
                                "layer", G_TYPE_INT, 3,
                                "rate", GST_TYPE_INT_RANGE, 8000, 48000,
                                "channels", GST_TYPE_INT_RANGE, 1, 8,
                                "parsed", G_TYPE_BOOLEAN, TRUE,
                                NULL);

    return caps;
}

static void
type_base_init (gpointer g_class)
{
    GstElementClass *element_class;

    element_class = GST_ELEMENT_CLASS (g_class);

    {
        GstElementDetails details;

        details.longname = "OpenMAX IL MP3 audio decoder";
        details.klass = "Codec/Decoder/Audio";
        details.description = "Decodes audio in MP3 format with OpenMAX IL";
        details.author = "Felipe Contreras";

        gst_element_class_set_details (element_class, &details);
    }

    {
        GstPadTemplate *template;

        template = gst_pad_template_new ("src", GST_PAD_SRC,
                                         GST_PAD_ALWAYS,
                                         generate_src_template ());

        gst_element_class_add_pad_template (element_class, template);
    }

    {
        GstPadTemplate *template;

        template = gst_pad_template_new ("sink", GST_PAD_SINK,
                                         GST_PAD_ALWAYS,
                                         generate_sink_template ());

        gst_element_class_add_pad_template (element_class, template);
    }
}

static void
omx_setup (GstOmxBaseFilter *omx_base)
{
    GOmxCore *gomx = omx_base->gomx;

GST_DEBUG_OBJECT (omx_base, "setting frame-mode");

    /* This is specific for TI. */
    {
        OMX_INDEXTYPE index;
        TI_OMX_DSP_DEFINITION audioinfo;

        memset (&audioinfo, 0, sizeof (audioinfo));

        audioinfo.framemode = TRUE; /* TRUE = frame mode */

        OMX_GetExtensionIndex (gomx->omx_handle, "OMX.TI.index.config.mp3headerinfo", &index);

        OMX_SetConfig (gomx->omx_handle, index, &audioinfo);

        GST_DEBUG_OBJECT (omx_base, "OMX_SetConfig OMX.TI.index.config.mp3headerinfo");
    }
}
static void
type_class_init (gpointer g_class,
                 gpointer class_data)
{
}

static void
type_instance_init (GTypeInstance *instance,
                    gpointer g_class)
{
	GstOmxBaseFilter *omx_base;

	omx_base = GST_OMX_BASE_FILTER (instance);

	GST_DEBUG_OBJECT (omx_base, "start");

	omx_base->omx_setup = omx_setup;

}
