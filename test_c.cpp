//socket test
//130912
#include "mysock.h"
#include <stdio.h>
char server[64];
int main()
{
  MySock ms1;
  
  sprintf(server,"127.0.0.1");//�T�[�o�[��
  printf("Client\n");
  printf("Connect TO - Server[%s]\n\n",server);

//connect()���āCsend() or recv()
//  ms1.connect("localhost", 5321);
  ms1.connect(server, 5321);//���肽����̃T�[�o�[���C�|�[�g������

  for(int i=0;i<3;i++)//�R�񑗐M���Ă݂�
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
