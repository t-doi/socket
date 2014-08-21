//ソケット通信クラス
//modified 020618 since010706
//参考→http://www.ohmsha.co.jp/
//serverとして使う場合
//local_portにポートをセットしてserver_init(),accept(),recv()or send()
//clientとして使う場合
//connect()して，send() or recv()

#if WIN32
	//#include <winsock.h>
	#include <winsock2.h>
	#include <windows.h>
  
#else
 extern "C"{
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 }
 #define SOCKET int
 #define SOCKADDR_IN  sockaddr_in
#endif

#ifndef MYSOCKH
#define MYSOCKH
#define MYSOCK_BUFSIZE 1024

//---------------------------------------------------------------------------
struct MySock
{
        SOCKET local_sock;//ローカルソケット(for server)
        SOCKET remote_sock;//リモートソケット
        SOCKADDR_IN local_sin;//SOCKADDR_IN構造体
        SOCKADDR_IN remote_sin;//SOCKADDR_IN構造体
        unsigned short local_port;
        char buffer[MYSOCK_BUFSIZE];
        char clientname[32];//接続先のホスト名
        enum Status{Idle, Wait, Connected} stat;
  
        char init(SOCKET *socket_in);
        char server_init();
        char bind(unsigned short port_in);
        char listen(void);
        void close_all();
        char accept(void);
        char recv(void);
        char send(void);
        char send(char *buf_in, int size_in);
        char recv(char *buf_out, int size_out);
        char connect(char *sv_name, unsigned short port_in);
};
#endif
