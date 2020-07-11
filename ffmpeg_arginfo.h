/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ad9af7eb390f313800f6ceca1db1159ca402552f */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_ffmpeg_movie___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ffmpeg_movie_getduration, 0, 0, double, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ffmpeg_movie_getframecount, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_movie_getframerate arginfo_class_ffmpeg_movie_getduration

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ffmpeg_movie_getfilename, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_movie_getcomment arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_gettitle arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getauthor arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getartist arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getcopyright arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getalbum arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getgenre arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getyear arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_gettracknumber arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getframewidth arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getframeheight arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getframenumber arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getpixelformat arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getbitrate arginfo_class_ffmpeg_movie_getframecount

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ffmpeg_movie_hasaudio, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_movie_hasvideo arginfo_class_ffmpeg_movie_hasaudio

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ffmpeg_movie_getnextkeyframe, 0, 0, ffmpeg_frame, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_ffmpeg_movie_getframe, 0, 1, ffmpeg_frame, 0)
	ZEND_ARG_INFO(0, int)
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_movie_getvideocodec arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getaudiocodec arginfo_class_ffmpeg_movie_getfilename

#define arginfo_class_ffmpeg_movie_getvideostreamid arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getaudiostreamid arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getaudiochannels arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getaudiosamplerate arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getaudiobitrate arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getvideobitrate arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_movie_getpixelaspectratio arginfo_class_ffmpeg_movie_getduration

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_ffmpeg_frame_togdimage, 0, 0, GdImage, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_frame_getwidth arginfo_class_ffmpeg_movie_getframecount

#define arginfo_class_ffmpeg_frame_getheight arginfo_class_ffmpeg_movie_getframecount

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_ffmpeg_frame_resize, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, wanted_width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, wanted_height, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, crop_top, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, crop_bottom, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, crop_left, IS_LONG, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, crop_right, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_ffmpeg_frame_iskeyframe arginfo_class_ffmpeg_movie_hasaudio

#define arginfo_class_ffmpeg_frame_getpresentationtimestamp arginfo_class_ffmpeg_movie_getduration

#define arginfo_class_ffmpeg_frame_getpts arginfo_class_ffmpeg_movie_getduration


ZEND_METHOD(ffmpeg_movie, __construct);
ZEND_METHOD(ffmpeg_movie, getduration);
ZEND_METHOD(ffmpeg_movie, getframecount);
ZEND_METHOD(ffmpeg_movie, getframerate);
ZEND_METHOD(ffmpeg_movie, getfilename);
ZEND_METHOD(ffmpeg_movie, getcomment);
ZEND_METHOD(ffmpeg_movie, gettitle);
ZEND_METHOD(ffmpeg_movie, getauthor);
ZEND_METHOD(ffmpeg_movie, getartist);
ZEND_METHOD(ffmpeg_movie, getcopyright);
ZEND_METHOD(ffmpeg_movie, getalbum);
ZEND_METHOD(ffmpeg_movie, getgenre);
ZEND_METHOD(ffmpeg_movie, getyear);
ZEND_METHOD(ffmpeg_movie, gettracknumber);
ZEND_METHOD(ffmpeg_movie, getframewidth);
ZEND_METHOD(ffmpeg_movie, getframeheight);
ZEND_METHOD(ffmpeg_movie, getframenumber);
ZEND_METHOD(ffmpeg_movie, getpixelformat);
ZEND_METHOD(ffmpeg_movie, getbitrate);
ZEND_METHOD(ffmpeg_movie, hasaudio);
ZEND_METHOD(ffmpeg_movie, hasvideo);
ZEND_METHOD(ffmpeg_movie, getnextkeyframe);
ZEND_METHOD(ffmpeg_movie, getframe);
ZEND_METHOD(ffmpeg_movie, getvideocodec);
ZEND_METHOD(ffmpeg_movie, getaudiocodec);
ZEND_METHOD(ffmpeg_movie, getvideostreamid);
ZEND_METHOD(ffmpeg_movie, getaudiostreamid);
ZEND_METHOD(ffmpeg_movie, getaudiochannels);
ZEND_METHOD(ffmpeg_movie, getaudiosamplerate);
ZEND_METHOD(ffmpeg_movie, getaudiobitrate);
ZEND_METHOD(ffmpeg_movie, getvideobitrate);
ZEND_METHOD(ffmpeg_movie, getpixelaspectratio);
ZEND_METHOD(ffmpeg_frame, togdimage);
ZEND_METHOD(ffmpeg_frame, getwidth);
ZEND_METHOD(ffmpeg_frame, getheight);
ZEND_METHOD(ffmpeg_frame, resize);
ZEND_METHOD(ffmpeg_frame, iskeyframe);
ZEND_METHOD(ffmpeg_frame, getpresentationtimestamp);
ZEND_METHOD(ffmpeg_frame, getpts);


static const zend_function_entry class_ffmpeg_movie_methods[] = {
	ZEND_ME(ffmpeg_movie, __construct, arginfo_class_ffmpeg_movie___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getduration, arginfo_class_ffmpeg_movie_getduration, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframecount, arginfo_class_ffmpeg_movie_getframecount, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframerate, arginfo_class_ffmpeg_movie_getframerate, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getfilename, arginfo_class_ffmpeg_movie_getfilename, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getcomment, arginfo_class_ffmpeg_movie_getcomment, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, gettitle, arginfo_class_ffmpeg_movie_gettitle, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getauthor, arginfo_class_ffmpeg_movie_getauthor, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getartist, arginfo_class_ffmpeg_movie_getartist, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getcopyright, arginfo_class_ffmpeg_movie_getcopyright, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getalbum, arginfo_class_ffmpeg_movie_getalbum, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getgenre, arginfo_class_ffmpeg_movie_getgenre, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getyear, arginfo_class_ffmpeg_movie_getyear, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, gettracknumber, arginfo_class_ffmpeg_movie_gettracknumber, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframewidth, arginfo_class_ffmpeg_movie_getframewidth, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframeheight, arginfo_class_ffmpeg_movie_getframeheight, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframenumber, arginfo_class_ffmpeg_movie_getframenumber, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getpixelformat, arginfo_class_ffmpeg_movie_getpixelformat, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getbitrate, arginfo_class_ffmpeg_movie_getbitrate, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, hasaudio, arginfo_class_ffmpeg_movie_hasaudio, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, hasvideo, arginfo_class_ffmpeg_movie_hasvideo, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getnextkeyframe, arginfo_class_ffmpeg_movie_getnextkeyframe, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getframe, arginfo_class_ffmpeg_movie_getframe, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getvideocodec, arginfo_class_ffmpeg_movie_getvideocodec, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getaudiocodec, arginfo_class_ffmpeg_movie_getaudiocodec, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getvideostreamid, arginfo_class_ffmpeg_movie_getvideostreamid, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getaudiostreamid, arginfo_class_ffmpeg_movie_getaudiostreamid, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getaudiochannels, arginfo_class_ffmpeg_movie_getaudiochannels, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getaudiosamplerate, arginfo_class_ffmpeg_movie_getaudiosamplerate, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getaudiobitrate, arginfo_class_ffmpeg_movie_getaudiobitrate, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getvideobitrate, arginfo_class_ffmpeg_movie_getvideobitrate, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_movie, getpixelaspectratio, arginfo_class_ffmpeg_movie_getpixelaspectratio, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_ffmpeg_frame_methods[] = {
	ZEND_ME(ffmpeg_frame, togdimage, arginfo_class_ffmpeg_frame_togdimage, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, getwidth, arginfo_class_ffmpeg_frame_getwidth, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, getheight, arginfo_class_ffmpeg_frame_getheight, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, resize, arginfo_class_ffmpeg_frame_resize, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, iskeyframe, arginfo_class_ffmpeg_frame_iskeyframe, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, getpresentationtimestamp, arginfo_class_ffmpeg_frame_getpresentationtimestamp, ZEND_ACC_PUBLIC)
	ZEND_ME(ffmpeg_frame, getpts, arginfo_class_ffmpeg_frame_getpts, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
