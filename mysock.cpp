//#define WIN32 1

#include "mysock.h"                                                   
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if WIN32
 #define SOCKLEN_T int
 #define  close(_z) closesocket(_z)
#else
 extern "C"{
 #include <unistd.h>
 }
 #define INVALID_SOCKET -1
 #define LPSOCKADDR sockaddr*
 #define SOCKET_ERROR -1
 #define SOCKLEN_T socklen_t
#endif
//---------------------------------------------------------------------------
char MySock::init(SOCKET *socket_in)
{
//�\�P�b�g����

#if WIN32
//WinSock�̏��������s������ 
        WORD wVersionRequested;
        int  nErrorStatus;
        WSADATA wsaData;

        wVersionRequested = MAKEWORD(1,1); // �o�[�W���� 1.1 ��v������
        nErrorStatus = WSAStartup(wVersionRequested,&wsaData);
        if(atexit((void (*)(void))(WSACleanup))){
          // �I������WinSock�̃��\�[�X���������悤�ɂ��Ă���
                fprintf(stderr,"Error: atexit(WSACleanup)���s\n");
                //ShowMessage("WSACleanup error");
                return 0;
        }
        if(nErrorStatus != 0){
                fprintf(stderr,"Error: WinSock�̏��������s\n");
                //ShowMessage("������ error");
                return 0;
        }
#endif


    *socket_in=INVALID_SOCKET;
    *socket_in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    *socket_in = socket(AF_INET, SOCK_STREAM, 0);  
    if(*socket_in == INVALID_SOCKET){        //�\�P�b�g�쐬���s
        printf("Error");
	//        closesocket(*socket_in);
        ::close(*socket_in);
        return 0;
    }
  stat=Idle;
return 1;
}
//---------------------------------------------------------------------------
char MySock::server_init()
{
//�T�[�o�[�\�P�b�g����
init(&local_sock);
//�o�C���h
    if(!bind(local_port))return 0;
//�ڑ��҂�
    if(!listen())return 0;//���ꂪ�Ȃ���OS��������
return 1;
}
//---------------------------------------------------------------------------
char MySock::connect(char *sv_name, unsigned short port_in)
{
//�N���C�A���g�Ƃ��ăT�[�o�[�ɐڑ�
        char svName[64];
        //sprintf(svName,sv_name);//�T�[�o�[�̖��O
        sprintf(svName,"%s",sv_name);//�T�[�o�[�̖��O
        //strcpy(sv_name,svName);//�T�[�o�[�̖��O
        unsigned long serveraddr;               /* �T�[�o��IP�A�h���X */
        struct hostent *serverhostent;  /* �T�[�o�̃z�X�g�����w���|�C���^ */

        /* svName�Ƀh�b�g�ŋ�؂���10�i����IP�A�h���X�������Ă���ꍇ�A
             serveraddr��32bit������IP�A�h���X���Ԃ�܂� */
        serveraddr = inet_addr((char*)svName);
        if (serveraddr == -1) {
                /* �T�[�o��(svName)����T�[�o�̃z�X�g�����擾���܂� */
                serverhostent = gethostbyname(svName);
                if (serverhostent == NULL) {
                        //ShowMessage("�z�X�g�A�h���X�擾���s");
                        return 0;
                }else{
                        /* �T�[�o�̃z�X�g��񂩂�IP�A�h���X��serveraddr�ɃR�s�[���܂� */
                        serveraddr = *((unsigned long *)((serverhostent->h_addr_list)[0]));
                }
        }


        if(!init(&remote_sock))return 0;

    memset(&remote_sin, 0x00, sizeof(remote_sin));              //�\���̏�����
    remote_sin.sin_family      = AF_INET;                   //�C���^�[�l�b�g
    remote_sin.sin_port        = htons(port_in);               //�|�[�g�ԍ��w��
    remote_sin.sin_addr.s_addr = serveraddr;         //�A�h���X�w��
    stat=Wait;
    int connect_flag=::connect(remote_sock,(LPSOCKADDR)&remote_sin,sizeof(remote_sin));
    stat=Connected;
    if(connect_flag==SOCKET_ERROR)return 0;
    else if(connect_flag==0)return 1;
return 0;
}
//---------------------------------------------------------------------------
char MySock::bind(unsigned short port_in)
{
//�o�C���h
local_port=port_in;
    memset((char *)&local_sin, 0, sizeof(local_sin));              //�\���̏�����
    local_sin.sin_family      = AF_INET;                   //�C���^�[�l�b�g
    local_sin.sin_port        = htons(local_port);               //�|�[�g�ԍ��w��
    local_sin.sin_addr.s_addr = htonl(INADDR_ANY);         //�A�h���X�w��

//    if(::bind(local_sock, (struct LPSOCKADDR)&local_sin, sizeof(local_sin))==SOCKET_ERROR)
    if(::bind(local_sock, (sockaddr *)&local_sin, sizeof(local_sin))==SOCKET_ERROR)
      {      
      //closesocket(local_sock);
        ::close(local_sock);
        local_sock = INVALID_SOCKET;
        return 0;
    }
return 1;
}
//---------------------------------------------------------------------------
char MySock::listen(void)
{
//�ڑ��҂�����
//    if(::listen(local_sock, 5)==SOCKET_ERROR)
    if(::listen(local_sock, 1)==SOCKET_ERROR)
      {
      //�ڑ��҂����s
      //        closesocket(local_sock);
        ::close(local_sock);
        local_sock = INVALID_SOCKET;
        return 0;
    }
return 1;
}
//---------------------------------------------------------------------------
char MySock::accept(void)
{
SOCKLEN_T len;

stat=Wait;
//remote_sock=::accept(local_sock,(struct LPSOCKADDR)&remote_sin,&len);
remote_sock=::accept(local_sock,NULL,NULL);    

  if(remote_sock==INVALID_SOCKET)return 0;//Caption="�ڑ��󂯓��ꎸ�s";

len = sizeof(remote_sin);
  getpeername(remote_sock, (sockaddr *)&remote_sin, &len);
  strcpy(clientname,inet_ntoa(remote_sin.sin_addr));
stat=Connected;
 return 1;//Caption="�ڑ�����";

}
//---------------------------------------------------------------------------
char MySock::recv(char *buf_out, int size_out)
{
int recvsize=::recv(remote_sock,buf_out,size_out,0);
 if(recvsize==SOCKET_ERROR)
   {
     stat=Idle;
     return 0;
   }
 if(recvsize==0)
 {
 	return 0;
 }
 return 1;
}
//---------------------------------------------------------------------------
char MySock::recv(void)
{
char flag=recv(buffer,MYSOCK_BUFSIZE);
return flag;
}
//---------------------------------------------------------------------------
char MySock::send(char *buf_in, int size_in)
{
int sendsize=::send(remote_sock,buf_in,size_in,0);
 if((sendsize==SOCKET_ERROR)||(sendsize==0))
  {
    stat=Idle;
    return 0;
  }
return 1;
}
//---------------------------------------------------------------------------
char MySock::send(void)
{
char flag=send(buffer,MYSOCK_BUFSIZE);
  return flag;
}
//---------------------------------------------------------------------------
void MySock::close_all()
{
#if WIN32
::closesocket(local_sock);
::closesocket(remote_sock);
shutdown(local_sock,2);
#else
::close(local_sock);
::close(remote_sock);
#endif
  stat=Idle;
}
//---------------------------------------------------------------------------

