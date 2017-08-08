#ifndef EXT_PHP_TRIE_FILTER_PHP7_WRAPPER_H_
#define EXT_PHP_TRIE_FILTER_PHP7_WRAPPER_H_

#include "ext/standard/php_http.h"

#if PHP_MAJOR_VERSION < 7
typedef int zend_size_t;

#define TRIE_ZEND_REGISTER_RESOURCE  ZEND_REGISTER_RESOURCE
#define TRIE_ZEND_FETCH_RESOURCE     ZEND_FETCH_RESOURCE
#define TRIE_MAKE_STD_ZVAL(p)        MAKE_STD_ZVAL(p)
#define TRIE_RESOURCE_FREE(resource) zend_list_delete(Z_RESVAL_P(resource))
#else /* PHP Version 7 */
typedef size_t zend_size_t;
typedef zend_resource zend_rsrc_list_entry;

#define TRIE_ZEND_REGISTER_RESOURCE(return_value, result, le_result) ZVAL_RES(return_value,zend_register_resource(result, le_result))
#define TRIE_ZEND_FETCH_RESOURCE(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type) \
        (rsrc = (rsrc_type) zend_fetch_resource(Z_RES_P(*passed_id), resource_type_name, resource_type))
#define TRIE_MAKE_STD_ZVAL(p) zval _stack_zval_##p; p = &(_stack_zval_##p)
#define TRIE_RESOURCE_FREE(resource) zend_list_close(Z_RES_P(resource))
#endif /* PHP Version */

#endif /* EXT_PHP_TRIE_FILTER_PHP7_WRAPPER_H_ */