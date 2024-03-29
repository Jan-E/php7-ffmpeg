/*
   This file is part of ffmpeg-php

   Copyright (C) 2004-2008 Todd Kirby (ffmpeg.php AT gmail.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   In addition, as a special exception, the copyright holders of ffmpeg-php
   give you permission to combine ffmpeg-php with code included in the
   standard release of PHP under the PHP license (or modified versions of
   such code, with unchanged license). You may copy and distribute such a
   system following the terms of the GNU GPL for ffmpeg-php and the licenses
   of the other code concerned, provided that you include the source code of
   that other code when and as the GNU GPL requires distribution of source code.

   You must obey the GNU General Public License in all respects for all of the
   code used other than standard release of PHP. If you modify this file, you
   may extend this exception to your version of the file, but you are not
   obligated to do so. If you do not wish to do so, delete this exception
   statement from your version.

 */

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

#include <avcodec.h>
#include <avformat.h>

#if LIBAVFORMAT_BUILD > 5770
#include <imgutils.h>
#endif

#ifndef CODEC_TYPE_VIDEO
#define CodecType AVMediaType
#define CODEC_TYPE_UNKNOWN    AVMEDIA_TYPE_UNKNOWN
#define CODEC_TYPE_VIDEO      AVMEDIA_TYPE_VIDEO
#define CODEC_TYPE_AUDIO      AVMEDIA_TYPE_AUDIO
#define CODEC_TYPE_DATA       AVMEDIA_TYPE_DATA
#define CODEC_TYPE_SUBTITLE   AVMEDIA_TYPE_SUBTITLE
#define CODEC_TYPE_ATTACHMENT AVMEDIA_TYPE_ATTACHMENT
#define CODEC_TYPE_NB         AVMEDIA_TYPE_NB
#define PKT_FLAG_KEY AV_PKT_FLAG_KEY
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_movie.h"
#include "ffmpeg_tools.h"

#define GET_MOVIE_RESOURCE(ffmovie_ctx) {\
    zend_resource *le;\
    if ((le = zend_hash_str_find_ptr(Z_OBJPROP_P(getThis()), "ffmpeg_movie", sizeof("ffmpeg_movie")-1)) == NULL) {\
        zend_error(E_WARNING, "Invalid ffmpeg_movie object");\
        RETURN_FALSE;\
    } else {\
        ffmovie_ctx = (ff_movie_context *)(le->ptr);\
    }\
\
}
#define LRINT(x) ((long) ((x)+0.5))

#if LIBAVFORMAT_BUILD > 5770
/* See libavcodec/avpicture.c */
#define av_picture_copy(dst, src, fmt, width, height)    \
    av_image_copy(dst->data, dst->linesize, (const uint8_t **)src->data,    \
                  src->linesize, fmt, width, height)
#endif
#if LIBAVFORMAT_BUILD > 4628
#define GET_CODEC_FIELD(codec, field) codec->field
#define GET_CODEC_PTR(codec) codec
#else
#define GET_CODEC_FIELD(codec, field) codec.field
#define GET_CODEC_PTR(codec) &codec
#endif

typedef struct {
    AVFormatContext *fmt_ctx;
    int64_t last_pts;
    int frame_number;
    zend_resource *rsrc_id;
    size_t    ncodec;
    AVCodecContext **codec_ctx;
} ff_movie_context;

static zend_class_entry *ffmpeg_movie_class_entry_ptr;
zend_class_entry ffmpeg_movie_class_entry;

static int le_ffmpeg_movie;
static int le_ffmpeg_pmovie;

/* {{{ ffmpeg_movie methods[]
    Methods of the ffmpeg_movie class
*/
zend_function_entry ffmpeg_movie_class_methods[] = {

#if PHP_MAJOR_VERSION >= 8
    /* contructor */
    FFMPEG_PHP_ME(ffmpeg_movie, __construct, arginfo_class_ffmpeg_movie___construct, 0)

    /* methods */
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getduration,         getDuration,         arginfo_class_ffmpeg_movie_getduration,         0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframecount,       getFrameCount,       arginfo_class_ffmpeg_movie_getframecount,       0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframerate,        getFrameRate,        arginfo_class_ffmpeg_movie_getframerate,        0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getfilename,         getFileName,         arginfo_class_ffmpeg_movie_getfilename,         0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getcomment,          getComment,          arginfo_class_ffmpeg_movie_getcomment,          0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, gettitle,            getTitle,            arginfo_class_ffmpeg_movie_gettitle,            0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getauthor,           getAuthor,           arginfo_class_ffmpeg_movie_getauthor,           0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getartist,           getAuthor,           arginfo_class_ffmpeg_movie_getartist,           0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getcopyright,        getCopyright,        arginfo_class_ffmpeg_movie_getcopyright,        0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getalbum,            getAlbum,            arginfo_class_ffmpeg_movie_getalbum,            0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getgenre,            getGenre,            arginfo_class_ffmpeg_movie_getgenre,            0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getyear,             getYear,             arginfo_class_ffmpeg_movie_getyear,             0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, gettracknumber,      getTrackNumber,      arginfo_class_ffmpeg_movie_gettracknumber,      0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframewidth,       getFrameWidth,       arginfo_class_ffmpeg_movie_getframewidth,       0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframeheight,      getFrameHeight,      arginfo_class_ffmpeg_movie_getframeheight,      0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframenumber,      getFrameNumber,      arginfo_class_ffmpeg_movie_getframenumber,      0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getpixelformat,      getPixelFormat,      arginfo_class_ffmpeg_movie_getpixelformat,      0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getbitrate,          getBitRate,          arginfo_class_ffmpeg_movie_getbitrate,          0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, hasaudio,            hasAudio,            arginfo_class_ffmpeg_movie_hasaudio,            0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, hasvideo,            hasVideo,            arginfo_class_ffmpeg_movie_hasvideo,            0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getnextkeyframe,     getNextKeyFrame,     arginfo_class_ffmpeg_movie_getnextkeyframe,     0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframe,            getFrame,            arginfo_class_ffmpeg_movie_getframe,            0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getvideocodec,       getVideoCodec,       arginfo_class_ffmpeg_movie_getvideocodec,       0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiocodec,       getAudioCodec,       arginfo_class_ffmpeg_movie_getaudiocodec,       0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getvideostreamid,    getVideoStreamId,    arginfo_class_ffmpeg_movie_getvideostreamid,    0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiostreamid,    getAudioStreamId,    arginfo_class_ffmpeg_movie_getaudiostreamid,    0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiochannels,    getAudioChannels,    arginfo_class_ffmpeg_movie_getaudiochannels,    0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiosamplerate,  getAudioSampleRate,  arginfo_class_ffmpeg_movie_getaudiosamplerate,  0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiobitrate,     getAudioBitRate,     arginfo_class_ffmpeg_movie_getaudiobitrate,     0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getvideobitrate,     getVideoBitRate,     arginfo_class_ffmpeg_movie_getvideobitrate,     0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getpixelaspectratio, getPixelAspectRatio, arginfo_class_ffmpeg_movie_getpixelaspectratio, 0)
#else
    /* contructor */
    FFMPEG_PHP_ME(ffmpeg_movie, __construct, NULL, 0)

    /* methods */
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getduration,         getDuration,         NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframecount,       getFrameCount,       NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframerate,        getFrameRate,        NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getfilename,         getFileName,         NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getcomment,          getComment,          NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, gettitle,            getTitle,            NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getauthor,           getAuthor,           NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getartist,           getAuthor,           NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getcopyright,        getCopyright,        NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getalbum,            getAlbum,            NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getgenre,            getGenre,            NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getyear,             getYear,             NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, gettracknumber,      getTrackNumber,      NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframewidth,       getFrameWidth,       NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframeheight,      getFrameHeight,      NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframenumber,      getFrameNumber,      NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getpixelformat,      getPixelFormat,      NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getbitrate,          getBitRate,          NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, hasaudio,            hasAudio,            NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, hasvideo,            hasVideo,            NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getnextkeyframe,     getNextKeyFrame,     NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getframe,            getFrame,            NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getvideocodec,       getVideoCodec,       NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiocodec,       getAudioCodec,       NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getvideostreamid,    getVideoStreamId,    NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiostreamid,    getAudioStreamId,    NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiochannels,    getAudioChannels,    NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiosamplerate,  getAudioSampleRate,  NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getaudiobitrate,     getAudioBitRate,     NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getvideobitrate,     getVideoBitRate,     NULL, 0)
    FFMPEG_PHP_MALIAS(ffmpeg_movie, getpixelaspectratio, getPixelAspectRatio, NULL, 0)
#endif
    FFMPEG_PHP_END_METHODS
};
/* }}} */

/* {{{ _php_get_ffmovie_ctx()
 */
static ff_movie_context* _php_get_ffmovie_ctx()
{
    zend_ulong numitems, i;
    zend_resource *le;
    ff_movie_context *ffmovie_ctx;

    numitems = zend_hash_next_free_element(&EG(regular_list));
    for (i=1; i<numitems; i++) {
        if ((le = zend_hash_index_find_ptr(&EG(regular_list), i)) == NULL) {
            continue;
        }
        if (le->type == le_ffmpeg_movie || le->type == le_ffmpeg_pmovie) {
            ffmovie_ctx = (ff_movie_context *)(le->ptr);
            return ffmovie_ctx;
        }
    }
    return 0;
}
/* }}} */

/* {{{ _php_get_stream_index()
 */
static int _php_get_stream_index(const AVFormatContext *fmt_ctx, int type)
{
    int i;
    //fprintf(stderr, "_php_get_stream_index fmt_ctx->nb_streams = %d\n", fmt_ctx->nb_streams);

    for (i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i] &&
                GET_CODEC_FIELD(fmt_ctx->streams[i]->codec, codec_type) == type) {
            return i;
        }
    }
    /* stream not found */
    return -1;
}
/* }}} */


/* {{{ _php_get_video_stream()
 */
static AVStream *_php_get_video_stream(AVFormatContext *fmt_ctx)
{
    int i = _php_get_stream_index(fmt_ctx, CODEC_TYPE_VIDEO);

    return i < 0 ? NULL : fmt_ctx->streams[i];
}
/* }}} */


/* {{{ _php_get_audio_stream()
 * TODO: Some containers can have multiple audio streams, so this
 *       will eventually need to be replaced by something smarter
 */
static AVStream *_php_get_audio_stream(AVFormatContext *fmt_ctx)
{
    int i = _php_get_stream_index(fmt_ctx, CODEC_TYPE_AUDIO);

    return i < 0 ? NULL : fmt_ctx->streams[i];
}
/* }}} */


static int has_audio(const ff_movie_context *ffmovie_ctx) {
    return _php_get_audio_stream(ffmovie_ctx->fmt_ctx) != NULL;
}


static int has_video(const ff_movie_context *ffmovie_ctx) {
    return _php_get_video_stream(ffmovie_ctx->fmt_ctx) != NULL;
}


/* {{{ _php_get_filename()
 */
const static char* _php_get_filename(const ff_movie_context *ffmovie_ctx)
{
    return ffmovie_ctx->fmt_ctx->filename;
}
/* }}} */


/* {{{ _php_alloc_ffmovie_ctx()
 */
static ff_movie_context* _php_alloc_ffmovie_ctx()
{
    ff_movie_context *ffmovie_ctx;

    ffmovie_ctx = ecalloc(1, sizeof(ff_movie_context));

    return ffmovie_ctx;
}
/* }}} */


/* {{{ _php_print_av_error()
 */
/*
static void _php_print_av_error(const char *filename, int err)
{
    switch(err) {
       case AVERROR_IO:
            zend_error(E_WARNING, "%s: I/O error.\n", filename);
            break;
       case AVERROR_NOMEM:
            zend_error(E_WARNING, "%s: Not enough memory.\n", filename);
            break;
        case AVERROR_NOTSUPP:
            zend_error(E_WARNING, "%s: Operation not supported.\n", filename);
            break;

       case AVERROR_NUMEXPECTED:
            zend_error(E_WARNING, "%s: Incorrect image filename syntax.\n", filename);
            break;
        case AVERROR_INVALIDDATA:
            zend_error(E_WARNING, "%s: Error while parsing header\n", filename);
            break;
        case AVERROR_NOFMT:
            zend_error(E_WARNING, "%s: Unknown format\n", filename);
        case AVERROR_UNKNOWN:
            // Fall thru to default case
        default:
            zend_error(E_WARNING, "%s: Error while opening file (%d)\n", filename, err);
            break;
    }
}
*/
/* }}} */


/* {{{ _php_open_movie_file()
 */
static int _php_open_movie_file(ff_movie_context *ffmovie_ctx,
        char* filename)
{
    if (ffmovie_ctx->fmt_ctx) {
        avformat_close_input(&ffmovie_ctx->fmt_ctx);
        ffmovie_ctx->fmt_ctx = NULL;
    }

    /* open the file with generic libav function */
    if (avformat_open_input(&ffmovie_ctx->fmt_ctx, filename, NULL, NULL) < 0) {
        return 1;
    }

    /* decode the first frames to get the stream parameters. */
    avformat_find_stream_info(ffmovie_ctx->fmt_ctx, NULL);

    return 0;
}
/* }}} */


/* {{{ proto object ffmpeg_movie(string filename)
   Constructor for ffmpeg_movie objects
 */
FFMPEG_PHP_CONSTRUCTOR(ffmpeg_movie, __construct)
{
    zval *argv = NULL;
    int ac = ZEND_NUM_ARGS();
    int persistent = 0;
    char *filename = NULL, *fullpath = NULL;
    ff_movie_context *ffmovie_ctx = NULL;
    int i;

    /* we pass additional args to the respective handler */
    argv = (zval *)safe_emalloc(sizeof(zval), ac, 0);
    if (zend_get_parameters_array_ex(ac, argv) != SUCCESS) {
        efree(argv);
        WRONG_PARAM_COUNT;
    }

    /* we only take string arguments */
    for (i = 0; i < ac; i++) {
        if (Z_TYPE(argv[i]) != IS_STRING) {
            convert_to_string_ex(&argv[i]);
        } else if (Z_REFCOUNTED(argv[i])) {
            Z_ADDREF(argv[i]);
        }
    }

    switch (ZEND_NUM_ARGS()) {
        case 2:
//            convert_to_boolean_ex(argv[1]);

            if (! INI_BOOL("ffmpeg.allow_persistent") && Z_STRVAL(argv[1])) {
                zend_error(E_WARNING,
                        "Persistent movies have been disabled in php.ini");
                break;
            }

            persistent = Z_LVAL(argv[1]);

            /* fallthru */
        case 1:
            filename = Z_STRVAL(argv[0]);
            break;
        default:
            WRONG_PARAM_COUNT;
    }

        ffmovie_ctx = _php_alloc_ffmovie_ctx();

        if (_php_open_movie_file(ffmovie_ctx, filename)) {
            zend_error(E_WARNING, "Can't open movie file %s",
                    filename);
            efree(argv);
            ZVAL_BOOL(getThis(), 0);
            RETURN_FALSE;
        }

        ffmovie_ctx->rsrc_id = zend_register_resource(ffmovie_ctx,
                le_ffmpeg_movie);
//        }

    add_property_resource(getThis(), "ffmpeg_movie", ffmovie_ctx->rsrc_id);

    efree(argv);
    if (fullpath) {
        efree(fullpath);
    }
}
/* }}} */

/* {{{ _php_free_ffmpeg_movie
 */
static void _php_free_ffmpeg_movie(zend_resource *rsrc)
{
    int i;
    ff_movie_context *ffmovie_ctx = (ff_movie_context*)rsrc->ptr;

    if (ffmovie_ctx == NULL)
        return;

    if (ffmovie_ctx->codec_ctx) {
        for (i = 0; i < ffmovie_ctx->ncodec; i++) {
            if (ffmovie_ctx->codec_ctx[i]) {
                avcodec_close(ffmovie_ctx->codec_ctx[i]);
            }
            ffmovie_ctx->codec_ctx[i] = NULL;
        }
    }

    avformat_close_input(&ffmovie_ctx->fmt_ctx);

    efree(ffmovie_ctx->fmt_ctx);
    efree(ffmovie_ctx);
}
/* }}} */


/* {{{ _php_free_ffmpeg_pmovie
 */
static void _php_free_ffmpeg_pmovie(zend_resource *rsrc)
{
    /* TODO: Factor into a single free function for pmovie and movie */
    int i;
    ff_movie_context *ffmovie_ctx = (ff_movie_context*)rsrc->ptr;

    if (ffmovie_ctx == NULL)
        return;

    if (ffmovie_ctx->codec_ctx) {
        for (i = 0; i < ffmovie_ctx->ncodec; i++) {
            if (ffmovie_ctx->codec_ctx[i]) {
                avcodec_close(ffmovie_ctx->codec_ctx[i]);
            }
            ffmovie_ctx->codec_ctx[i] = NULL;
        }
    }

    avformat_close_input(&ffmovie_ctx->fmt_ctx);

    efree(ffmovie_ctx->fmt_ctx);
    free(ffmovie_ctx);
}
/* }}} */


/* {{{ register_ffmpeg_movie_class()
 */
void register_ffmpeg_movie_class(int module_number)
{
    le_ffmpeg_movie = zend_register_list_destructors_ex(_php_free_ffmpeg_movie,
            NULL, "ffmpeg_movie", module_number);

    le_ffmpeg_pmovie = zend_register_list_destructors_ex(NULL,
            _php_free_ffmpeg_pmovie, "ffmpeg_pmovie", module_number);

    INIT_CLASS_ENTRY(ffmpeg_movie_class_entry, "ffmpeg_movie",
            ffmpeg_movie_class_methods);

    /* register ffmpeg movie class */
    ffmpeg_movie_class_entry_ptr =
        zend_register_internal_class(&ffmpeg_movie_class_entry);
}
/* }}} */


/* {{{ __php_get_decoder_context()
   Opens decoders and gets codec context. Always call this to get a pointer to
   the codec context. This allows to postpone codec init until a function that
   requires it is called.
 */
static AVCodecContext* _php_get_decoder_context(ff_movie_context *ffmovie_ctx,
        int stream_type)
{
    AVCodec *decoder;
    int stream_index;

    stream_index = _php_get_stream_index(ffmovie_ctx->fmt_ctx, stream_type);
    if (stream_index < 0) {
        // FIXME: factor out the conditional.
        if (stream_type == CODEC_TYPE_VIDEO) {
            zend_error(E_WARNING, "Can't find video stream in %s",
                    _php_get_filename(ffmovie_ctx));
            return NULL;
        } else {
            zend_error(E_WARNING, "Can't find audio stream in %s",
                    _php_get_filename(ffmovie_ctx));
            return NULL;
        }
    }

    if (stream_index >= ffmovie_ctx->ncodec) {
        void *p = erealloc(ffmovie_ctx->codec_ctx,
            (sizeof(*ffmovie_ctx->codec_ctx)) * (stream_index + 1));

        if (p == NULL) {
            zend_error(E_ERROR, "Out of memory trying to allocate for codec %d",
                stream_index);
            return NULL;
        }

        ffmovie_ctx->codec_ctx = p;

        /* Zero-out the newly-allocated pointers */
        do {
            ffmovie_ctx->codec_ctx[ffmovie_ctx->ncodec] = NULL;
        } while (++ffmovie_ctx->ncodec <= stream_index);
    }

    /* check if the codec for this stream is already open */
    if (!ffmovie_ctx->codec_ctx[stream_index]) {

        /* find the decoder */
        decoder = avcodec_find_decoder(GET_CODEC_FIELD(
                    ffmovie_ctx->fmt_ctx->streams[stream_index]->codec,
                    codec_id));

        if (!decoder) {
            zend_error(E_ERROR, "Could not find decoder for %s",
                    _php_get_filename(ffmovie_ctx));
            return NULL;
        }

        ffmovie_ctx->codec_ctx[stream_index] =
            GET_CODEC_PTR(ffmovie_ctx->fmt_ctx->streams[stream_index]->codec);

       /* open the decoder */
        if (avcodec_open2(ffmovie_ctx->codec_ctx[stream_index], decoder, NULL) < 0) {
            zend_error(E_WARNING, "Could not open codec for %s", _php_get_filename(ffmovie_ctx));
            return NULL;
        }
    }
    return ffmovie_ctx->codec_ctx[stream_index];
}
/* }}} */


/* {{{ proto string getComment()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getComment)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "comment", NULL, 0);
    if (entry) {
	    RETURN_STRINGL(entry->value, strlen(entry->value));
    } else {
        RETURN_NULL();
    }
#else
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->comment,
            strlen(ffmovie_ctx->fmt_ctx->comment));
#endif
}
/* }}} */


/* {{{ proto string getTitle()
 * Return title field from movie or title ID3 tag from an MP3 file.
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getTitle)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "title", NULL, 0);
    if (entry) {
	    RETURN_STRINGL(entry->value, strlen(entry->value));
    } else {
        RETURN_NULL();
    }
#else
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->title,
            strlen(ffmovie_ctx->fmt_ctx->title));
#endif
}
/* }}} */


/* {{{ proto string getAuthor() or getArtist()
 * Return author field from a movie or artist ID3 tag from am MP3 files.
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAuthor)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

 #if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "author", NULL, 0);
	if (!entry) {
	    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "artist", NULL, 0);
	}

    if (entry) {
	    RETURN_STRINGL(entry->value, strlen(entry->value));
    } else {
        RETURN_NULL();
    }
#else
   RETURN_STRINGL(ffmovie_ctx->fmt_ctx->author,
            strlen(ffmovie_ctx->fmt_ctx->author));
#endif
}
/* }}} */


/* {{{ proto string getCopyright()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getCopyright)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "copyright", NULL, 0);
    if (entry) {
	    RETURN_STRINGL(entry->value, strlen(entry->value));
    } else {
        RETURN_NULL();
    }
#else
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->copyright,
            strlen(ffmovie_ctx->fmt_ctx->copyright));
#endif
}
/* }}} */


/* {{{ proto string getAlbum()
 *  Return ID3 album field from an mp3 file
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAlbum)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "album", NULL, 0);
    if (entry) {
	    RETURN_STRINGL(entry->value, strlen(entry->value));
    } else {
        RETURN_NULL();
    }
#else
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->album,
            strlen(ffmovie_ctx->fmt_ctx->album));
#endif
}
/* }}} */

/* {{{ proto string getGenre()
 *  Return ID3 genre field from an mp3 file
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getGenre)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "genre", NULL, 0);
    if (entry) {
	    RETURN_STRINGL(entry->value, strlen(entry->value));
    } else {
        RETURN_NULL();
    }
#else
    RETURN_STRINGL(ffmovie_ctx->fmt_ctx->genre,
            strlen(ffmovie_ctx->fmt_ctx->genre));
#endif
}
/* }}} */


/* {{{ proto int getTrackNumber()
 *  Return ID3 track field from an mp3 file
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getTrackNumber)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "track", NULL, 0);
    if (entry) {
	    RETURN_LONG(atol(entry->value));
    } else {
	    RETURN_NULL();
    }
#else
    RETURN_LONG(ffmovie_ctx->fmt_ctx->track);
#endif
}
/* }}} */

/* {{{ proto int getYear()
 *  Return ID3 year field from an mp3 file
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getYear)
{
    ff_movie_context *ffmovie_ctx;
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    AVDictionaryEntry *entry;
#endif

    GET_MOVIE_RESOURCE(ffmovie_ctx);

#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "year", NULL, 0);
	if (!entry) {
	    entry = av_dict_get(ffmovie_ctx->fmt_ctx->metadata, "date", NULL, 0);
	}

    if (entry && (strlen(entry->value) <= 4)) {
	    RETURN_LONG(atol(entry->value));
    } else {
	    RETURN_NULL();
    }
#else
    RETURN_LONG(ffmovie_ctx->fmt_ctx->year);
#endif
	
}
/* }}} */


/* {{{ _php_get_duration()
 */
static float _php_get_duration(ff_movie_context *ffmovie_ctx)
{
    float duration;

    duration = (float)ffmovie_ctx->fmt_ctx->duration / AV_TIME_BASE;

    if (duration < 0) {
        duration = 0.0f;
    }
    return duration;
}
/* }}} */


/* {{{ proto int getDuration()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getDuration)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_DOUBLE(_php_get_duration(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framerate()
 */
static float _php_get_framerate(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);
    float rate = 0.0f;

    if (!st) {
      return rate;
    }

#if LIBAVCODEC_BUILD > 4753
    if (GET_CODEC_FIELD(st->codec, codec_type) == CODEC_TYPE_VIDEO){
        if (st->r_frame_rate.den && st->r_frame_rate.num) {
            rate = av_q2d(st->r_frame_rate);
        } else {
            rate = 1 / av_q2d(GET_CODEC_FIELD(st->codec, time_base));
        }
    }
    return (float)rate;
#else
    return (float)GET_CODEC_FIELD(st->codec, frame_rate) /
                        GET_CODEC_FIELD(st->codec, frame_rate_base);
#endif
}
/* }}} */


/* {{{ _php_get_framecount()
 */
static long _php_get_framecount(ff_movie_context *ffmovie_ctx)
{
    /* does this movie have a video stream?  */
    if (!_php_get_video_stream(ffmovie_ctx->fmt_ctx)) {
      return 0;
    }

    return LRINT(_php_get_framerate(ffmovie_ctx) *
            _php_get_duration(ffmovie_ctx));
}
/* }}} */


/* {{{ proto int getFrameCount()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameCount)
{
    ff_movie_context *ffmovie_ctx;
    GET_MOVIE_RESOURCE(ffmovie_ctx);
    RETURN_LONG(_php_get_framecount(ffmovie_ctx));
}
/* }}} */


/* {{{ proto int getFrameRate()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameRate)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_DOUBLE(_php_get_framerate(ffmovie_ctx));
}
/* }}} */


/* {{{ proto string getFileName()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFileName)
{
    ff_movie_context *ffmovie_ctx;
    const char* filename;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    filename = _php_get_filename(ffmovie_ctx);
    RETURN_STRINGL(filename, strlen(filename));
}
/* }}} */


/* {{{ _php_get_framewidth()
 */
static int _php_get_framewidth(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0;
    }

    return GET_CODEC_FIELD(st->codec, width);
}
/* }}} */


/* {{{ proto int getFrameWidth()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameWidth)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_framewidth(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_frameheight()
 */
static int _php_get_frameheight(ff_movie_context *ffmovie_ctx)
{
    AVStream *st = _php_get_video_stream(ffmovie_ctx->fmt_ctx);

    if (!st) {
      return 0;
    }

    return GET_CODEC_FIELD(st->codec, height);
}
/* }}} */


/* {{{ proto int getFrameHeight()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameHeight)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_frameheight(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_framenumber()
 */
static long _php_get_framenumber(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx = NULL;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    if (!decoder_ctx) {
        return 0;
    }

    if (ffmovie_ctx->frame_number <= 0) {
        return 1; /* no frames read yet so return the first */
    } else {
        return ffmovie_ctx->frame_number;
    }
}
/* }}} */


/* {{{ proto resource getFrameNumber()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrameNumber)
{
    ff_movie_context *ffmovie_ctx;
    int frame_number = 0;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    frame_number =_php_get_framenumber(ffmovie_ctx);

    if (frame_number) {
        RETURN_LONG(frame_number);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_pixelformat()
 */
static int _php_get_pixelformat(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);

    return decoder_ctx ? decoder_ctx->pix_fmt : 0;
}
/* }}} */


/* {{{ proto int getPixelFormat()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getPixelFormat)
{
    int pix_fmt;
    const char *fmt;
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    pix_fmt = _php_get_pixelformat(ffmovie_ctx);
    fmt = av_get_pix_fmt_name(pix_fmt);

    if (fmt) {
        /* cast const to non-const to keep compiler from complaining,
           RETURN_STRINGL just copies so the string won't get overwritten
           */
        RETURN_STRINGL((char *)fmt, strlen(fmt));
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_bitrate()
 */
static int _php_get_bitrate(ff_movie_context *ffmovie_ctx)
{
    return ffmovie_ctx->fmt_ctx->bit_rate;
}
/* }}} */


/* {{{ proto int getBitrate()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getBitRate)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_LONG(_php_get_bitrate(ffmovie_ctx));
}
/* }}} */


/* {{{ proto int hasAudio()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, hasAudio)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_BOOL(has_audio(ffmovie_ctx));
}
/* }}} */


/* {{{ proto int hasVideo()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, hasVideo)
{
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    RETURN_BOOL(has_video(ffmovie_ctx));
}
/* }}} */


/* {{{ _php_get_codec_name()
   Returns the name of a video or audio codec from a movie
 */
static const char* _php_get_codec_name(ff_movie_context *ffmovie_ctx, int type)
{
    AVCodecContext *decoder_ctx = NULL;
    AVCodec *p = NULL;
    const char *codec_name;
    char buf1[32];

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, type);
    if (!decoder_ctx) {
        return NULL;
    }

    p = avcodec_find_decoder(decoder_ctx->codec_id);

    /* Copied from libavcodec/utils.c::avcodec_string */
    if (p) {
        codec_name = p->name;
#ifdef FF_API_SUB_ID
        if (decoder_ctx->codec_id == CODEC_ID_MP3) {
            if (decoder_ctx->sub_id == 2)
                codec_name = "mp2";
            else if (decoder_ctx->sub_id == 1)
                codec_name = "mp1";
        }
#endif
    } else if (decoder_ctx->codec_id == AV_CODEC_ID_MPEG2TS) {
        /* fake mpeg2 transport stream codec (currently not registered) */
        codec_name = "mpeg2ts";
    } else if (decoder_ctx->codec_name[0] != '\0') {
        codec_name = decoder_ctx->codec_name;
    } else {
        /* output avi tags */
        if (decoder_ctx->codec_type == CODEC_TYPE_VIDEO) {
            snprintf(buf1, sizeof(buf1), "%c%c%c%c",
                    decoder_ctx->codec_tag & 0xff,
                    (decoder_ctx->codec_tag >> 8) & 0xff,
                    (decoder_ctx->codec_tag >> 16) & 0xff,
                    (decoder_ctx->codec_tag >> 24) & 0xff);
        } else {
            snprintf(buf1, sizeof(buf1), "0x%04x", decoder_ctx->codec_tag);
        }
        codec_name = buf1;
    }

    return codec_name;
}
/* }}} */


/* {{{ proto int getVideoCodec()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getVideoCodec)
{
    ff_movie_context *ffmovie_ctx;
    char *codec_name;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    codec_name = (char*)_php_get_codec_name(ffmovie_ctx, CODEC_TYPE_VIDEO);

    if (codec_name) {
        RETURN_STRINGL(codec_name, strlen(codec_name));
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getAudioCodec()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioCodec)
{
    ff_movie_context *ffmovie_ctx;
    char *codec_name;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    codec_name = (char*)_php_get_codec_name(ffmovie_ctx, CODEC_TYPE_AUDIO);

    if (codec_name) {
        RETURN_STRINGL(codec_name, strlen(codec_name));
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getVideoStreamId()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getVideoStreamId )
{
    int stream_id;
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    stream_id= _php_get_stream_index(ffmovie_ctx->fmt_ctx, CODEC_TYPE_VIDEO);

    if( stream_id == -1 )
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_LONG(stream_id);
    }
}
/* }}} */

/* {{{ proto int getAudioStreamId()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioStreamId )
{
    int stream_id;
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    stream_id= _php_get_stream_index(ffmovie_ctx->fmt_ctx, CODEC_TYPE_AUDIO);

    if( stream_id == -1 )
    {
        RETURN_NULL();
    }
    else
    {
        RETURN_LONG(stream_id);
    }
}
/* }}} */

/* {{{ _php_get_audio_channels()
 */
static int _php_get_codec_channels(ff_movie_context *ffmovie_ctx, int type)
{
    AVCodecContext *decoder_ctx = NULL;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, type);
    if (!decoder_ctx) {
        return 0;
    }

    return decoder_ctx->channels;
}
/* }}} */


/* {{{ proto int getAudioChannels()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioChannels)
{
    ff_movie_context *ffmovie_ctx;
    int channels;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    channels = _php_get_codec_channels(ffmovie_ctx, CODEC_TYPE_AUDIO);

    if (channels) {
        RETURN_LONG(channels);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_codec_sample_rate()
 */
static int _php_get_codec_sample_rate(ff_movie_context *ffmovie_ctx, int type)
{
    AVCodecContext *decoder_ctx = NULL;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, type);
    if (!decoder_ctx) {
        return 0;
    }

    return decoder_ctx->sample_rate;
}
/* }}} */


/* {{{ proto int getAudioSampleRate()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioSampleRate)
{
    ff_movie_context *ffmovie_ctx = NULL;
    int sample_rate = 0;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    sample_rate = _php_get_codec_sample_rate(ffmovie_ctx, CODEC_TYPE_AUDIO);

    if (sample_rate) {
        RETURN_LONG(sample_rate);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_get_codec_bit_rate()
   Returns the bit rate for codec of type.
 */
static int _php_get_codec_bit_rate(ff_movie_context *ffmovie_ctx, int type)
{
    AVCodecContext *decoder_ctx = NULL;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, type);
    if (!decoder_ctx) {
        return 0;
    }

    return decoder_ctx->bit_rate;
}


/* {{{ proto int getAudioBitRate()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getAudioBitRate)
{
    ff_movie_context *ffmovie_ctx = NULL;
    int bit_rate = 0;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    bit_rate = _php_get_codec_bit_rate(ffmovie_ctx, CODEC_TYPE_AUDIO);

    if (bit_rate) {
        RETURN_LONG(bit_rate);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto int getVideoBitRate()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getVideoBitRate)
{
    ff_movie_context *ffmovie_ctx = NULL;
    int bit_rate = 0;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    bit_rate = _php_get_codec_bit_rate(ffmovie_ctx, CODEC_TYPE_VIDEO);

    if (bit_rate) {
        RETURN_LONG(bit_rate);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_read_av_frame()
 Returns the next frame from the movie
 */
static AVFrame* _php_read_av_frame(ff_movie_context *ffmovie_ctx,
        AVCodecContext *decoder_ctx, int *is_keyframe, int64_t *pts)
{
    int video_stream;
    AVPacket packet;
    AVFrame *frame = NULL;
    int got_frame;

    video_stream = _php_get_stream_index(ffmovie_ctx->fmt_ctx,
            CODEC_TYPE_VIDEO);
    if (video_stream < 0) {
        return NULL;
    }

    frame = av_frame_alloc();

    /* read next frame */
    while (av_read_frame(ffmovie_ctx->fmt_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream) {

            avcodec_decode_video2(decoder_ctx, frame, &got_frame, &packet);

            if (got_frame) {
                *is_keyframe = (packet.flags & PKT_FLAG_KEY);
                *pts = packet.pts;
                av_free_packet(&packet);
                return frame;
            }
        }

        /* free the packet allocated by av_read_frame */
        av_free_packet(&packet);
    }

    av_free(frame);
    return NULL;
}
/* }}} */

/* {{{ _php_get_av_frame()
   Returns a frame from the movie.
   */
#define GETFRAME_KEYFRAME -1
#define GETFRAME_NEXTFRAME 0
static AVFrame* _php_get_av_frame(ff_movie_context *ffmovie_ctx,
        int wanted_frame, int *is_keyframe, int64_t *pts)
{
    AVCodecContext *decoder_ctx = NULL;
    AVFrame *frame = NULL;

    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    if (decoder_ctx == NULL) {
        return NULL;
    }

    /* Rewind to the beginning of the stream if wanted frame already passed */
    if (wanted_frame > 0 && wanted_frame <= ffmovie_ctx->frame_number) {
        if (

#if LIBAVFORMAT_BUILD >=  4619
                av_seek_frame(ffmovie_ctx->fmt_ctx, -1, 0, 0)
#else
                av_seek_frame(ffmovie_ctx->fmt_ctx, -1, 0)
#endif
                < 0) {
            //zend_error(E_ERROR,"Error seeking to beginning of video stream");
            // If we can't seek, fall back to reopening the file.
            // NOTE: This may mask locking problems in persistent movies.
            _php_open_movie_file(ffmovie_ctx, _php_get_filename(ffmovie_ctx));
        }

        /* flush decoder buffers here */
        avcodec_flush_buffers(decoder_ctx);

        ffmovie_ctx->frame_number = 0;
    }

    /* read frames looking for wanted_frame */
    while (1) {
        frame = _php_read_av_frame(ffmovie_ctx, decoder_ctx, is_keyframe, pts);

        /* hurry up if we're still a ways from the target frame */
        if (wanted_frame != GETFRAME_KEYFRAME &&
                wanted_frame != GETFRAME_NEXTFRAME &&
                wanted_frame - ffmovie_ctx->frame_number >
                decoder_ctx->gop_size + 1) {
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
            decoder_ctx->skip_frame = AVDISCARD_NONREF;
#else
            decoder_ctx->hurry_up = 1;
#endif
        } else {
#if LIBAVFORMAT_VERSION_INT > AV_VERSION_INT(52, 31, 0)
            decoder_ctx->skip_frame = AVDISCARD_DEFAULT;
#else
            decoder_ctx->hurry_up = 0;
#endif
        }
        ffmovie_ctx->frame_number++;

        /*
         * if caller wants next keyframe then get it and break out of loop.
         */
        if (wanted_frame == GETFRAME_KEYFRAME && is_keyframe) {
            return frame;
        }

        if (wanted_frame == GETFRAME_NEXTFRAME ||
                ffmovie_ctx->frame_number == wanted_frame) {
            return frame;
        }
    }

    av_free(frame);
    return NULL;
}
/* }}} */


/* {{{ _php_get_ff_frame()
   puts a ff_frame object into the php return_value variable
   returns 1 on success, 0 on failure.
 */
static int _php_get_ff_frame(ff_movie_context *ffmovie_ctx,
        int wanted_frame, INTERNAL_FUNCTION_PARAMETERS) {
    int is_keyframe = 0;
    int64_t pts;
    AVFrame *frame = NULL;

    ff_frame_context *ff_frame;

    frame = _php_get_av_frame(ffmovie_ctx, wanted_frame, &is_keyframe, &pts);
    if (frame) {
        /*
         * _php_create_ffmpeg_frame sets PHP return_value to a ffmpeg_frame
         * object via INTERNAL_FUNCTION_PARAM_PASSTHRU, the returned ff_frame
         * is just for conveniently setting its fields.
         */
        ff_frame = _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAM_PASSTHRU);

        if (!ff_frame) {
            php_error_docref(NULL, E_ERROR,
                    "Error allocating ffmpeg_frame resource");
        }

        /* TODO: Provide function(s) for setting these in ffmpeg_frame */
        ff_frame->width = _php_get_framewidth(ffmovie_ctx);
        ff_frame->height = _php_get_frameheight(ffmovie_ctx);
        ff_frame->pixel_format = _php_get_pixelformat(ffmovie_ctx);
        ff_frame->keyframe = is_keyframe;
        ff_frame->pts = pts;

        ff_frame->av_frame = av_frame_alloc();
        avpicture_alloc((AVPicture*)ff_frame->av_frame, ff_frame->pixel_format,
            ff_frame->width, ff_frame->height);

        /* FIXME: temporary hack until I figure out how to pass new buffers
         *        to the decoder
         */
        av_picture_copy((AVPicture*)ff_frame->av_frame,
                        (AVPicture*)frame, ff_frame->pixel_format,
                ff_frame->width, ff_frame->height);

        return 1;
    } else {
        return 0;
    }

}
/* }}} */


/* {{{ proto resource getNextKeyFrame()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getNextKeyFrame)
{
    ff_movie_context *ffmovie_ctx;

    if (ZEND_NUM_ARGS()) {
        WRONG_PARAM_COUNT;
    }

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    if (!_php_get_ff_frame(ffmovie_ctx, GETFRAME_KEYFRAME,
                INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto resource getFrame([int frame])
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getFrame)
{
    zval *argv = NULL;
    int wanted_frame = 0;
    ff_movie_context *ffmovie_ctx;
    int ac = ZEND_NUM_ARGS();

    if (ZEND_NUM_ARGS() > 1) {
        WRONG_PARAM_COUNT;
    }

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    if (ZEND_NUM_ARGS()) {
        /* retrieve arguments */
        argv = (zval *)safe_emalloc(sizeof(zval), ac, 0);
        if (zend_get_parameters_array_ex(ac, argv) != SUCCESS) {
            efree(argv);
            WRONG_PARAM_COUNT;
        }

        convert_to_long_ex(&argv[0]);
        wanted_frame = Z_LVAL(argv[0]);

        /* bounds check wanted frame */
        if (wanted_frame < 1) {
            php_error_docref(NULL, E_ERROR,
                    "Frame number must be greater than zero");
        }
    }

    if (! _php_get_ff_frame(ffmovie_ctx, wanted_frame,
                INTERNAL_FUNCTION_PARAM_PASSTHRU)) {
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ _php_pre_read_frame()
 * ffmpeg seems not to fill in some AVCodecContext fields until at least
 * one frame is read. This function will read a frame without moving the
 * frame counter.
 */
void _php_pre_read_frame(ff_movie_context *ffmovie_ctx) {
    AVFrame *frame = NULL;
    int is_keyframe;
    int64_t pts;

    frame = _php_get_av_frame(ffmovie_ctx,
            _php_get_framenumber(ffmovie_ctx) - 1, &is_keyframe, &pts);

    av_free(frame);
}


/* {{{ _php_get_sample_aspec_ratio()
 */
static double _php_get_sample_aspect_ratio(ff_movie_context *ffmovie_ctx)
{
    AVCodecContext *decoder_ctx;


    decoder_ctx = _php_get_decoder_context(ffmovie_ctx, CODEC_TYPE_VIDEO);
    if (!decoder_ctx) {
        return -1;
    }


    if (decoder_ctx->sample_aspect_ratio.num == 0) {
        // pre read a frame so ffmpeg will fill in sample aspect ratio field.
        _php_pre_read_frame(ffmovie_ctx);

        if (decoder_ctx->sample_aspect_ratio.num == 0) {
            return -2; // aspect not set
        }
    }

    return (double)av_q2d(decoder_ctx->sample_aspect_ratio);
}
/* }}} */


/* {{{ proto double getPixelAspectRatio()
 */
FFMPEG_PHP_METHOD(ffmpeg_movie, getPixelAspectRatio)
{
    double aspect;
    ff_movie_context *ffmovie_ctx;

    GET_MOVIE_RESOURCE(ffmovie_ctx);

    aspect = _php_get_sample_aspect_ratio(ffmovie_ctx);

    if (aspect < 0) {
        RETURN_FALSE;
    }

    RETURN_DOUBLE(aspect);
}
/* }}} */


/* {{{ proto array ffmpeg_movie_list()
   List opened ffmpeg_movies */
PHP_FUNCTION(ffmpeg_movie_list)
{
    zend_ulong numitems, i;
    zend_resource *le;
    ff_movie_context *ffmovie_ctx;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    array_init(return_value);

    numitems = zend_hash_next_free_element(&EG(regular_list));
    for (i=1; i<numitems; i++) {
        if ((le = zend_hash_index_find_ptr(&EG(regular_list), i)) == NULL) {
            continue;
        }
        if (le->type == le_ffmpeg_movie) {
            ffmovie_ctx = (ff_movie_context *)(le->ptr);
            add_index_long(return_value, i, (zend_long)le->ptr);
            add_index_long(return_value, 10*i, (zend_long)le->type);
            add_index_string(return_value, 100*i, "le_ffmpeg_movie");
            add_index_string(return_value, 1000*i, ffmovie_ctx->fmt_ctx->filename);
        }
        if (le->type == le_ffmpeg_pmovie) {
            ffmovie_ctx = (ff_movie_context *)(le->ptr);
            add_index_long(return_value, i, (zend_long)le->ptr);
            add_index_long(return_value, 10*i, (zend_long)le->type);
            add_index_string(return_value, 100*i, "le_ffmpeg_pmovie");
            add_index_string(return_value, 1000*i, ffmovie_ctx->fmt_ctx->filename);
        }
    }
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
