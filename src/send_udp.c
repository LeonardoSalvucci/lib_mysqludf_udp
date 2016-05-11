#ifdef STANDARD
   /* STANDARD is defined, don't use any mysql functions */
   #include <stdlib.h>
   #include <stdio.h>
   #include <string.h>
   #ifdef __WIN__
   typedef unsigned __int64 ulonglong;/* Microsofts 64 bit types */
   typedef __int64 longlong;
   #else
   typedef unsigned long long ulonglong;
   typedef long long longlong;
   #endif /*__WIN__*/
   #else
   #include <my_global.h>
   #include <my_sys.h>
   #if defined(MYSQL_SERVER)
   #include <m_string.h>/* To get strmov() */
   #else
   /* when compiled as standalone */
   #include <string.h>
   #define strmov(a,b) stpcpy(a,b)
   #define bzero(a,b) memset(a,0,b)
   #define memcpy_fixed(a,b,c) memcpy(a,b,c)
   #endif
   #endif
   #include <mysql.h>
   #include <ctype.h>
   
   #ifdef HAVE_DLOPEN
   
   #if !defined(HAVE_GETHOSTBYADDR_R) || !defined(HAVE_SOLARIS_STYLE_GETHOST)
   static pthread_mutex_t LOCK_hostname;
   #endif
   
   #include <math.h>
   
   /* needed included for send udp */
   #include<stdio.h>
	#include<sys/socket.h>
	#include<arpa/inet.h>
   
   my_bool send_udp_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
   void send_udp_deinit(UDF_INIT *initid __attribute__((unused)));
   int send_udp(UDF_INIT* initid, UDF_ARGS* args __attribute__((unused)),
                     char* is_null __attribute__((unused)), char* error __attribute__((unused)));
   
   my_bool send_udp_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
   {
     if(!(args->arg_count == 3)) {
       strcpy(message, "Expected three arguments (ip, port, message)");
       return 1;
     }
   
     args->arg_type[0] = STRING_RESULT;
     args->arg_type[1] = INT_RESULT;
     args->arg_type[2] = STRING_RESULT;
   
     return 0;
   }
   
   void send_udp_deinit(UDF_INIT *initid __attribute__((unused)))
   {
   
   }
   
   int send_udp(UDF_INIT* initid, UDF_ARGS* args __attribute__((unused)),
                     char* is_null __attribute__((unused)), char* error __attribute__((unused)))
   {
     char *serverIP = ((char *)(args->args[0]));
     int port = *((int *)(args->args[1]));
     char *message2send = ((char *)(args->args[2]));
     struct sockaddr_in server;
     int socket_desc;
   
	socket_desc = socket(AF_INET,SOCK_DGRAM,0);
	if(socket_desc==-1){
		strcpy(error,"Could not create socket");
		return 1;
	}
	
	server.sin_addr.s_addr = inet_addr(serverIP);
	server.sin_family = AF_INET;
	server.sin_port=htons(port);
	
	if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		strcpy(error,"Connect Error");
		return 1;
	}
	
	if (send(socket_desc, message2send, strlen(message2send),0) < 0) {
		strcpy(error,"Send Failed");
		return 1;
	}
	return 0;

   }
   
   #endif /* HAVE_DLOPEN */
