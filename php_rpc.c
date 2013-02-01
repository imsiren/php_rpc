#include "php_rpc.h"
/************************CURL EASY********************/
void php_rpc_curl_init(php_rpc_curl_t **curl_t){
	//if(*curl_t==NULL){
		(*curl_t)=(php_rpc_curl_t*)malloc(sizeof(php_rpc_curl_t));
		(*curl_t)->open=php_rpc_curl_open;
		(*curl_t)->setopt=php_rpc_curl_setopt;
		(*curl_t)->exec=php_rpc_curl_exec;


		php_rpc_data_t *data=NULL;
		CURL *cp=NULL;
		data=(php_rpc_data_t*)malloc(sizeof(php_rpc_data_t));
		(*curl_t)->data=data;
		cp=curl_easy_init();
		data->cp=cp;
		{
			data->url="";
			data->buf=NULL;

			curl_easy_setopt(data->cp,CURLOPT_WRITEFUNCTION,php_rpc_write_handle);
			curl_easy_setopt(data->cp,CURLOPT_WRITEDATA,&data);	
		}
//	}			
}
void php_rpc_curl_destroy(php_rpc_curl_t *curl_t){
	php_rpc_data_t *data=curl_t->data;
	curl_easy_cleanup(data->cp);	
	free(data);
	free(curl_t);
}

void php_rpc_curl_open(php_rpc_curl_t *curl_t,char *url,int timeout){
	php_rpc_data_t *data=curl_t->data;
	curl_easy_setopt(data->cp,CURLOPT_URL,url);
	curl_easy_setopt(data->cp,CURLOPT_CONNECTTIMEOUT,timeout);
	//curl_easy_setopt(data->cp,CURLOPT_CONNECTTIMEOUT_MS,timeout);
}

void php_rpc_curl_setopt(php_rpc_curl_t *curl_t,CURLoption op,void *val){

}
void php_rpc_curl_exec(php_rpc_curl_t *curl_t){
	php_rpc_data_t *data=curl_t->data;
	CURLcode code=curl_easy_perform(data->cp);
	if(code!=CURLE_OK){
		printf("curl easy perform failed:%s\n",curl_easy_strerror(code));
	}
}
/************************CURL MULTI********************/
void php_rpc_curl_multi_init(php_rpc_curl_multi_t **curl_multi_t){
	if(*curl_multi_t==NULL){
		(*curl_multi_t)=(php_rpc_curl_multi_t*)malloc(sizeof(php_rpc_curl_multi_t));
		(*curl_multi_t)->add	=php_rpc_curl_multi_add;
		(*curl_multi_t)->exec	=php_rpc_curl_multi_exec;
		(*curl_multi_t)->close=php_rpc_curl_multi_destroy;

		php_rpc_curl_multi_data_t *data=(php_rpc_curl_multi_data_t*)malloc(sizeof(php_rpc_curl_multi_data_t));
		(*curl_multi_t)->data=data;
		data->cm=curl_multi_init();
		PHP_RPC_LIST_INIT(data->list);
	}
}

void php_rpc_curl_multi_add(php_rpc_curl_multi_t *curl_multi_t,php_rpc_curl_t *curl_t){
	php_rpc_curl_multi_data_t* curl_multi_data=(php_rpc_curl_multi_data_t*)curl_multi_t->data;
	php_rpc_data_t *curl_data=curl_t->data;
	CURLcode code=curl_multi_add_handle(curl_multi_data->cm,curl_data->cp);
	if(code!=CURLE_OK){
		printf("curl error:%s\n",curl_easy_strerror(code));
	}
}

void php_rpc_curl_multi_destroy(php_rpc_curl_multi_t *curl_multi_t){
	php_rpc_curl_multi_data_t* curl_multi_data=(php_rpc_curl_multi_data_t*)curl_multi_t->data;
	curl_multi_cleanup(curl_multi_data->cm);
	php_rpc_list_destroy(curl_multi_data->list);
}
void php_rpc_curl_multi_exec(php_rpc_curl_multi_t *curl_multi_t){
	php_rpc_curl_multi_data_t* curl_multi_data=(php_rpc_curl_multi_data_t*)curl_multi_t->data;
	int still,ret_count;
	struct timeval tv;
	tv.tv_sec=1;
	tv.tv_usec=0;
	#ifdef PHP_RPC_EPOLL
		struct epoll_event ev,*events;//[MAX_EPOLL_EVENTS];	
		int epoll_fd,nfds;
		php_epoll_data_t *epoll_data;

		events=(struct epoll_event*)malloc(sizeof(struct epoll_event)*MAX_EPOLL_EVENTS);
		epoll_data=(php_epoll_data_t*)malloc(sizeof(php_epoll_data_t));
		epoll_data->cm=curl_multi_data->cm;

		epoll_fd=epoll_create(500);
		if(epoll_fd==-1){
			perror("epoll_create");
		}
		epoll_data->epoll_fd=epoll_fd;

		curl_multi_setopt(curl_multi_data->cm,CURLMOPT_SOCKETFUNCTION,php_rpc_multi_socket);
		curl_multi_setopt(curl_multi_data->cm,CURLMOPT_SOCKETDATA,epoll_data);
		while(CURLM_CALL_MULTI_PERFORM==curl_multi_socket_all(curl_multi_data->cm,&still));
	#else
		while(CURLM_CALL_MULTI_PERFORM==curl_multi_perform(curl_multi_data->cm,&still));
	#endif
		ret_count=still;
	while(still){
		#ifdef PHP_RPC_EPOLL
			int i=0;
			nfds=epoll_wait(epoll_fd,events,MAX_EPOLL_EVENTS,500);
			for(i=0;i<nfds;i++){
				if(events[i].events & EPOLLIN){
					while(CURLM_CALL_MULTI_PERFORM==curl_multi_socket_all(curl_multi_data->cm,&still));
				}
			}
		#else
			int max_fd,ret_fd;
			fd_set read_fds,write_fds,except_fds;
			
			FD_ZERO(&read_fds);
			FD_ZERO(&write_fds);
			FD_ZERO(&except_fds);
			curl_multi_fdset(curl_multi_data->cm,&read_fds,&write_fds,&except_fds,&max_fd);
			int ret_code=select(max_fd+1,&read_fds,&write_fds,&except_fds,&tv);
			while(CURLM_CALL_MULTI_PERFORM==curl_multi_perform(curl_multi_data->cm,&still));
		#endif	
		int queue=0;	
		CURLMsg *msg=NULL;
		while((msg=curl_multi_info_read(curl_multi_data->cm,&queue))){
			if(msg==NULL){

			}
		}

	}
}
int php_rpc_multi_socket(CURL *easy, /*  easy handle */   curl_socket_t s, /*  socket */   int action, /*  see values below */   void *userp, /*  private callback pointer */   void *socketp){
	php_epoll_data_t *epoll_data=(php_epoll_data_t*)userp;
	struct epoll_event ev;
	ev.data.ptr=NULL;
	if(action==CURL_POLL_REMOVE){
		--epoll_data->still;
		ev.data.fd=s;
		ev.events=EPOLLIN;
		epoll_ctl(epoll_data->epoll_fd,EPOLL_CTL_DEL,s,&ev);
	}else{
		ev.data.fd=s;
		ev.events=EPOLLIN;
		epoll_ctl(epoll_data->epoll_fd,EPOLL_CTL_ADD,s,&ev);
		curl_multi_assign(epoll_data->cm,s,socketp);
	}
	return 0;
  }

/************************CURL MULTI END********************/

size_t php_rpc_write_handle(char *buf,size_t size,size_t nmemp,void *userp){
	size_t len=size*nmemp;
	printf("%s\n",buf);
	return len;
}
void php_rpc_list_destroy(php_rpc_curl_list *list){
	if(list){
		php_rpc_list_destroy(list->next);
		free(list);
		list->next=NULL;
	}
}


int main(int argc,char **argv){

	char url[]="http://www.imsiren.com";
	php_rpc_curl_multi_t *curl_multi_t=NULL;
	php_rpc_curl_multi_init(&curl_multi_t);

	php_rpc_curl_t *curl_t=NULL;
	php_rpc_curl_init(&curl_t);
	curl_t->open(curl_t,url,3);
	curl_multi_t->add(curl_multi_t,curl_t);
/*
	php_rpc_curl_t *curl_t;
	php_rpc_curl_init(&curl_t);
	curl_t->open(curl_t,url,10);
	curl_t->exec(curl_t);
	php_rpc_curl_destroy(curl_t);
*/
//	php_rpc_curl_t *curl_a=NULL;
	
	char url2[]="http://www.baidu.com";
	php_rpc_curl_init(&curl_t);
	curl_t->open(curl_t,url2,3);
	curl_multi_t->add(curl_multi_t,curl_t);

	curl_multi_t->exec(curl_multi_t);
	curl_multi_t->close(curl_multi_t);
	return 0;
}
