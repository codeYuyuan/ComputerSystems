#include <stdio.h>
#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define CACHE_OBJS_COUNT 10
#define LRU_MAX 9999
/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *conn_hdr = "Connection: close\r\n";
static const char *prox_hdr = "Proxy-Connection: close\r\n";
static const char *host_hdr_format = "Host: %s\r\n";
static const char *requestlint_hdr_format = "Get %s HTTP/1.0\r\n";
static const char *endof_hdr = "\r\n";
static const char *connection_key = "Connection";
static const char *user_agent_key = "User-Agent";
static const char *proxy_connection_key = "Proxy-Connection";
static const char *host_key = "Host";

void doit(int connfd);
void parse_url(char *url, char *hostname, char *path, int *port);
void build_http_header(char *http_header, char *hostname, char *path, int port, rio_t *client_rio);
int connect_endServer(char *hostname, int port, char *http_header);
void *thread(void *vargp);
/*cache functions*/
void cache_init();
int cache_find(char *url);
int cache_eviction();
void cache_writer(char *url, char *buf);
void cache_reader(int i);

typedef struct{
    char cache_contents[MAX_OBJECT_SIZE];
    char cache_url[MAXLINE];
    int LRU;
    int isEmpty;

    int readCnt;
    sem_t mutex_readCnt;
    sem_t mutex_writer;
}cache_block;

typedef struct{
    cache_block cacheobjs[CACHE_OBJS_COUNT];
    int cache_num;
}Cache;

Cache cache;

int main(int argc, char **argv){
    int listenfd, connfd;
    socklen_t clientlen;
    char hostname[MAXLINE], port[MAXLINE];
    pthread_t tid;
    struct sockaddr_storage clientaddr;
    cache_init();
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    Signal(SIGPIPE,SIG_IGN);
    listenfd = Open_listenfd(argv[1]);
    while(1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, port,MAXLINE,0);
        printf("Accepted connection from (%s %s).\n",hostname, port);
        Pthread_create(&tid,NULL,thread,(void *)connfd);
    }
    return 0;
}

void *thread(void *vargp){
    Pthread_detach(pthread_self());
    int connfd = (int)vargp;
    doit(connfd);
    Close(connfd);
}

/*handle http request from client*/
void doit(int connfd){
    int end_serverfd;
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char endserver_http_header[MAXLINE];
    char hostname[MAXLINE], path[MAXLINE];
    int port;

    rio_t client_rio, server_rio;
    Rio_readinitb(&client_rio, connfd);
    Rio_readlineb(&client_rio, buf, MAXLINE);
    sscanf(buf,"%s %s %s", method, url ,version);  //read client request line
    char url_backup[100];
    strcpy(url_backup,url);
    if(strcasecmp(method, "GET")){
        printf("%s is not supported\n",method);
        return;
    }
    int cache_index = cache_find(url_backup);
    //printf("%d\n",cache_index);
    if(cache_index!=-1){
        cache_reader(cache_index);
        Rio_writen(connfd,cache.cacheobjs[cache_index].cache_contents,strlen(cache.cacheobjs[cache_index].cache_contents));
        return;
    }
    /*parse url*/
    parse_url(url, hostname,path,&port);

    /*build http header for server*/
    build_http_header(endserver_http_header, hostname, path, port,&client_rio);

    /*connect to server*/
    end_serverfd = connect_endServer(hostname, port, endserver_http_header);
    if(end_serverfd<0){
        printf("connection failed\n");
        return;
    }
    Rio_readinitb(&server_rio, end_serverfd);
    Rio_writen(end_serverfd, endserver_http_header, strlen(endserver_http_header));

    /*receive message from server, send it to client*/
    char cachebuf[MAX_OBJECT_SIZE];
    int sizebuf;
    size_t n;
    while((n = Rio_readlineb(&server_rio,buf,MAXLINE))!=0){
        sizebuf+=n;
        if(sizebuf<MAX_OBJECT_SIZE)
            strcat(cachebuf,buf);
        Rio_writen(connfd,buf,n);
    }
    Close(end_serverfd);
    //printf("===Size buf %d\n",sizebuf);
    if(sizebuf<MAX_OBJECT_SIZE){
        cache_writer(url_backup, cachebuf);
    }
}

void build_http_header(char *http_header, char *hostname, char *path, int port, rio_t *client_rio){
    char buf[MAXLINE], request_hdr[MAXLINE], other_hdr[MAXLINE],host_hdr[MAXLINE];
    /*request line*/
    sprintf(request_hdr, requestlint_hdr_format, path);
    while(Rio_readlineb(client_rio,buf,MAXLINE)>0){
        if(!strcmp(buf,endof_hdr))
            break;
        if(!strncasecmp(buf, host_key, strlen(host_key))){
            strcpy(host_hdr,buf);
            continue;
        }
        if(!strncasecmp(buf,connection_key, strlen(connection_key))||!strncasecmp(buf,proxy_connection_key,strlen(proxy_connection_key))||!strncasecmp(buf,user_agent_key, strlen(user_agent_key))){
            strcat(other_hdr,buf);
        }
        if(!strlen(host_hdr)){
            sprintf(host_hdr,host_hdr_format,hostname);
        }
        sprintf(http_header,"%s%s%s%s%s%s%s",
                request_hdr,
                host_hdr,
                conn_hdr,
                prox_hdr,
                user_agent_hdr,
                other_hdr,
                endof_hdr);
        return;
    }
}
int connect_endServer(char *hostname, int port, char *http_header){
    char portStr[100];
    sprintf(portStr, "%d",port);
    return Open_clientfd(hostname,portStr);
}

    /*parse url, get hostname, file path, port*/
void parse_url(char *url, char *hostname, char *path, int *port){
    *port = 80;
    char *pos = strstr(url,"//");
    pos = pos!=NULL?pos+2:url;
    char *pos2 = strstr(pos,":");
    if(pos2!=NULL){
        *pos2 = '\0';
        sscanf(pos, "%s",hostname);
        sscanf(pos2+1,"%d%s",port,path);
    }else{
        pos2 = strstr(pos,"/");
        if(pos2!=NULL){
            *pos2 = '\0';
            sscanf(pos,"%s",hostname);
            *pos2 = '/';
            sscanf(pos2,"%s",path);
        }else{
            sscanf(pos,"%s",hostname);
        }
    }
    return;
}

void cache_init(){
    cache.cache_num = 0;
    int i;
    for(i = 0; i<CACHE_OBJS_COUNT; i++){
        cache.cacheobjs[i].LRU = 0;
        cache.cacheobjs[i].isEmpty = 1;
        cache.cacheobjs[i].readCnt = 0;
        Sem_init(&cache.cacheobjs[i].mutex_readCnt, 0 ,1);
        Sem_init(&cache.cacheobjs[i].mutex_writer, 0 , 1);
    }
}

int cache_find(char *url){
    int i;
    for(i = 0 ; i < CACHE_OBJS_COUNT; i++){
        if(!cache.cacheobjs[i].isEmpty&&!strcmp(url,cache.cacheobjs[i].cache_url))
            return i;
    }
    return -1;
}

/*if not in cache, find empty index or evict one if full*/
int cache_eviction(){
    //printf("===evicting...\n");
    int min = LRU_MAX;
    int i;
    int index = 0;
    for(i = 0; i < CACHE_OBJS_COUNT; i++){
        if(cache.cacheobjs[i].isEmpty){
          //  printf("====Eviction index: %d\n", i);
            index = i;
            break;
        }
        if(cache.cacheobjs[i].LRU<min){
            min = cache.cacheobjs[i].LRU;
            index = i;
        }
    }
    return index;
}

void cache_reader(int i){
    P(&cache.cacheobjs[i].mutex_readCnt);
    cache.cacheobjs[i].readCnt++;
    cache.cacheobjs[i].LRU+=1;
    if(cache.cacheobjs[i].readCnt == 1)
        P(&cache.cacheobjs[i].mutex_writer);
    V(&cache.cacheobjs[i].mutex_readCnt);
    P(&cache.cacheobjs[i].mutex_readCnt);
    cache.cacheobjs[i].readCnt--;
    if(cache.cacheobjs[i].readCnt == 0)
        V(&cache.cacheobjs[i].mutex_writer);
    V(&cache.cacheobjs[i].mutex_readCnt);
}

void cache_writer(char *url, char* buf){
    //printf("===writing to cache\n");
    int i = cache_eviction();
    P(&cache.cacheobjs[i].mutex_writer);
    strcpy(cache.cacheobjs[i].cache_contents,buf);
    strcpy(cache.cacheobjs[i].cache_url, url);
    cache.cacheobjs[i].isEmpty = 0;
    V(&cache.cacheobjs[i].mutex_writer);
}
