/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifndef PHP_DD_H
#define PHP_DD_H

#define PHP_DD_VERSION       "0.1.3"
#define PHP_DD_RELEASE_DATE  "2012.06.01"

extern zend_module_entry dd_module_entry;
#define phpext_dd_ptr &dd_module_entry

#ifdef PHP_WIN32
#	define PHP_DD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_DD_API __attribute__ ((visibility("default")))
#else
#	define PHP_DD_API
#endif

extern "C" {
#ifdef ZTS
#include "TSRM.h"
#endif
}

PHP_MINIT_FUNCTION(dd);
PHP_MSHUTDOWN_FUNCTION(dd);
PHP_RINIT_FUNCTION(dd);
PHP_RSHUTDOWN_FUNCTION(dd);
PHP_MINFO_FUNCTION(dd);


/* DDCookie */
PHP_FUNCTION(ddcookie_new);
PHP_FUNCTION(ddcookie_destroy);
PHP_FUNCTION(ddcookie_get_login_user);

/* full_urlencode */
PHP_FUNCTION(dd_full_urlencode);

/* DDStat */
PHP_FUNCTION(ddstat_new);
PHP_FUNCTION(ddstat_initTrackServer);
PHP_FUNCTION(ddstat_trackByUDP);
PHP_FUNCTION(ddstat_trackByHTTP);
PHP_FUNCTION(ddstat_trackCountByUDP);
PHP_FUNCTION(ddstat_trackValueByUDP);
PHP_FUNCTION(ddstat_trackCountByHTTP);
PHP_FUNCTION(ddstat_trackValueByHTTP);


/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(dd)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(dd)
*/

/* In every utility function you add that needs to use variables 
   in php_dd_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as DD_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define DD_G(v) TSRMG(dd_globals_id, zend_dd_globals *, v)
#else
#define DD_G(v) (dd_globals.v)
#endif

#endif	/* PHP_DD_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
