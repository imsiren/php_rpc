PHP_ARG_WITH(php_rpc,
[--with-php_rpc		Include rpc support])

if test "$PHP_RPC"!="no"; then

	if test -r $PHP_CURL/include/curl/easy.y; then
		CURL_DIR=$PHP_CURL;
	else
		for i in /usr/local /usr; do
			AC_MSG_RESULT(find lib curl path);
			if test -r $i/include/curl/easy.h;then
				CURL_DIR=$i;
				AC_MSG_RESULT(we found it in $i);	
			fi;
		done;	
	fi
	if test -z "$CURL_DIR"; then
		AC_MSG_RESULT(not found libcurl);
		AC_MSG_ERROR(please reinstall the libcurl);
	fi;

PHP_ADD_INCLUDE($CURL_DIR/include)
PHP_EVAL_LIBLINE($CURL_LIBS,RPC_SHARED_LIBADD)
PHP_ADD_LIBRARY_WITH_PATH(curl,$CURL_DIR/$PHP_LIBDIR,RPC_SHARED_LIBADD)

PHP_NEW_EXTENSION(rpc,php_rpc.c,php_rpc.h,$ext_shared)
PHP_SUBST(RPC_SHARED_LIBADD)

fi
