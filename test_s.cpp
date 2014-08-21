//socket test
//130912
#include <stdio.h>
//#include <unistd.h>//fork?sleep?
//#include <pthread.h>
#include "mysock.h"


char buf[65535];
char *buf_ptr;


//-------------
char get_data()
{
  MySock ms1;
  char res;

   printf("Server\n");
//local_portにポートをセットしてserver_init(),accept(),recv()or send()
  ms1.local_port=5321;
  printf("port=%d\n",ms1.local_port);

  if(ms1.server_init()!=1)
    {
      printf("socket init error.\n");
      return -1;
    }
  if(ms1.accept()!=1)
    {
      printf("socket accept error.\n");
      return -1;
    }
  printf("Client name=%s\n", ms1.clientname);

while(1)
{
  res=ms1.recv(buf_ptr,MYSOCK_BUFSIZE);
//  res=ms1.recv(buf_ptr,64);
     if(res==1)
     {
          printf("[%s]res=%d\n",buf_ptr,res);
     }

	  if(ms1.stat==ms1.Idle)
    	 {
       printf("Disconnected.\n");
       break;
       ;
     	}
     	
     //Sleep(1);
}
  ms1.close_all();
return 1;
}
//---------------------
int main()
{
	buf_ptr=buf;
 get_data();
   return 0;

}
