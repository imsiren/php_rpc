
#ifndef PHP_RPC_H
#define PHP_RPC_H

extern zend_module_entry rpc_module_entry;
#define phpext_rpc_ptr &rpc_module_entry;

#ifdef PHP_WIN32
	#define PHP_RPC_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >=4
	#define PHP_RPC_API __attribute__(visibility("default"))
#else
	#define PHP_RPC_API 
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(rpc);
PHP_MSHUTDOWN_FUNCTION(rpc);
PHP_RINIT_FUNCTION(rpc);
PHP_RSHUTDOWN_FUNCTION(rpc);
PHP_MINFO_FUNCTION(rpc);

/** class Rpc_client**/
PHP_METHOD(Rpc_client,__construct);
PHP_METHOD(Rpc_client,__destroy);
PHP_METHOD(Rpc_client,register);
PHP_METHOD(Rpc_client,send);

extern zend_class_entry rpc_client_entry;


#endif PHP_RPC_H
