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

/* 
   include gd header from local include dir. This is a copy of gd.h that is 
   distributed with php-5.2.5. It is distributed along with ffmpeg-php to
   allow ffmpeg-php to be built without access to the php sources
   */
#if HAVE_LIBGD20
#include "gd.h" 

#if PHP_VERSION_ID >= 70000
#define FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, ret) { \
	ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd"); \
	if ((gd_img = (gdImagePtr)zend_fetch_resource(Z_RES_P(ret), "Image", le_gd)) == NULL) {\
		RETURN_FALSE;\
	}\
}
#else
#define FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, ret) { \
	ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd"); \
	ZEND_FETCH_RESOURCE(gd_img, gdImagePtr, ret, -1, "Image", le_gd); \
}
#endif

#if PIX_FMT_RGBA32
#define FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT PIX_FMT_RGBA32
#else
#define FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT PIX_FMT_RGB32
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

#if PHP_VERSION_ID >= 70000
	ret = zend_register_resource(ff_frame, le_ffmpeg_frame);
	object_init_ex(return_value, ffmpeg_frame_class_entry_ptr);
	add_property_resource(return_value, "ffmpeg_frame", ret);
#else
	ret = ZEND_REGISTER_RESOURCE(NULL, ff_frame, le_ffmpeg_frame);
	object_init_ex(return_value, ffmpeg_frame_class_entry_ptr);
	add_property_resource(return_value, "ffmpeg_frame", ret);
#endif

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
#if PHP_VERSION_ID >= 70000
static void _php_free_ffmpeg_frame(zend_resource *rsrc TSRMLS_DC)
#else
static void _php_free_ffmpeg_frame(zend_rsrc_list_entry *rsrc TSRMLS_DC)
#endif
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
	TSRMLS_FETCH();

	le_ffmpeg_frame = zend_register_list_destructors_ex(_php_free_ffmpeg_frame,
	        NULL, "ffmpeg_frame", module_number);

	INIT_CLASS_ENTRY(ffmpeg_frame_class_entry, "ffmpeg_frame", 
	        ffmpeg_frame_class_methods);

	/* register ffmpeg frame class */
	ffmpeg_frame_class_entry_ptr = 
	    zend_register_internal_class(&ffmpeg_frame_class_entry TSRMLS_CC);
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

	dst_frame = avcodec_alloc_frame();
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
static int _php_get_gd_image(int ww, int hh)
{
	zval gd_function_name;
	zval gd_argv[2]; /* borrowed from php_pcre.c */
	zend_function *gd_func;
	zval retval;
	zend_long ret;
	TSRMLS_FETCH();

	array_init_size(&gd_argv[0], 2);
	ZVAL_LONG(&gd_argv[0], (zend_long)ww);
	ZVAL_LONG(&gd_argv[1], (zend_long)hh);
	ZVAL_STRING(&gd_function_name, "imagecreatetruecolor");

	if ((gd_func = Z_PTR_P(&gd_function_name)) == NULL) {
	    zend_error(E_ERROR, "Error can't find %s function", "imagecreatetruecolor");
	}

	if (call_user_function_ex(EG(function_table), NULL, &gd_function_name, 
			&retval, 2, gd_argv, 0, NULL) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
				/* hooray */
	} else {
	    zend_error(E_ERROR, "Error calling %s function", "imagecreatetruecolor");
	}

	if (Z_TYPE(retval) != IS_RESOURCE) {
	    php_error_docref(NULL TSRMLS_CC, E_ERROR,
	            "Error creating GD Image");
	}

	ret = retval.value.lval;
	// _zend_list_addref(ret);
	Z_ADDREF_P(&retval);
	if (&retval) {
		zval_ptr_dtor(&retval);
	}

	return ret;
}
/* }}} */

// Borrowed from ffmpeg-r10461-include
#define gdImageBoundsSafeMacro(im, x, y) (!((((y) < (im)->cy1) || ((y) > (im)->cy2)) || (((x) < (im)->cx1) || ((x) > (im)->cx2))))

/* {{{ _php_avframe_to_gd_image()
*/
static int _php_avframe_to_gd_image(AVFrame *frame, gdImage *dest, int width, 
	    int height)
{
	int x, y;
	int *src = (int*)frame->data[0];

	for (y = 0; y < height; y++) {
	    for (x = 0; x < width; x++) {
			if (gdImageBoundsSafeMacro(dest, x, y)) {
                /* copy pixel to gdimage buffer zeroing the alpha channel */
                dest->tpixels[y][x] = src[x] & 0x00ffffff;
            } else {
#ifndef _WIN64 // [
				__asm int 3; /* breakpoint */
#endif  // _WIN64 ]
                return -1;
            }
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
	gdImage *gd_img;

	GET_FRAME_RESOURCE(getThis(), ff_frame);

	_php_convert_frame(ff_frame, FFMPEG_PHP_FFMPEG_RGB_PIX_FORMAT);

	//fprintf(stderr, "before toGDImage width = %d, height = %d, le->ptr = %d\n", ff_frame->width, ff_frame->height, le->ptr);
	return_value->value.lval = _php_get_gd_image(ff_frame->width, ff_frame->height);
	//fprintf(stderr, "after  toGDImage return_value->value.lval = %ld, Z_PTR_P(return_value) = %ld\n", return_value->value.lval, Z_PTR_P(return_value));

#if PHP_VERSION_ID < 70000
	return_value->type = IS_RESOURCE;
#endif

	FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, return_value);

	if (_php_avframe_to_gd_image(ff_frame->av_frame, gd_img,
	            ff_frame->width, ff_frame->height)) {
	    zend_error(E_ERROR, "failed to convert frame to gd image");
	}
	RETURN_RES(Z_PTR_P(return_value));

	if (0) {
		zend_ulong numitems, i;
		zend_resource *le;
		numitems = zend_hash_next_free_element(&EG(regular_list));
		array_init(return_value);
		for (i=1; i<numitems; i++) {
			if ((le = zend_hash_index_find_ptr(&EG(regular_list), i)) == NULL) {
				continue;
			}
			add_index_long(return_value, i, (zend_long)le->ptr);
			if (le->type == le_ffmpeg_frame) {
				add_index_string(return_value, 10*i, "le_ffmpeg_frame");
			} else if (le->type == 23) {
				add_index_string(return_value, 10*i, "le->type == 23");
			} else if (le->type == 26) {
				add_index_string(return_value, 10*i, "le->type == 26");
				fprintf(stderr, "after  toGDImage le->ptr = %ld\n", (long)le->ptr);
				gd_img = (gdImage *)(le->ptr);
				add_index_long(return_value, 100*i, (zend_long)gd_img->red);
				add_index_long(return_value, 1000*i, (zend_long)gd_img->green);
				add_index_long(return_value, 10000*i, (zend_long)gd_img->blue);
				add_index_long(return_value, 100000*i, (zend_long)gd_img->open);
				//RETURN_RES(le);
				//break;
			} else {
				add_index_long(return_value, 10*i, (zend_long)le->type);
			}
		}
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
	int width, height; //, ret;
	zend_resource *ret;

	if (ZEND_NUM_ARGS() != 1) {
	    WRONG_PARAM_COUNT;
	}

	/* retrieve argument */
	argv = (zval *)safe_emalloc(sizeof(zval), ZEND_NUM_ARGS(), 0);
	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) != SUCCESS) {
	    php_error_docref(NULL TSRMLS_CC, E_ERROR,
	            "Error parsing arguments");
	}

	ff_frame = _php_alloc_ff_frame();

#if PHP_VERSION_ID >= 70000
	ret = zend_register_resource(ff_frame, le_ffmpeg_frame);
	add_property_resource(getThis(), "ffmpeg_frame", ret);
#else
	ret = ZEND_REGISTER_RESOURCE(NULL, ff_frame, le_ffmpeg_frame);
	object_init_ex(getThis(), ffmpeg_frame_class_entry_ptr);
	add_property_resource(getThis(), "ffmpeg_frame", ret);
#endif

//	switch (Z_TYPE_PP(argv[0])) {
//	    case IS_STRING:
//	        convert_to_string_ex(argv[0]);
//	        zend_error(E_ERROR, 
//	                "Creating an ffmpeg_frame from a file is not implemented\n");
//	        //_php_read_frame_from_file(ff_frame, Z_STRVAL_PP(argv[0]));
//	        break;
//	    case IS_RESOURCE:
	        FFMPEG_PHP_FETCH_IMAGE_RESOURCE(gd_img, &argv[0]);
			__asm int 3;
	        if (!gd_img->trueColor) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "First parameter must be a truecolor gd image.");
	        }

	        width = gdImageSX(gd_img);
	        height = gdImageSY(gd_img);

	        /* create a an av_frame and allocate space for it */
	        frame = avcodec_alloc_frame();
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

	/* convert to PIX_FMT_YUV420P required for resampling */
	_php_convert_frame(ff_frame, PIX_FMT_YUV420P);

	img_resample_ctx = img_resample_full_init(
	        wanted_width, wanted_height,
	        ff_frame->width, ff_frame->height,
	        crop_top, crop_bottom, crop_left, crop_right,
	        0, 0, 0, 0);
	if (!img_resample_ctx) {
	    return -1;
	}

	resampled_frame = avcodec_alloc_frame();
	avpicture_alloc((AVPicture*)resampled_frame, PIX_FMT_YUV420P, 
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
	    php_error_docref(NULL TSRMLS_CC, E_ERROR,
	            "Error parsing arguments");
	}

	switch (ZEND_NUM_ARGS()) {
	    case 6:
	        convert_to_long_ex(&argv[5]);
	        crop_right = Z_LVAL(argv[5]);

	        /* crop right must be even number for lavc cropping */
	        if (crop_right % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop right must be an even number");
	        }
	        /* fallthru */
	    case 5:
	        convert_to_long_ex(&argv[4]);
	        crop_left = Z_LVAL(argv[4]);

	        /*  crop left must be even number for lavc cropping */
	        if (crop_left % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop left must be an even number");
	        }

	        /* fallthru */
	    case 4:
	        convert_to_long_ex(&argv[3]);
	        crop_bottom = Z_LVAL(argv[3]);

	        /*  crop bottom must be even number for lavc cropping */
	        if (crop_bottom % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop bottom must be an even number");
	        }

	        /* fallthru */
	    case 3:
	        convert_to_long_ex(&argv[2]);
	        crop_top = Z_LVAL(argv[2]);

	        /*  crop top must be even number for lavc cropping */
	        if (crop_top % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop top must be an even number");
	        }

	        /* fallthru */
	    case 2:
	        /* height arg */
	        convert_to_long_ex(&argv[1]);
	        wanted_height = Z_LVAL(argv[1]);

	        /* bounds check wanted height */
	        if (wanted_height < 1) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Frame height must be greater than zero");
	        }

	        /* wanted height must be even number for lavc resample */
	        if (wanted_height % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Frame height must be an even number");
	        }
	        /* fallthru */
	    case 1:
	        /* width arg */
	        convert_to_long_ex(&argv[0]);
	        wanted_width = Z_LVAL(argv[0]);

	        /* bounds check wanted width */
	        if (wanted_width < 1) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Frame width must be greater than zero");
	        }

	        /* wanted width must be even number for lavc resample */
	        if (wanted_width % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
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
	    php_error_docref(NULL TSRMLS_CC, E_ERROR,
	            "Error parsing arguments");
	}

	switch (ZEND_NUM_ARGS()) {
	    case 4:
	        convert_to_long_ex(&argv[3]);
	        crop_right = Z_LVAL(argv[3]);

	        /* crop right must be even number for lavc cropping */
	        if (crop_right % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop right must be an even number");
	        }
	        /* fallthru */
	    case 3:
	        convert_to_long_ex(&argv[2]);
	        crop_left = Z_LVAL(argv[2]);

	        /*  crop left must be even number for lavc cropping */
	        if (crop_left % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop left must be an even number");
	        }

	        /* fallthru */
	    case 2:
	        convert_to_long_ex(&argv[1]);
	        crop_bottom = Z_LVAL(argv[1]);

	        /*  crop bottom must be even number for lavc cropping */
	        if (crop_bottom % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
	                    "Crop bottom must be an even number");
	        }

	        /* fallthru */
	    case 1:
	        convert_to_long_ex(&argv[0]);
	        crop_top = Z_LVAL(argv[0]);

	        /*  crop top  must be even number for lavc cropping */
	        if (crop_top % 2) {
	            php_error_docref(NULL TSRMLS_CC, E_ERROR,
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
