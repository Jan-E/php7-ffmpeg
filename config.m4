PHP_ARG_WITH(ffmpeg,for ffmpeg support, 
[  --with-ffmpeg[=DIR]       Include ffmpeg support])

PHP_ARG_WITH(libgd-incdir, C include dir for libgd,
[  --with-libgd-incdir[=DIR] Include path for the C headers of libgd])

dnl Determine path to ffmpeg libs
if test "$PHP_FFMPEG" != "no"; then

  dnl Default directories to check for ffmpeg headers
  INC_CHECK_DIRS="/usr/local /usr"

  dnl User has specified using --with=ffmpeg=[dir] a directory where we should
  dnl look for ffmpeg headers. Prepend this directory to the default dirs so it
  dnl gets checked first.
  if test "$PHP_FFMPEG" != "yes"; then
    INC_CHECK_DIRS="$PHP_FFMPEG $INC_CHECK_DIRS"
  fi

  AC_MSG_CHECKING(for ffmpeg headers)
  for i in $INC_CHECK_DIRS ; do
    if test -f $i/include/ffmpeg/avcodec.h; then
      PHP_ADD_INCLUDE($i/include/ffmpeg)
      FFMPEG_INC_FOUND=$i/include/ffmpeg
      break
    elif test -f $i/include/avcodec.h; then
      PHP_ADD_INCLUDE($i/include)
      FFMPEG_INC_FOUND=$i/include
      break
    elif test -f $i/include/libavcodec/avcodec.h; then
      dnl ffmpeg svn revision 12194 and newer put each header in its own dir
      dnl so we have to include them all.
      PHP_ADD_INCLUDE($i/include/libavcodec/)
      PHP_ADD_INCLUDE($i/include/libavformat/)
      PHP_ADD_INCLUDE($i/include/libavutil/)
      PHP_ADD_INCLUDE($i/include/libswscale/)
      PHP_ADD_INCLUDE($i/include/libavfilter/)
      PHP_ADD_INCLUDE($i/include/libavdevice/)
      FFMPEG_INC_FOUND=$i/include/libavcodec
      break
    fi
  done

  if test -z "$FFMPEG_INC_FOUND"; then 
    AC_MSG_RESULT()
    AC_MSG_ERROR([ffmpeg headers not found. Make sure ffmpeg is compiled as shared libraries using the --enable-shared option])
  else
    AC_MSG_RESULT(...found in $FFMPEG_INC_FOUND)
  fi
 

  AC_MSG_CHECKING(for ffmpeg libavcodec.so)
  for i in $PHP_FFMPEG /usr/local /usr ; do
    if test -f $i/lib/libavcodec.so; then
      FFMPEG_LIBDIR=$i/lib
    fi
    dnl PATCH: 1785450 x86_64 support (Bent Nagstrup Terp)
    if test -f $i/lib64/libavcodec.so; then
      FFMPEG_LIBDIR=$i/lib64
    fi
    dnl MacOS-X support (Alexey Zakhlestin)
    if test -f $i/lib/libavcodec.dylib; then
      FFMPEG_LIBDIR=$i/lib
    fi
  done

  PHP_ADD_LIBRARY_WITH_PATH(avcodec, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(avformat, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)


  if test -z "$FFMPEG_LIBDIR"; then
    AC_MSG_RESULT()
    AC_MSG_ERROR(ffmpeg shared libraries not found. Make sure ffmpeg is compiled as shared libraries using the --enable-shared option)
  else
    dnl For debugging
    AC_MSG_RESULT(...found in $FFMPEG_LIBDIR)
  fi

  dnl check if libavcodec contains img_convert
  dnl if not, that means that libswscale is compiled in
  AC_MSG_CHECKING(for ffmpeg swscale support)
  SAVED_LIBS=$LIBS
  LIBS="$LIBS -lavcodec"
  SAVED_CFLAGS=$CFLAGS
  CFLAGS="$CFLAGS -I $INCLUDES"

  AC_TRY_LINK([ #include <avcodec.h> ],
              [ img_convert(0, 0, 0, 0, 0, 0) ],
              [ enable_ffmpeg_swscale=no ],
              [ enable_ffmpeg_swscale=yes ] )
  AC_MSG_RESULT($enable_ffmpeg_swscale)
  LIBS=$SAVED_LIBS
  CFLAGS=$SAVED_CFLAGS

  if test "$enable_ffmpeg_swscale" == yes; then
    AC_DEFINE(HAVE_SWSCALER, 1, [Define to 1 if software scaler is compiled into ffmpeg])
    PHP_ADD_LIBRARY_WITH_PATH(swscale, $FFMPEG_LIBDIR, FFMPEG_SHARED_LIBADD)
  else
    dnl Ignore deprecation warnings that using img_convert generates these days
    CFLAGS="$CFLAGS -Wno-deprecated-declarations"
  fi

  dnl Get PHP version
  AC_MSG_CHECKING([PHP version])
  if test -z "$PHP_VERSION"; then
    if test -z "$PHP_CONFIG"; then
      AC_MSG_ERROR([php-config not found])
    fi
    PHP_VERSION=`$PHP_CONFIG --version 2>/dev/null`
    PHP_MAJOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/g' 2>/dev/null`
    PHP_MINOR_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/g' 2>/dev/null`
    PHP_RELEASE_VERSION=`echo $PHP_VERSION | sed -e 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/g' 2>/dev/null`
    AC_MSG_RESULT([$PHP_MAJOR_VERSION.$PHP_MINOR_VERSION.$PHP_RELEASE_VERSION])
  fi

  dnl usual suspect: --with-libgd-incdir=/usr/local/php71/include/php/ext/gd/libgd
  SEARCH_PATH="/usr/local /usr /usr/local/php$PHP_MAJOR_VERSION$PHP_MINOR_VERSION/include/php/ext/gd/libgd"
  SEARCH_FOR="gd.h"

  AC_MSG_CHECKING([for libgd includes])
  if test -r "$PHP_LIBGD_INCDIR/include/$SEARCH_FOR"; then
    LIBGD_INCDIR=$PHP_LIBGD_INCDIR/include
  elif test -r "$PHP_LIBGD_INCDIR/$SEARCH_FOR"; then
    LIBGD_INCDIR=$PHP_LIBGD_INCDIR
  else # search default path list
    for i in $SEARCH_PATH ; do
      if test -r "$i/include/$SEARCH_FOR"; then
        LIBGD_INCDIR=$i/include
      elif test -r "$i/$SEARCH_FOR"; then
        LIBGD_INCDIR=$i
      fi
    done
  fi

  if test -z "$LIBGD_INCDIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please install or include libgd])
  else
    AC_MSG_RESULT(found in $LIBGD_INCDIR)
    PHP_ADD_INCLUDE($LIBGD_INCDIR)
    AC_DEFINE(HAVE_LIBGD20,1,[ ])
  fi

  CFLAGS="$CFLAGS -Wall -fno-strict-aliasing"

  PHP_NEW_EXTENSION(ffmpeg, ffmpeg-php.c ffmpeg_movie.c ffmpeg_frame.c ffmpeg_errorhandler.c ffmpeg_tools.c, $ext_shared,, \\$(GDLIB_CFLAGS))
  dnl PHP_ADD_EXTENSION_DEP(ffmpeg, gd)

  PHP_SUBST(FFMPEG_SHARED_LIBADD)
  AC_DEFINE(HAVE_FFMPEG_PHP,1,[ ])
    
  dnl PHP_DEBUG_MACRO(test.dbg)
fi
