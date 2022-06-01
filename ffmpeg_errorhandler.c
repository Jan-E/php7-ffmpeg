/*
   This file is part of ffmpeg-php

   Copyright (C) 2004-2008 Alexey Zakhlestin <indeyets@gmail.com>

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
#include <avcodec.h>

/* {{{ ffmpeg_errorhandler()
 */
void ffmpeg_errorhandler(void *ptr, int level, const char *msg, va_list args)
{
	int php_level;
	TSRMLS_FETCH();

	switch (level) {
		case AV_LOG_ERROR:
			php_level = E_WARNING;
		break;

		case AV_LOG_INFO:
		case AV_LOG_DEBUG:
		default:
			php_level = E_NOTICE;
		break;
	}

	php_verror("", "", php_level, msg, args TSRMLS_CC);
}
/* }}} */


/* {{{ ffmpeg_hide_errors()
 */
void ffmpeg_hide_errors(void *ptr, int level, const char *msg, va_list args)
{
    // NO OP
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
