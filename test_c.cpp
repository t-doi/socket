//socket test
//130912
#include "mysock.h"
#include <stdio.h>
char server[64];
int main()
{
  MySock ms1;
  
  sprintf(server,"127.0.0.1");//サーバー名
  printf("Client\n");
  printf("Connect TO - Server[%s]\n\n",server);

//connect()して，send() or recv()
//  ms1.connect("localhost", 5321);
  ms1.connect(server, 5321);//送りたい先のサーバー名，ポートを入れる

  for(int i=0;i<3;i++)//３回送信してみる
{
  sprintf(ms1.buffer,"Hello! TCP/IP[%d]",i);
  printf("send[%s]",ms1.buffer);
  ms1.send(ms1.buffer,MYSOCK_BUFSIZE);
  //ms1.send(ms1.buffer,64);
getchar();
}

  ms1.close_all();
printf("done.\n");
getchar();
  return 0;
}
