#ifndef PHP_RPC_H
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/select.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
//#include <php_smart_str.h>
#define PHP_RPC_EPOLL 
#define MAX_EPOLL_EVENTS 50
#ifdef PHP_RPC_EPOLL
	#include <sys/epoll.h>
#else
	#include <sys/types.h>
#endif
	typedef struct _php_epoll_data_t{
		int epoll_fd;
		int still;
		CURLM *cm;
	}php_epoll_data_t;

typedef struct _php_rpc_curl_list php_rpc_curl_list;

#define PHP_RPC_LIST_INIT(desc) do{\
		(desc)=(php_rpc_curl_list*)malloc(sizeof(php_rpc_curl_list));	\
		(desc)->next=NULL;\
		(desc)->curl_p=NULL;\
}while(0);

#define PHP_RPC_LIST_FIND(desc,sh,p) do{\
		(p)=(desc);\
		while(p && p->curl_p!=sh){\
			p=p->next;\
		}\
}while(0);

#define PHP_RPC_LIST_APPEND(desc,src) do{\
	if((desc)){\
		while(desc!=NULL){\
			desc=desc->next;\
		}\
		desc->curl_p=src;\
		desc->next=NULL;\
	}\
}while(0);

/*
#define PHP_RPC_LIST_DESTROY(desc) do{\
	if((desc)){\
		PHP_RPC_LIST_DESTROY((desc->next));\
		free(desc);\
		(desc)->next=NULL;\
	}\
}while(0);
*/
typedef struct _php_rpc_curl_t php_rpc_curl_t;
typedef struct _php_rpc_curl_multi_t php_rpc_curl_multi_t;

typedef struct _php_rpc_data_t{
	CURL *cp;
	int len;
	char *url;
	char *buf;
}php_rpc_data_t;

struct _php_rpc_curl_t{
	void *data;	
	void (*open)(php_rpc_curl_t *,char *url,int timeout);	
	void (*setopt)(php_rpc_curl_t *,CURLoption op,void *val);
	void (*exec)(php_rpc_curl_t *);
};
typedef struct _php_rpc_curl_multi_data_t{
	CURLM *cm;
	php_rpc_curl_list *list;
}php_rpc_curl_multi_data_t;

struct _php_rpc_curl_multi_t{
	void *data;
	void (*add)(php_rpc_curl_multi_t *,php_rpc_curl_t*);
	void (*exec)(php_rpc_curl_multi_t *);
	void (*close)(php_rpc_curl_multi_t *curl_multi_t);
};

struct _php_rpc_curl_list{
	php_rpc_curl_t *curl_p;
	php_rpc_curl_list *next;
};
php_rpc_curl_t *php_rpc_curl_init();
void php_rpc_curl_open(php_rpc_curl_t *curl_t,char *url,int timeout);
void php_rpc_curl_setopt(php_rpc_curl_t *,CURLoption op,void *val);
void php_rpc_curl_exec(php_rpc_curl_t *);

size_t php_rpc_write_handle(char *buf,size_t len,size_t nmemp,void *userp);

/**multi**/
void php_rpc_curl_multi_init(php_rpc_curl_multi_t **);
void php_rpc_curl_multi_add(php_rpc_curl_multi_t *,php_rpc_curl_t*);
void php_rpc_curl_multi_exec(php_rpc_curl_multi_t *);
void php_rpc_curl_multi_destroy(php_rpc_curl_multi_t *curl_multi_t);

void php_rpc_list_destroy(php_rpc_curl_list *list);

int php_rpc_multi_socket(CURL *easy, /*  easy handle */   curl_socket_t s, /*  socket */   int action, /*  see values below */   void *userp, /*  private callback pointer */   void *socketp); 

#endif //PHP_RPC_H
