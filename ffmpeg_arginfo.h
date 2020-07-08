/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1d85259c468632bc48a758f16674b7f281956e4c */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ffmpeg_movie___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ffmpeg_movie_getframewidth, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_movie_getframeheight arginfo_class_ffmpeg_movie_getframewidth

#define arginfo_class_ffmpeg_frame_getwidth arginfo_class_ffmpeg_movie_getframewidth

#define arginfo_class_ffmpeg_frame_getheight arginfo_class_ffmpeg_movie_getframewidth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ffmpeg_frame_iskeyframe, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(ffmpeg_movie, __construct);
ZEND_METHOD(ffmpeg_movie, getframewidth);
ZEND_METHOD(ffmpeg_movie, getframeheight);
ZEND_METHOD(ffmpeg_frame, getwidth);
ZEND_METHOD(ffmpeg_frame, getheight);
ZEND_METHOD(ffmpeg_frame, iskeyframe);


static const zend_function_entry class_ffmpeg_movie_methods[] = {
	ZEND_ME(ffmpeg_movie, __construct, arginfo_class_ffmpeg_movie___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframewidth, arginfo_class_ffmpeg_movie_getframewidth, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframeheight, arginfo_class_ffmpeg_movie_getframeheight, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ffmpeg_frame_methods[] = {
	ZEND_ME(ffmpeg_frame, getwidth, arginfo_class_ffmpeg_frame_getwidth, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, getheight, arginfo_class_ffmpeg_frame_getheight, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, iskeyframe, arginfo_class_ffmpeg_frame_iskeyframe, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
