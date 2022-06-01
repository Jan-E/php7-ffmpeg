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

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_ffmpeg.h"

#include "ffmpeg_frame.h"
#include "ffmpeg_tools.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef ZEND_GET_RESOURCE_TYPE_ID
#define ZEND_GET_RESOURCE_TYPE_ID(le_id, le_type_name) \
    if (le_id == 0) {                                  \
        le_id = zend_fetch_list_dtor_id(le_type_name); \
	}
#endif

#if HAVE_LIBGD20

#include <php_gd.h>
#include <gd.h>

#define FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, ret) { \
	ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd"); \
	if ((gd_img = (gdImagePtr)zend_fetch_resource(Z_RES_P(ret), "Image", le_gd)) == NULL) {\
		RETURN_FALSE;\
	}\
}

#if AV_PIX_FMT_RGBA32
#define FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT AV_PIX_FMT_RGBA32
#else
#define FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT AV_PIX_FMT_RGB32
#endif

static int le_gd; // this is only valid after calling
// FFMPEG_PHP_FETCH_IMAGE_RESOURCE()

#endif // HAVE_LIBGD20

int le_ffmpeg_frame; // not static since it is used in ffmpeg_output_movie

static zend_class_entry *ffmpeg_frame_class_entry_ptr;
zend_class_entry ffmpeg_frame_class_entry;

/* {{{ ffmpeg_frame methods[]
   Methods of the ffmpeg_frame class
   */
zend_function_entry ffmpeg_frame_class_methods[] = {

	/* object can't be created from user space so no PHP constructor */
	//PHP_ME(ffmpeg_frame, __construct, NULL, 0)

#if PHP_MAJOR_VERSION >= 8

#if HAVE_LIBGD20
	/* gd methods */
	FFMPEG_PHP_MALIAS(ffmpeg_frame, togdimage,                   toGDImage,                  arginfo_class_ffmpeg_frame_togdimage,                  0)


#endif // HAVE_LIBGD20

	/* methods */
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getwidth,                    getWidth,                   arginfo_class_ffmpeg_frame_getwidth,                   0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getheight,                   getHeight,                  arginfo_class_ffmpeg_frame_getheight,                  0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, resize,                      resize,                     arginfo_class_ffmpeg_frame_resize,                     0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, iskeyframe,                  isKeyFrame,                 arginfo_class_ffmpeg_frame_iskeyframe,                 0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getpresentationtimestamp,    getPresentationTimestamp,   arginfo_class_ffmpeg_frame_getpresentationtimestamp,   0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getpts,                      getPresentationTimestamp,   arginfo_class_ffmpeg_frame_getpts,                     0)

#else

#if HAVE_LIBGD20
	/* gd methods */
	FFMPEG_PHP_MALIAS(ffmpeg_frame, togdimage,      toGDImage,     NULL, 0)


#endif // HAVE_LIBGD20

	/* methods */
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getwidth,                    getWidth,                   NULL, 0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getheight,                   getHeight,                  NULL, 0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, resize,                      resize,                     NULL, 0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, iskeyframe,                  isKeyFrame,                 NULL, 0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getpresentationtimestamp,    getPresentationTimestamp,   NULL, 0)
	FFMPEG_PHP_MALIAS(ffmpeg_frame, getpts,                      getPresentationTimestamp,   NULL, 0)

#endif

	FFMPEG_PHP_END_METHODS
};
/* }}} */


/* {{{ _php_alloc_ff_frame()
*/
static ff_frame_context* _php_alloc_ff_frame()
{
	ff_frame_context *ff_frame = NULL;

	ff_frame = emalloc(sizeof(ff_frame_context));

	if (!ff_frame) {
	    zend_error(E_ERROR, "Error allocating ffmpeg_frame");
	}

	ff_frame->av_frame = NULL;
	ff_frame->width = 0;
	ff_frame->height = 0;
	ff_frame->pixel_format = 0;

	return ff_frame;
}
/* }}} */


/* {{{ proto object _php_create_ffmpeg_frame()
   creates an ffmpeg_frame object, adds a ffmpeg_frame resource to the
   object, registers the resource and returns a direct pointer to the
   resource.
   */
ff_frame_context* _php_create_ffmpeg_frame(INTERNAL_FUNCTION_PARAMETERS)
{
	zend_resource *ret;
	ff_frame_context *ff_frame;

	ff_frame = _php_alloc_ff_frame();

	ret = zend_register_resource(ff_frame, le_ffmpeg_frame);
	object_init_ex(return_value, ffmpeg_frame_class_entry_ptr);
	add_property_resource(return_value, "ffmpeg_frame", ret);

	return ff_frame;
}
/* }}} */


/* {{{ _php_free_av_frame()
*/
static void _php_free_av_frame(AVFrame *av_frame)
{
	if (av_frame) {
	    if (av_frame->data[0]) {
	        av_free(av_frame->data[0]);
	        av_frame->data[0] = NULL;
	    }
	    av_free(av_frame);
	}
}
/* }}} */


/* {{{ _php_free_ffmpeg_frame()
*/
static void _php_free_ffmpeg_frame(zend_resource *rsrc)
{
	ff_frame_context *ff_frame = (ff_frame_context*)rsrc->ptr;
	_php_free_av_frame(ff_frame->av_frame);
	efree(ff_frame);
}
/* }}} */


/* {{{ register_ffmpeg_frame_class()
*/
void register_ffmpeg_frame_class(int module_number)
{
	le_ffmpeg_frame = zend_register_list_destructors_ex(_php_free_ffmpeg_frame,
	        NULL, "ffmpeg_frame", module_number);

	INIT_CLASS_ENTRY(ffmpeg_frame_class_entry, "ffmpeg_frame",
	        ffmpeg_frame_class_methods);

	/* register ffmpeg frame class */
	ffmpeg_frame_class_entry_ptr =
	    zend_register_internal_class(&ffmpeg_frame_class_entry);
}
/* }}} */


/* {{{ _php_convert_frame()
*/
int _php_convert_frame(ff_frame_context *ff_frame, int dst_fmt) {
	AVFrame *dst_frame;
	int result = 0;

	if (!ff_frame->av_frame) {
	    return -1;
	}

	if (dst_fmt == ff_frame->pixel_format) {
	    return 0; // NOP
	}

	dst_frame = av_frame_alloc();
	avpicture_alloc((AVPicture*)dst_frame, dst_fmt, ff_frame->width,
	        ff_frame->height);

	result = img_convert( (AVPicture *)dst_frame, dst_fmt,
	        (AVPicture *) ff_frame->av_frame, ff_frame->pixel_format, ff_frame->width, ff_frame->height);

	if (result) {
	    zend_error(E_ERROR, "Error converting frame");
	    _php_free_av_frame(dst_frame);
	} else {
	    ff_frame->av_frame = dst_frame;
	    ff_frame->pixel_format = dst_fmt;
	}

	return result;
}
/* }}} */

#if HAVE_LIBGD20

/* {{{ _php_get_gd_image()
*/
static void
_php_get_gd_image(zval *retval, int ww, int hh)
{
	zval gd_function_name;
	zval gd_argv[2]; /* borrowed from php_pcre.c */
	zend_function *gd_func;

	array_init_size(&gd_argv[0], 2);
	ZVAL_LONG(&gd_argv[0], (zend_long)ww);
	ZVAL_LONG(&gd_argv[1], (zend_long)hh);
	ZVAL_STRING(&gd_function_name, "imagecreatetruecolor");

	if ((gd_func = Z_PTR_P(&gd_function_name)) == NULL) {
	    zend_error(E_ERROR, "Error can't find %s function", "imagecreatetruecolor");
	}

#if PHP_MAJOR_VERSION < 8
	if (call_user_function_ex(EG(function_table), NULL, &gd_function_name,
			retval, 2, gd_argv, 0, NULL) == SUCCESS && Z_TYPE(*retval) != IS_UNDEF) {
#else
	if (call_user_function(EG(function_table), NULL, &gd_function_name,
			retval, 2, gd_argv) == SUCCESS && Z_TYPE(*retval) != IS_UNDEF) {
#endif
				/* hooray */
	} else {
	    zend_error(E_ERROR, "Error calling %s function", "imagecreatetruecolor");
	}

#if PHP_MAJOR_VERSION < 8
	if (Z_TYPE(*retval) != IS_RESOURCE) {
	    php_error_docref(NULL, E_ERROR,
	            "Error creating GD Image, Z_TYPE(*retval) = %d: %s ( %d, %d ). Note: 8 = IS_OBJECT, see zend_types.h", Z_TYPE(*retval), "imagecreatetruecolor", ww, hh );
	}
#else
	if (Z_TYPE(*retval) != IS_OBJECT) {
	    php_error_docref(NULL, E_ERROR,
	            "Error creating GD Image, Z_TYPE(*retval) = %d: %s ( %d, %d ). Note: 8 = IS_OBJECT, see zend_types.h", Z_TYPE(*retval), "imagecreatetruecolor", ww, hh );
	}
#endif

	Z_ADDREF_P(retval);
	zval_ptr_dtor(retval);
}
/* }}} */


/* {{{ _php_avframe_to_gd_image()
*/
static int _php_avframe_to_gd_image(AVFrame *frame, gdImagePtr dest, int width,
	    int height)
{
	int x, y;
	int *src = (int*)frame->data[0];

	// Borrowed from https://github.com/tony2001/ffmpeg-php/commit/23174b4
	if (width > dest->sx || height > dest->sy) {
		php_error_docref(NULL, E_ERROR,
	            "width (%d) > gdImage->sx (%d) || height (%d) > gdImage->sy (%d) ", width, dest->sx, height, dest->sy );
		return -1;
	}

	for (y = 0; y < height; y++) {
	    for (x = 0; x < width; x++) {
			dest->tpixels[y][x] = src[x] & 0x00ffffff;
	    }
	    src += width;
	}
	return 0;
}
/* }}} */


/* {{{ _php_gd_image_to_avframe()
*/
static int _php_gd_image_to_avframe(gdImage *src, AVFrame *frame, int width,
	    int height)
{
	int x, y;
	int *dest = (int*)frame->data[0];

	for (y = 0; y < height; y++) {
	    for (x = 0; x < width; x++) {
	        dest[x] = src->tpixels[y][x];
	    }
	    dest += width;
	}
	return 0;
}
/* }}} */

/* {{{ proto resource toGDImage()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, toGDImage)
{
	ff_frame_context *ff_frame;
	gdImagePtr gd_img;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	_php_convert_frame(ff_frame, FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT);

	_php_get_gd_image(return_value, ff_frame->width, ff_frame->height);

#if PHP_MAJOR_VERSION < 8
	FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, return_value);
#else
	gd_img = php_gd_libgdimageptr_from_zval_p(return_value);
	//php_error_docref(NULL, E_NOTICE, "gdImage->sx = %d, gdImage->sy = %d", gd_img->sx, gd_img->sy );
#endif

	if (_php_avframe_to_gd_image(ff_frame->av_frame, gd_img,
	            ff_frame->width, ff_frame->height)) {
	    zend_error(E_ERROR, "failed to convert frame to gd image");
	}
}
/* }}} */


/* {{{ proto object ffmpeg_frame(mixed)
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, ffmpeg_frame)
{
	zval *argv = NULL;
	AVFrame *frame;
	gdImage *gd_img;
	ff_frame_context *ff_frame;
	int width, height;
	zend_resource *ret;

	if (ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}

	/* retrieve argument */
	argv = (zval *)safe_emalloc(sizeof(zval), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
	    php_error_docref(NULL, E_ERROR,
	            "Error parsing arguments");
	}

	ff_frame = _php_alloc_ff_frame();

	ret = zend_register_resource(ff_frame, le_ffmpeg_frame);
	add_property_resource(getThis(), "ffmpeg_frame", ret);

//	switch (Z_TYPE_PP(argv[0])) {
//	    case IS_STRING:
//	        convert_to_string_ex(argv[0]);
//	        zend_error(E_ERROR,
//	                "Creating an ffmpeg_frame from a file is not implemented\n");
//	        //_php_read_frame_from_file(ff_frame, Z_STRVAL_PP(argv[0]));
//	        break;
//	    case IS_RESOURCE:
	        FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &argv[0]);
	        if (!gd_img->trueColor) {
	            php_error_docref(NULL, E_ERROR,
	                    "First parameter must be a truecolor gd image.");
	        }

	        width = gdImageSX(gd_img);
	        height = gdImageSY(gd_img);

	        /* create a an av_frame and allocate space for it */
	        frame = av_frame_alloc();
	        avpicture_alloc((AVPicture*)frame, FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT, width, height);

	        /* copy the gd image to the av_frame */
	        _php_gd_image_to_avframe(gd_img, frame, width, height);

	        /* set the ffmepg_frame to point to this av_frame */
	        ff_frame->av_frame = frame;

	        /* set the ffpmeg_frame's properties */
	        ff_frame->width = width;
	        ff_frame->height = height;
	        ff_frame->pixel_format = FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT;
//	        break;
//	    default:
//	        zend_error(E_ERROR, "Invalid argument\n");
//	}
}
/* }}} */

#endif /* HAVE_LIBGD20 */


/* {{{ proto int getPresentationTimestamp()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, getPresentationTimestamp)
{
	ff_frame_context *ff_frame;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	RETURN_DOUBLE((double)ff_frame->pts / AV_TIME_BASE);
}
/* }}} */


/* {{{ proto int isKeyFrame()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, isKeyFrame)
{
	ff_frame_context *ff_frame;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	RETURN_LONG(ff_frame->keyframe);
}
/* }}} */


/* {{{ proto int getWidth()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, getWidth)
{
	ff_frame_context *ff_frame;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	RETURN_LONG(ff_frame->width);
}
/* }}} */


/* {{{ proto int getHeight()
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, getHeight)
{
	ff_frame_context *ff_frame;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	RETURN_LONG(ff_frame->height);
}
/* }}} */


/* {{{ _php_resample_frame()
*/
int _php_resample_frame(ff_frame_context *ff_frame,
	    int wanted_width, int wanted_height, int crop_top, int crop_bottom,
	    int crop_left, int crop_right)
{
	AVFrame *resampled_frame;
	ImgReSampleContext *img_resample_ctx = NULL;

	if (!ff_frame->av_frame) {
	    return -1;
	}

	/*
	 * do nothing if width and height are the same as the frame and no
	 * cropping was specified
	 * */
	if (wanted_width == ff_frame->width &&
	        wanted_height == ff_frame->height &&
	        (!crop_left && !crop_right && !crop_top && !crop_bottom)) {
	    return 0;
	}

	/* convert to AV_PIX_FMT_YUV420P required for resampling */
	_php_convert_frame(ff_frame, AV_PIX_FMT_YUV420P);

	img_resample_ctx = img_resample_full_init(
	        wanted_width, wanted_height,
	        ff_frame->width, ff_frame->height,
	        crop_top, crop_bottom, crop_left, crop_right,
	        0, 0, 0, 0);
	if (!img_resample_ctx) {
	    return -1;
	}

	resampled_frame = av_frame_alloc();
	avpicture_alloc((AVPicture*)resampled_frame, AV_PIX_FMT_YUV420P,
	        wanted_width, wanted_height);

	img_resample(img_resample_ctx, (AVPicture*)resampled_frame,
	        (AVPicture*)ff_frame->av_frame);

	_php_free_av_frame(ff_frame->av_frame);

	img_resample_close(img_resample_ctx);

	ff_frame->av_frame = resampled_frame;
	ff_frame->width = wanted_width;
	ff_frame->height = wanted_height;

	return 0;
}
/* }}} */


/* {{{ proto boolean resize(int width, int height [, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
FFMPEG_PHP_METHOD(ffmpeg_frame, resize)
{
	zval *argv;
	ff_frame_context *ff_frame = NULL;
	int wanted_width = 0, wanted_height = 0;
	int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	/* retrieve arguments */
	argv = (zval *)safe_emalloc(sizeof(zval), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
	    efree(argv);
	    php_error_docref(NULL, E_ERROR,
	            "Error parsing arguments");
	}

	switch (ZEND_NUM_ARGS()) {
	    case 6:
	        convert_to_long_ex(&argv[5]);
	        crop_right = Z_LVAL(argv[5]);

	        /* crop right must be even number for lavc cropping */
	        if (crop_right % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop right must be an even number");
	        }
	        /* fallthru */
	    case 5:
	        convert_to_long_ex(&argv[4]);
	        crop_left = Z_LVAL(argv[4]);

	        /*  crop left must be even number for lavc cropping */
	        if (crop_left % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop left must be an even number");
	        }

	        /* fallthru */
	    case 4:
	        convert_to_long_ex(&argv[3]);
	        crop_bottom = Z_LVAL(argv[3]);

	        /*  crop bottom must be even number for lavc cropping */
	        if (crop_bottom % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop bottom must be an even number");
	        }

	        /* fallthru */
	    case 3:
	        convert_to_long_ex(&argv[2]);
	        crop_top = Z_LVAL(argv[2]);

	        /*  crop top must be even number for lavc cropping */
	        if (crop_top % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop top must be an even number");
	        }

	        /* fallthru */
	    case 2:
	        /* height arg */
	        convert_to_long_ex(&argv[1]);
	        wanted_height = Z_LVAL(argv[1]);

	        /* bounds check wanted height */
	        if (wanted_height < 1) {
	            php_error_docref(NULL, E_ERROR,
	                    "Frame height must be greater than zero");
	        }

	        /* wanted height must be even number for lavc resample */
	        if (wanted_height % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Frame height must be an even number");
	        }
	        /* fallthru */
	    case 1:
	        /* width arg */
	        convert_to_long_ex(&argv[0]);
	        wanted_width = Z_LVAL(argv[0]);

	        /* bounds check wanted width */
	        if (wanted_width < 1) {
	            php_error_docref(NULL, E_ERROR,
	                    "Frame width must be greater than zero");
	        }

	        /* wanted width must be even number for lavc resample */
	        if (wanted_width % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Frame width must be an even number");
	        }
	        break;
	    default:
	        WRONG_PARAM_COUNT;
	}

	efree(argv);

	/* resize frame */
	_php_resample_frame(ff_frame, wanted_width, wanted_height,
	        crop_top, crop_bottom, crop_left, crop_right);

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto boolean crop([, int crop_top [, int crop_bottom [, int crop_left [, int crop_right ]]]])
*/
PHP_FUNCTION(crop)
{
	zval *argv;
	ff_frame_context *ff_frame;
	int crop_top = 0, crop_bottom = 0, crop_left = 0, crop_right = 0;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	/* retrieve arguments */
	argv = (zval *) safe_emalloc(sizeof(zval), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
	    efree(argv);
	    php_error_docref(NULL, E_ERROR,
	            "Error parsing arguments");
	}

	switch (ZEND_NUM_ARGS()) {
	    case 4:
	        convert_to_long_ex(&argv[3]);
	        crop_right = Z_LVAL(argv[3]);

	        /* crop right must be even number for lavc cropping */
	        if (crop_right % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop right must be an even number");
	        }
	        /* fallthru */
	    case 3:
	        convert_to_long_ex(&argv[2]);
	        crop_left = Z_LVAL(argv[2]);

	        /*  crop left must be even number for lavc cropping */
	        if (crop_left % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop left must be an even number");
	        }

	        /* fallthru */
	    case 2:
	        convert_to_long_ex(&argv[1]);
	        crop_bottom = Z_LVAL(argv[1]);

	        /*  crop bottom must be even number for lavc cropping */
	        if (crop_bottom % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop bottom must be an even number");
	        }

	        /* fallthru */
	    case 1:
	        convert_to_long_ex(&argv[0]);
	        crop_top = Z_LVAL(argv[0]);

	        /*  crop top  must be even number for lavc cropping */
	        if (crop_top % 2) {
	            php_error_docref(NULL, E_ERROR,
	                    "Crop top must be an even number");
	        }
	        break;
	    default:
	        WRONG_PARAM_COUNT;
	}

	efree(argv);

	/* resample with same dimensions */
	_php_resample_frame(ff_frame, ff_frame->width, ff_frame->height,
	        crop_top, crop_bottom, crop_left, crop_right);

	RETURN_TRUE;
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
