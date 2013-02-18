#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rpc_client.h"
#include "php.h"
#include "php_ini.h"
#include "Zend/zend_API.h"
#include "Zend/errors.h"
#include "Zend/zend_variables.h"
#include "ext/standard/info.h"

#ifdef COMPILE_DL_RPC
ZEND_GET_MODULE(rpc)
#endif

/**	register(url,callback,timeout)**/

ZEND_BEGIN_ARG_INFO_EX(register_arg_info,0,0,1){
	ZEND_ARG_INFO(0,url)
	ZEND_ARG_INFO(0,callback)
	ZEND_ARG_INFO(0,timeout)
ZEND_END_ARG_INFO();

const zend_function_entry rpc_method[]={
	PHP_ME(Rpc_client,__construct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Rpc_client,__destrory,NULL,ZEND_ACC_PUBLIC|ZEND_ACCDTOR)
	PHP_ME(Rpc_client,register,register_arg_info,ZEND_ACC_PUBLIC)
	PHP_ME(Rpc_client,send,NULL,ZEND_ACC_PUBLIC)
	PHP_FE_END
}
PHP_METHOD(Rpc_client,__construct){

}
PHP_METHOD(Rpc_client,__destroy){

}
PHP_METHOD(Rpc_client,register){
	char *uri;
	zval *callback;
	int timeout;	
	char *name;
	if(zend_parse_parameters_ex(ZEND_NUM_ARGS(),"s|z!l!",&uri,&callback,&timeout)==FAILURE){
		//TODO
		ZVAL_BOOL(return_value,0);
	}
	if(!callback || ZVAL_NULL(callback) || zend_is_callable(callback,0,&name)){
		ZVAL_BOOL(return_value,0);	
	}
	
}
PHP_METHOD(Rpc_client,send){

}

PHP_MINIT_FUNCTION(rpc){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"Rpc_client",rpc_method);
	rpc_client_entry=zend_register_internal_class_ex(&ce,NULL,NULL);
	zend_declare_property_null(&ce,ZEND_STRL("_callstack"),ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	return SUCCESS;
}
PHP_MSHUTDOWN_FUNCTION(rpc){

	return SUCCESS;
}
PHP_RINIT_FUNCTION(rpc){

	return SUCCESS;
}
PHP_RSHUTDOWN_FUNCTION(rpc){

	return SUCCESS;
}
PHP_MINFO_FUNCTION(){
	
	return SUCCESS;
}

