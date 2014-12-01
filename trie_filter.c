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
  | Author:  Lijun Wu    <wulijun01234@gmail.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_trie_filter.h"

/* True global resources - no need for thread safety here */
static int le_trie_filter;

/* {{{ trie_filter_functions[]
 *
 * Every user visible function must have an entry in trie_filter_functions[].
 */
zend_function_entry trie_filter_functions[] = {
	PHP_FE(trie_filter_load, NULL)
	PHP_FE(trie_filter_search, NULL)
    PHP_FE(trie_filter_search_all, NULL)
    PHP_FE(trie_filter_new, NULL)
    PHP_FE(trie_filter_store, NULL)
    PHP_FE(trie_filter_save, NULL)
    PHP_FE(trie_filter_free, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in trie_filter_functions[] */
};
/* }}} */

/* {{{ trie_filter_module_entry
 */
zend_module_entry trie_filter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"trie_filter",
	trie_filter_functions,
	PHP_MINIT(trie_filter),
	PHP_MSHUTDOWN(trie_filter),
	NULL,
	NULL,
	PHP_MINFO(trie_filter),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TRIE_FILTER
ZEND_GET_MODULE(trie_filter)
#endif

/* {{{ PHP_INI
 */
/*
PHP_INI_BEGIN()
    PHP_INI_ENTRY("trie_filter.dict_charset", "utf-8", PHP_INI_ALL, NULL)
PHP_INI_END()
*/
/* }}} */

static void php_trie_filter_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	Trie *trie = (Trie *)rsrc->ptr;
	trie_free(trie);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(trie_filter)
{
	le_trie_filter = zend_register_list_destructors_ex(
			php_trie_filter_dtor, 
			NULL, PHP_TRIE_FILTER_RES_NAME, module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(trie_filter)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(trie_filter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "trie_filter support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource trie_filter_load(string dict_file_path)
   Returns resource id, or NULL on error*/
PHP_FUNCTION(trie_filter_load)
{
	Trie *trie;
	char *path;
	int path_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", 
				&path, &path_len) == FAILURE) {
		RETURN_NULL();
	}

	trie = trie_new_from_file(path);
	if (!trie) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				"Unable to load %s", path);
		RETURN_NULL();
	}

	ZEND_REGISTER_RESOURCE(return_value, trie, le_trie_filter);
}
/* }}} */

static int trie_search_one(Trie *trie, const AlphaChar *text, int *offset, TrieData *length)
{
	TrieState *s;
	const AlphaChar *p;
	const AlphaChar *base;

	base = text;
    if (! (s = trie_root(trie))) {
        return -1;
    }

	while (*text) {		
		p = text;
		if (! trie_state_is_walkable(s, *p)) {
            trie_state_rewind(s);
			text++;
			continue;
		} else {
			trie_state_walk(s, *p++);
        }

		while (trie_state_is_walkable(s, *p) && ! trie_state_is_terminal(s))
			trie_state_walk(s, *p++);

		if (trie_state_is_terminal(s)) {
			*offset = text - base;
			*length = p - text;
            trie_state_free(s);
            
			return 1;
		}

        trie_state_rewind(s);
		text++;
	}
    trie_state_free(s);

	return 0;
}

static int trie_search_all(Trie *trie, const AlphaChar *text, zval *data)
{
	TrieState *s;
	const AlphaChar *p;
	const AlphaChar *base;
    zval *word = NULL;

	base = text;
    if (! (s = trie_root(trie))) {
        return -1;
    }

    while (*text) {   
        p = text;
        if(! trie_state_is_walkable(s, *p)) {
            trie_state_rewind(s);
            text++;
            continue;
        }

        while(*p && trie_state_is_walkable(s, *p) && ! trie_state_is_leaf(s)) {
            trie_state_walk(s, *p++);  
            if (trie_state_is_terminal(s)) { 
                MAKE_STD_ZVAL(word);
                array_init_size(word, 3);
                add_next_index_long(word, text - base);
                add_next_index_long(word, p - text);
                add_next_index_zval(data, word);        
            }        
        }
        trie_state_rewind(s);
        text++;
    }
    trie_state_free(s);

	return 0;
}

/* {{{ proto array trie_filter_search(int trie_tree_identifier, string centent)
   Returns info about first keyword, or false on error*/
PHP_FUNCTION(trie_filter_search)
{
	Trie *trie;
	zval *trie_resource;
	unsigned char *text;
	int text_len;

	int offset = -1, i, ret;
    TrieData length = 0;

	AlphaChar *alpha_text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", 
				&trie_resource, &text, &text_len) == FAILURE) {
		RETURN_FALSE;
	}

    array_init(return_value);
    if (text_len < 1 || strlen(text) != text_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "input is empty");
		return;
	}

	ZEND_FETCH_RESOURCE(trie, Trie *, &trie_resource, -1, 
			PHP_TRIE_FILTER_RES_NAME, le_trie_filter);

	alpha_text = emalloc(sizeof(AlphaChar) * (text_len + 1));

	for (i = 0; i < text_len; i++) {
		alpha_text[i] = (AlphaChar) text[i];
	}

	alpha_text[text_len] = TRIE_CHAR_TERM;

	ret = trie_search_one(trie, alpha_text, &offset, &length);
    efree(alpha_text);
	if (ret == 0) {
        return;
    } else if (ret == 1) {
		add_next_index_long(return_value, offset);
		add_next_index_long(return_value, length);
	} else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array trie_filter_search_all(int trie_tree_identifier, string centent)
   Returns info about all keywords, or false on error*/
PHP_FUNCTION(trie_filter_search_all)
{
	Trie *trie;
	zval *trie_resource;
	unsigned char *text;
	int text_len;

	int i, ret;

	AlphaChar *alpha_text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", 
				&trie_resource, &text, &text_len) == FAILURE) {
		RETURN_FALSE;
	}

    array_init(return_value);
    if (text_len < 1 || strlen(text) != text_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "input is empty");
		return;
	}

	ZEND_FETCH_RESOURCE(trie, Trie *, &trie_resource, -1, 
			PHP_TRIE_FILTER_RES_NAME, le_trie_filter);

	alpha_text = emalloc(sizeof(AlphaChar) * (text_len + 1));

	for (i = 0; i < text_len; i++) {
		alpha_text[i] = (AlphaChar) text[i];
	}

	alpha_text[text_len] = TRIE_CHAR_TERM;

	ret = trie_search_all(trie, alpha_text, return_value);
    efree(alpha_text);
	if (ret == 0) {
        return;
	} else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto resource trie_filter_new()
   Returns resource id, or NULL on error*/
PHP_FUNCTION(trie_filter_new)
{
    Trie *trie;
    AlphaMap *alpha_map;
    int ret;

    alpha_map = alpha_map_new();
    if (! alpha_map) {
        RETURN_NULL();
    }

    if (alpha_map_add_range(alpha_map, 0x00, 0xff) != 0) {
        /* treat all strings as byte stream */
        alpha_map_free(alpha_map);
        RETURN_NULL();
    }

    trie = trie_new(alpha_map);
    alpha_map_free(alpha_map);
    if (! trie) {      
        RETURN_NULL();
    }
    ZEND_REGISTER_RESOURCE(return_value, trie, le_trie_filter);
}
/* }}} */

#define KEYWORD_MAX_LEN 1024
/* {{{ proto bool trie_filter_store(int trie_tree_identifier, string keyword)
   Returns true, or false on error*/
PHP_FUNCTION(trie_filter_store)
{
    Trie *trie;
	zval *trie_resource;
	unsigned char *keyword, *p;
	int keyword_len, i;
    AlphaChar alpha_key[KEYWORD_MAX_LEN+1];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", 
				&trie_resource, &keyword, &keyword_len) == FAILURE) {
		RETURN_FALSE;
	}
    if (keyword_len > KEYWORD_MAX_LEN || keyword_len < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "keyword should has [1, %d] bytes", KEYWORD_MAX_LEN);
        RETURN_FALSE;
    }
	ZEND_FETCH_RESOURCE(trie, Trie *, &trie_resource, -1, PHP_TRIE_FILTER_RES_NAME, le_trie_filter);
    p = keyword;
    i = 0;
    while (*p && *p != '\n' && *p != '\r') {
        alpha_key[i++] = (AlphaChar)*p;
        p++;
    }
    alpha_key[i] = TRIE_CHAR_TERM;
    if (! trie_store(trie, alpha_key, -1)) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool trie_filter_save(int trie_tree_identifier, string dict_path)
   Returns true, or false on error*/
PHP_FUNCTION(trie_filter_save)
{
    Trie *trie;
    zval *trie_resource;
    unsigned char *filename;
    int filename_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", 
                &trie_resource, &filename, &filename_len) == FAILURE) {
        RETURN_FALSE;
    }
    if (filename_len < 1 || strlen(filename) != filename_len) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "save path required");
        RETURN_FALSE;
    }
    ZEND_FETCH_RESOURCE(trie, Trie *, &trie_resource, -1, PHP_TRIE_FILTER_RES_NAME, le_trie_filter);
    if (trie_save(trie, filename)) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool trie_filter_free(int trie_tree_identifier)
   Returns true, or false on error*/
PHP_FUNCTION(trie_filter_free)
{
    Trie *trie;
    zval *trie_resource;
    int resource_id;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &trie_resource) == FAILURE) {
        RETURN_FALSE;
    }
    ZEND_FETCH_RESOURCE(trie, Trie *, &trie_resource, -1, PHP_TRIE_FILTER_RES_NAME, le_trie_filter);
    resource_id = Z_RESVAL_P(trie_resource);
    if (zend_list_delete(resource_id) == SUCCESS) {
        RETURN_TRUE;
    }
    RETURN_FALSE;   
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
