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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
}
#include "php_dd.h"
#include "dd_cookie.h"
#include "dd_stat.h"

#include <string>

/* If you declare any globals in php_dd.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(dd)
*/

/* True global resources - no need for thread safety here */
static int le_dd;
static int le_ddcookie; // class: DDCookie
static int le_ddstat; // class: DDStat

/* for dd_full_urlencode() */
static unsigned char hexchars[] = "0123456789ABCDEF";

/* {{{ dd_functions[]
 *
 * Every user visible function must have an entry in dd_functions[].
 */
zend_function_entry dd_functions[] = {
  PHP_FE(ddcookie_new,            NULL)
  PHP_FE(ddcookie_destroy,        NULL)
  PHP_FE(ddcookie_get_login_user, NULL)
  
  PHP_FE(dd_full_urlencode, NULL)
  
  {NULL, NULL, NULL}  /* Must be the last line in dd_functions[] */
};
/* }}} */

/* {{{ dd_module_entry
 */
zend_module_entry dd_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  "dd",
  dd_functions,
  PHP_MINIT(dd),
  PHP_MSHUTDOWN(dd),
  PHP_RINIT(dd),    /* Replace with NULL if there's nothing to do at request start */
  PHP_RSHUTDOWN(dd),  /* Replace with NULL if there's nothing to do at request end */
  PHP_MINFO(dd),
#if ZEND_MODULE_API_NO >= 20010901
  PHP_DD_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DD
BEGIN_EXTERN_C()
ZEND_GET_MODULE(dd)
END_EXTERN_C()
#endif






/*
 +----------------------------------------------------------------------+
 | DD Extension                                                         |
 | Class: DDCookie,                                                     |
 +----------------------------------------------------------------------+
 | Author:      PanZhibiao                                              |
 | Since:       2011-11                                                 |
 | LastModify:  2011-11                                                 |
 +----------------------------------------------------------------------+
 */

/* {{{ DD Class */
zend_class_entry *dd_ce_ddcookie;
static zend_object_handlers dd_object_handlers_ddcookie;

function_entry dd_ddcookie_methods[] = {
  /* dd_cookie */
  ZEND_ME_MAPPING(DDCookie,     ddcookie_new,            NULL, 0)
  ZEND_ME_MAPPING(getLoginUser, ddcookie_get_login_user, NULL, 0)
  
  {NULL, NULL, NULL}
};



/* dd_stat */
zend_class_entry *dd_ce_ddstat;
static zend_object_handlers dd_object_handlers_ddstat;
function_entry dd_ddstat_methods[] = {
  /* dd_stat */
  ZEND_ME_MAPPING(DDStat,           ddstat_new,              NULL, 0)
  ZEND_ME_MAPPING(initTrackServer,  ddstat_initTrackServer,  NULL, 0)
  ZEND_ME_MAPPING(trackByUDP,       ddstat_trackByUDP,       NULL, 0)
  ZEND_ME_MAPPING(trackByHTTP,      ddstat_trackByHTTP,      NULL, 0)
  ZEND_ME_MAPPING(trackCountByUDP,  ddstat_trackCountByUDP,  NULL, 0)
  ZEND_ME_MAPPING(trackValueByUDP,  ddstat_trackValueByUDP,  NULL, 0)
  ZEND_ME_MAPPING(trackCountByHTTP, ddstat_trackCountByHTTP, NULL, 0)
  ZEND_ME_MAPPING(trackValueByHTTP, ddstat_trackValueByHTTP, NULL, 0)
  
//  PHP_ME(DDStat, __construct,      arginfo___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
//  PHP_ME(DDStat, __destruct,       NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
//  PHP_ME(DDStat, initTrackServer,  NULL, ZEND_ACC_PUBLIC)
//  PHP_ME(DDStat, trackByUDP,       NULL, ZEND_ACC_PUBLIC)
//  PHP_ME(DDStat, trackByHTTP,      NULL, ZEND_ACC_PUBLIC)
//  PHP_ME(DDStat, trackCountByUDP,  NULL, ZEND_ACC_PUBLIC)
//  PHP_ME(DDStat, trackValueByUDP,  NULL, ZEND_ACC_PUBLIC)
//  PHP_ME(DDStat, trackCountByHTTP, NULL, ZEND_ACC_PUBLIC)
//  PHP_ME(DDStat, trackValueByHTTP, NULL, ZEND_ACC_PUBLIC)
  
  {NULL, NULL, NULL}
};

typedef enum {
  is_ddcookie,
  is_ddstat
} dd_obj_type;

struct dd_obj {
  zend_object std;
  int rsrc_id;
  dd_obj_type type;
  union {
    DDCookie *ddcookie;
    DDStat   *ddstat;
  } u;
};


/* }}}  */


static void dd_object_free_storage(zend_object *object TSRMLS_DC) {
  dd_obj *obj = (dd_obj *) object;
  zend_hash_destroy(obj->std.properties);
  FREE_HASHTABLE(obj->std.properties);
  if (obj->u.ddcookie) {
    zend_list_delete(obj->rsrc_id);
  }
  efree(obj);
}

static void dd_object_new(zend_class_entry *class_type,
                          zend_object_handlers *handlers,
                          zend_object_value *retval TSRMLS_DC) {
  zval *tmp;
  dd_obj *obj = (dd_obj *)emalloc(sizeof(dd_obj));
  memset(obj, 0, sizeof(dd_obj));
  obj->std.ce = class_type;
  
  ALLOC_HASHTABLE(obj->std.properties);
  zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
  zend_hash_copy(obj->std.properties,
                 &class_type->default_properties,
                 (copy_ctor_func_t) zval_add_ref,
                 (void *) &tmp,
                 sizeof(zval *));
  retval->handle = zend_objects_store_put(obj,
                                          (zend_objects_store_dtor_t)zend_objects_destroy_object,
                                          (zend_objects_free_object_storage_t)dd_object_free_storage,
                                          NULL TSRMLS_CC);
  retval->handlers = handlers;
}

/* Register the class entry.. */
#define DD_REGISTER_CLASS(name, obj_name) \
  { \
    zend_class_entry ce; \
    INIT_CLASS_ENTRY(ce, obj_name, dd_ ## name ##_methods); \
    ce.create_object = dd_object_new_ ## name; \
    dd_ce_ ## name = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC); \
    memcpy(&dd_object_handlers_ ## name, \
    zend_get_std_object_handlers(), \
    sizeof(zend_object_handlers)); \
    dd_object_handlers_ ## name.clone_obj = NULL; \
    dd_ce_ ## name->ce_flags |= ZEND_ACC_FINAL_CLASS; \
  }


/* Register resources. If we're using an object, put it into the object store. */
#define DD_REGISTER_RESOURCE(obj_type, return_value, res, le) \
{ \
  int rsrc_id = ZEND_REGISTER_RESOURCE(object ? NULL : return_value, res, le); \
  if (object) { \
    dd_obj *obj = (dd_obj *) zend_object_store_get_object(object TSRMLS_CC); \
    obj->u.obj_type= res; \
    obj->rsrc_id = rsrc_id; \
    obj->type = is_ ## obj_type; \
  } \
}


#define DD_REGISTER_DDCOOKIE_RESOURCE(return_value, res, le) \
  DD_REGISTER_RESOURCE(ddcookie, return_value, res, le)

#define DD_REGISTER_DDSTAT_RESOURCE(return_value, res, le) \
  DD_REGISTER_RESOURCE(ddstat, return_value, res, le)


/* These are for parsing parameters and getting the actual object from the store. */
#define DD_GET_THIS() \
  zval* object = getThis();

#define DD_SET_OBJ(type) \
  dd_obj *obj = (dd_obj *) zend_object_store_get_object(object TSRMLS_CC); \
  type ## _instance = obj->u.type;

#define DD_OBJ_PARAMS(type, params) \
  DD_GET_THIS(); \
  if (object) { \
    if (params == FAILURE) { \
      RETURN_FALSE; \
    } \
      DD_SET_OBJ(type) \
    } \
  else

#define DD_OBJ_NO_PARAMS(type) \
  DD_GET_THIS(); \
  if (object) { \
    if (ZEND_NUM_ARGS() != 0) { \
      php_error(E_WARNING, "didn't expect any arguments in %s()", \
      get_active_function_name(TSRMLS_C)); \
    } \
    DD_SET_OBJ(type) \
  } \
  else

#define DD_DDCOOKIE_OBJ_PARAMS(params)  DD_OBJ_PARAMS(ddcookie, params)
#define DD_DDCOOKIE_OBJ_NO_PARAMS()     DD_OBJ_NO_PARAMS(ddcookie)

#define DD_DDSTAT_OBJ_PARAMS(params)    DD_OBJ_PARAMS(ddstat, params)
#define DD_DDSTAT_OBJ_NO_PARAMS()       DD_OBJ_NO_PARAMS(ddstat)


#define DD_DDCOOKIE_RSRC_PARAMS(params) \
  if (params == FAILURE) { \
    RETURN_FALSE; \
  } \
  else { \
    ZEND_FETCH_RESOURCE(ddcookie_instance, DDCookie*, &resource, -1, "ddcookie", le_ddcookie); \
  }
#define DD_DDCOOKIE_RSRC_NO_PARAMS() \
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource) == FAILURE) { \
    RETURN_FALSE; \
  } \
  else { \
    ZEND_FETCH_RESOURCE(ddcookie_instance, DDCookie*, &resource, -1, "ddcookie", le_ddcookie); \
  }


#define DD_DDSTAT_RSRC_PARAMS(params) \
  if (params == FAILURE) { \
    RETURN_FALSE; \
  } \
  else { \
    ZEND_FETCH_RESOURCE(ddstat_instance, DDStat*, &resource, -1, "ddstat", le_ddstat); \
  }
#define DD_DDSTAT_RSRC_NO_PARAMS() \
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource) == FAILURE) { \
    RETURN_FALSE; \
  } \
  else { \
    ZEND_FETCH_RESOURCE(ddstat_instance, DDStat*, &resource, -1, "ddstat", le_ddstat); \
  }


static ZEND_RSRC_DTOR_FUNC(destroy_ddcookie) {
  if (rsrc->ptr) {
    delete (DDCookie*) rsrc->ptr;
    rsrc->ptr = NULL;
  }
}
static zend_object_value dd_object_new_ddcookie(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;
  dd_object_new(class_type, &dd_object_handlers_ddcookie, &retval TSRMLS_CC);
  return retval;
}


static ZEND_RSRC_DTOR_FUNC(destroy_ddstat) {
  if (rsrc->ptr) {
    delete (DDStat*) rsrc->ptr;
    rsrc->ptr = NULL;
  }
}
static zend_object_value dd_object_new_ddstat(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;
  dd_object_new(class_type, &dd_object_handlers_ddstat, &retval TSRMLS_CC);
  return retval;
}






/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("dd.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_dd_globals, dd_globals)
    STD_PHP_INI_ENTRY("dd.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_dd_globals, dd_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_dd_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_dd_init_globals(zend_dd_globals *dd_globals)
{
  dd_globals->global_value = 0;
  dd_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(dd)
{
  /* If you have INI entries, uncomment these lines 
  REGISTER_INI_ENTRIES();
  */
  
  /* DDCookie */
  DD_REGISTER_CLASS(ddcookie, "DDCookie");
  le_ddcookie = zend_register_list_destructors_ex(destroy_ddcookie, NULL,
                                                 "ddcookie", module_number);
  
  /* DDStat */
  DD_REGISTER_CLASS(ddstat, "DDStat");
  le_ddstat = zend_register_list_destructors_ex(destroy_ddstat, NULL,
                                                "ddstat", module_number);
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(dd)
{
  /* uncomment this line if you have INI entries
  UNREGISTER_INI_ENTRIES();
  */
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(dd)
{
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(dd)
{
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(dd)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "dd support", "enabled");
  
  php_info_print_table_row(2, "Version", PHP_DD_VERSION);
  php_info_print_table_row(2, "Release Date", PHP_DD_RELEASE_DATE);
  php_info_print_table_row(2, "Copyright", "Copyright (C) 2004-2011 Dangdang Inc.");
  
  php_info_print_table_end();

  /* Remove comments if you have entries in php.ini
  DISPLAY_INI_ENTRIES();
  */
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


/* {{{ ddcookie_new()
 */
PHP_FUNCTION(ddcookie_new)
{
  DDCookie *ddcookie_instance;
  char* s;
  int len = 0;
  DD_GET_THIS();
  
  ddcookie_instance = new DDCookie();
  if (ddcookie_instance != NULL) {
    DD_REGISTER_DDCOOKIE_RESOURCE(return_value, ddcookie_instance, le_ddcookie);
  }
  
  if (ddcookie_instance->Init() != 0) {
    php_error(E_ERROR, "DDCookie Init Error: can't open \"/dev/urandom\"");
  }
}
/* }}} */


/* {{{ ddcookie_destroy()
 */
PHP_FUNCTION(ddcookie_destroy)
{
  zval *resource;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &resource) == FAILURE) {
    RETURN_FALSE;
  }
  
  zend_list_delete(Z_RESVAL_P(resource));
}
/* }}} */


/* {{{ ddcookie_get_login_user(string login_dangdang_com, string dangdang_com)
 */
PHP_FUNCTION(ddcookie_get_login_user)
{
  zval *resource;
  DDCookie *ddcookie_instance;
  
  char *dangdang_com, *login_dangdang_com;
  int len_dangdang_com = -1, len_login_dangdang_com = -1;
  char user_arr[DD_COOKIE_USER_KEY_SIZE][DD_COOKIE_USER_VALUE_SIZE] = {0};
  
  DD_DDCOOKIE_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "ss", &login_dangdang_com, &len_login_dangdang_com,
                                               &dangdang_com, &len_dangdang_com))
  DD_DDCOOKIE_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                                "rss", &resource,
                                                &login_dangdang_com, &len_login_dangdang_com,
                                                &dangdang_com, &len_dangdang_com))
  if (ddcookie_instance == NULL) {
    php_error(E_WARNING, "can't set string on null resource in %s()",
              get_active_function_name(TSRMLS_C));
    RETURN_FALSE;
  }
  if (len_dangdang_com == 0 || len_login_dangdang_com == 0) {
    RETURN_FALSE;
  }
  
  int res = ddcookie_instance->getLoginUser(dangdang_com,
                                            login_dangdang_com, user_arr);
  if (res != 0) { RETURN_FALSE; }
  
  // return array
  array_init(return_value);
  add_assoc_string(return_value, "email",      user_arr[0], 1);
  add_assoc_long  (return_value, "customerid", atol(user_arr[1]));
  add_assoc_string(return_value, "nickname",   user_arr[2], 1);
  add_assoc_string(return_value, "display_id", user_arr[3], 1);
  add_assoc_string(return_value, "show_name",  user_arr[5], 1);
}
/* }}} */


/* {{{ dd_full_urlencode(string s) */
PHP_FUNCTION(dd_full_urlencode)
{
  char *in = NULL;
  int in_len, out_len; 
  register unsigned char c;
  const char *in_ptr;
  char *out, *out_ptr;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_len) == FAILURE) {
    RETURN_FALSE;
  }
  out_len = in_len * 3;
  out = out_ptr = (char *)emalloc(out_len + 1);
  in_ptr = in;

  while((c = *in_ptr++) != '\0'){
    out_ptr[0] = '%';
    out_ptr[1] = hexchars[c >> 4];
    out_ptr[2] = hexchars[c & 15];
    out_ptr += 3;
  }
  out_ptr[0] = '\0';

  RETURN_STRINGL(out, out_len, 0);
}
/* }}} */



PHP_METHOD(DDStat, __construct)
{
  DDStat *ddstat_instance;
  char* s;
  int len = 0;
  DD_GET_THIS();
  
  ddstat_instance = new DDStat();
  if (ddstat_instance != NULL) {
    DD_REGISTER_DDSTAT_RESOURCE(return_value, ddstat_instance, le_ddstat);
  }
}


PHP_FUNCTION(ddstat_new)
{
  DDStat *ddstat_instance;
  char* s;
  int len = 0;
  DD_GET_THIS();
  
  ddstat_instance = new DDStat();
  if (ddstat_instance != NULL) {
    DD_REGISTER_DDSTAT_RESOURCE(return_value, ddstat_instance, le_ddstat);
  }
}

PHP_FUNCTION(ddstat_initTrackServer)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  char *host;
  int port, host_len;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "sl",
                                               &host, &host_len,
                                               &port))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                             "rsl", &resource,
                                             &host, &host_len,
                                             &port))
  if (ddstat_instance->initTrackServer(host, port) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


PHP_FUNCTION(ddstat_trackByHTTP)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  int op, trk_id, dtlen;
  char *data;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "lls",
                                               &op, &trk_id,
                                               &data, &dtlen))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                              "rlls", &resource,
                                              &op, &trk_id,
                                              &data, &dtlen))
  if (ddstat_instance->trackByHTTP(op, trk_id, data, dtlen) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


PHP_FUNCTION(ddstat_trackByUDP)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  int op, trk_id, dtlen;
  char *data;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "lls",
                                               &op, &trk_id,
                                               &data, &dtlen))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                              "rlls", &resource,
                                              &op, &trk_id,
                                              &data, &dtlen))
  if (ddstat_instance->trackByUDP(op, trk_id, data, dtlen) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


PHP_FUNCTION(ddstat_trackCountByUDP)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  int op, trk_id, val;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "lll",
                                               &op, &trk_id, &val))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                              "rlll", &resource,
                                              &op, &trk_id, &val))
  if (ddstat_instance->trackCountByUDP(op, trk_id, val) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}

PHP_FUNCTION(ddstat_trackValueByUDP)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  int op, trk_id, val;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "lll",
                                               &op, &trk_id, &val))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                              "rlll", &resource,
                                              &op, &trk_id, &val))
  if (ddstat_instance->trackValueByUDP(op, trk_id, val) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


PHP_FUNCTION(ddstat_trackCountByHTTP)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  int op, trk_id, val;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "lll",
                                               &op, &trk_id, &val))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                              "rlll", &resource,
                                              &op, &trk_id, &val))
  if (ddstat_instance->trackCountByHTTP(op, trk_id, val) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


PHP_FUNCTION(ddstat_trackValueByHTTP)
{
  zval *resource;
  DDStat *ddstat_instance;
  
  int op, trk_id, val;
  DD_DDSTAT_OBJ_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                               "lll",
                                               &op, &trk_id, &val))
  DD_DDSTAT_RSRC_PARAMS(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                                              "rlll", &resource,
                                              &op, &trk_id, &val))
  if (ddstat_instance->trackValueByHTTP(op, trk_id, val) == 0) {
    RETURN_TRUE;
  }
  RETURN_FALSE;
}


