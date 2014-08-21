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
//ソケット生成

#if WIN32
//WinSockの初期化を行う処理 
        WORD wVersionRequested;
        int  nErrorStatus;
        WSADATA wsaData;

        wVersionRequested = MAKEWORD(1,1); // バージョン 1.1 を要求する
        nErrorStatus = WSAStartup(wVersionRequested,&wsaData);
        if(atexit((void (*)(void))(WSACleanup))){
          // 終了時にWinSockのリソースを解放するようにしておく
                fprintf(stderr,"Error: atexit(WSACleanup)失敗\n");
                //ShowMessage("WSACleanup error");
                return 0;
        }
        if(nErrorStatus != 0){
                fprintf(stderr,"Error: WinSockの初期化失敗\n");
                //ShowMessage("初期化 error");
                return 0;
        }
#endif


    *socket_in=INVALID_SOCKET;
    *socket_in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    *socket_in = socket(AF_INET, SOCK_STREAM, 0);  
    if(*socket_in == INVALID_SOCKET){        //ソケット作成失敗
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
//サーバーソケット生成
init(&local_sock);
//バインド
    if(!bind(local_port))return 0;
//接続待ち
    if(!listen())return 0;//これがないとOSが落ちる
return 1;
}
//---------------------------------------------------------------------------
char MySock::connect(char *sv_name, unsigned short port_in)
{
//クライアントとしてサーバーに接続
        char svName[64];
        //sprintf(svName,sv_name);//サーバーの名前
        sprintf(svName,"%s",sv_name);//サーバーの名前
        //strcpy(sv_name,svName);//サーバーの名前
        unsigned long serveraddr;               /* サーバのIPアドレス */
        struct hostent *serverhostent;  /* サーバのホスト情報を指すポインタ */

        /* svNameにドットで区切った10進数のIPアドレスが入っている場合、
             serveraddrに32bit整数のIPアドレスが返ります */
        serveraddr = inet_addr((char*)svName);
        if (serveraddr == -1) {
                /* サーバ名(svName)からサーバのホスト情報を取得します */
                serverhostent = gethostbyname(svName);
                if (serverhostent == NULL) {
                        //ShowMessage("ホストアドレス取得失敗");
                        return 0;
                }else{
                        /* サーバのホスト情報からIPアドレスをserveraddrにコピーします */
                        serveraddr = *((unsigned long *)((serverhostent->h_addr_list)[0]));
                }
        }


        if(!init(&remote_sock))return 0;

    memset(&remote_sin, 0x00, sizeof(remote_sin));              //構造体初期化
    remote_sin.sin_family      = AF_INET;                   //インターネット
    remote_sin.sin_port        = htons(port_in);               //ポート番号指定
    remote_sin.sin_addr.s_addr = serveraddr;         //アドレス指定
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
//バインド
local_port=port_in;
    memset((char *)&local_sin, 0, sizeof(local_sin));              //構造体初期化
    local_sin.sin_family      = AF_INET;                   //インターネット
    local_sin.sin_port        = htons(local_port);               //ポート番号指定
    local_sin.sin_addr.s_addr = htonl(INADDR_ANY);         //アドレス指定

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
//接続待ち準備
//    if(::listen(local_sock, 5)==SOCKET_ERROR)
    if(::listen(local_sock, 1)==SOCKET_ERROR)
      {
      //接続待ち失敗
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

  if(remote_sock==INVALID_SOCKET)return 0;//Caption="接続受け入れ失敗";

len = sizeof(remote_sin);
  getpeername(remote_sock, (sockaddr *)&remote_sin, &len);
  strcpy(clientname,inet_ntoa(remote_sin.sin_addr));
stat=Connected;
 return 1;//Caption="接続成功";

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

