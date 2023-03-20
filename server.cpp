#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<thread>
#include<winsock2.h>   
#include "server.h"
#include<assert.h>
#include<stdio.h>
#include<Windows.h>
#include<list>
#pragma comment(lib,"ws2_32.lib")     //链接库文件
using namespace std;

list<float> X;
list<float>::iterator X_itr = X.begin();
list<float> Y;
list<float>::iterator Y_itr = Y.begin();
list<float> Z;
list<float>::iterator Z_itr = Z.begin();

char Ip[20][200] = { '\0' };
int iConnect = 0;                     //当前客户端数量

//使用thread库时，不要使用namespace std，因为std::bind()有冲突；
void test_threadpro(void* pParam)           //创建多线程函数;
{
	std::cout << "a new client connected..." << std::endl;
	SOCKET hsock = (SOCKET)pParam;
	char buffer[1024];
	char sendBuffer[1024];
	if (hsock != INVALID_SOCKET)                //INVALID_SOCKET表示无效
		std::cout << "Start receive information from IP:" << Ip[iConnect] << std::endl << std::endl;
	while (true) {
		//int recv( SOCKET s, char FAR *buf, int len, int flags);
		/*第一个参数指定接收端套接字描述符；
			第二个参数指明一个缓冲区，该缓冲区用来存放recv函数接收到的数据；
			第三个参数指明buf的长度；
			第四个参数一般置0。
		*/
		int num = recv(hsock, buffer, 1024, 0);
		//阻塞函数,等待接受内容 程序会停留在这
		/*都是 <0 出错 =0 连接关闭 >0 接收到数据大小。*/
		if (num <= 0) {
			std::cout << "Client with IP:" << Ip[iConnect] << " disconnected!" << std::endl;
			break;
		}
		if (num >= 0)
			std::cout << "Information from:" << Ip[iConnect] << ":" << buffer << std::endl;

		if (!strcmp(buffer, "exit"))
		{
			//如果接受到 exit 结束该进程
			std::cout << "Client with IP:" << Ip[iConnect] << " disconnected!" << std::endl;
			std::cout << "Server Process Close:  " << std::endl;
			return;
		}
		else {
			memset(sendBuffer, 0, 1024);
			strcpy(sendBuffer, buffer);
			strcat(sendBuffer, "完成");
			int ires = send(hsock, sendBuffer, sizeof(sendBuffer), 0);
			std::cout << "The message sent to IP:" << Ip[iConnect] << "is: " << sendBuffer << std::endl;
		}
	}
	return;
};

bool Server::Conn_DB()
{
	HRESULT hr = CoInitialize(NULL);
	assert(SUCCEEDED(hr));
	//尝试连接
	try
	{
		cout << "连接数据库" << endl;
		//创建实例
		m_conn_ptr.CreateInstance(__uuidof(Recordset));
		m_conn_ptr.CreateInstance("ADODB.Connection");
		//连接字符串
		_bstr_t strConn = "Provider=OraOLEDB.Oracle.1;Data Source=";
		strConn += "orcl";
		strConn += " ;Persist Security Info=False", "mldn";
		//使用Open方法连接数据库
		//参数分别为数据类型，账号，密码，(不知道是啥)；
		m_conn_ptr->Open(strConn, "system", "Yyl001220!", adModeUnknown);
		std::cout << "数据库连接成功" << std::endl;
	}
	catch (const std::exception&)
	{
		std::cout << "数据库连接失败" << std::endl;
	}
	return TRUE;
}
bool Server::ReadData()
{
	_RecordsetPtr m_res_ptr;
	m_res_ptr.CreateInstance("ADODB.Recordset");
	_variant_t x, y, z;
	try
	{
		m_res_ptr->Open(_variant_t("select * from cpp_oracle_test"),
			m_conn_ptr.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);
		while (!m_res_ptr->adoEOF)
		{
			x = m_res_ptr->GetCollect(_variant_t("x"));
			y = m_res_ptr->GetCollect(_variant_t("y"));
			z = m_res_ptr->GetCollect(_variant_t("z"));

			X.insert(X_itr, x);
			Y.insert(Y_itr, y);
			Z.insert(Z_itr, z);
						
			//printf("%s\t%s\t%s\n", (LPSTR)(LPCSTR)(_bstr_t)x, (LPSTR)(LPCSTR)(_bstr_t)y, (LPSTR)(LPCSTR)(_bstr_t)z);    //打印表中的数据
			m_res_ptr->MoveNext(); //指向下一行
		}
		//copy(X.cbegin(), X.cend(), ostream_iterator<float>(cout, " \n"));	//打印list
	}
	catch (const exception&)
	{
		cout << "error" << endl;
		return FALSE;
	}

	return TRUE;
}

int main(void)
{
	/*Server* ser=new Server();
	ser->Conn_DB();
	ser->ReadData();*/
	
	WSADATA wsd;         //定义WSADATA对象
	//WSAData功能是:存放windows socket初始化信息。结构体如下：
	/*
	 struct WSAData {
		WORD wVersion;
		WORD wHighVersion;
		char szDescription[WSADESCRIPTION_LEN+1];
		char szSystemStatus[WSASYSSTATUS_LEN+1];
		unsigned short iMaxSockets;
		unsigned short iMaxUdpDg;
		char FAR * lpVendorInfo;
		wVersion为你将使用的Winsock版本号，

wHighVersion为载入的Winsock动态库支持的最高版本，注意，它们的高字节代表次版本，低字节代表主版本。
szDescription与szSystemStatus由特定版本的Winsock设置，实际上没有太大用处。
iMaxSockets表示最大数量的并发Sockets，其值依赖于可使用的硬件资源。
iMaxUdpDg表示数据报的最大长度;然而，获取数据报的最大长度，你需要使用WSAEnumProtocols对协议进行查询。
最大数量的并发Sockets并不是什么神奇的数字,它是由可用的物理资源来决定的.
lpVendorInfo是为Winsock实现而保留的制造商信息,这个在Windows平台上并没有什么用处.
	};
	*/
	//这里的WSAStartup就是为了向操作系统说明，我们要用哪个库文件，让该库文件与当前的应用程序绑定，从而就可以调用该版本的socket的各种函数了。
	/*例：假如一个程序要使用2.1版本的Socket,那么程序代码如下
wVersionRequested = MAKEWORD( 2, 1 );
MAKEWORD 宏 ：创建一个被指定变量连接而成的WORD变量。返回一个WORD变量。
WORD 是16位短整数，可以表示0-65535之间的整数；    typedef unsigned short WORD;
err = WSAStartup( wVersionRequested, &wsaData );
	 */
	if (WSAStartup(MAKEWORD(2, 2), &wsd)) 
	{
		printf("Initlalization Error!");
		return -1;
	}

	sockaddr_in serveraddr; //创建sockaddr_in对象储存自身信息(当有多个端口，可以多个绑定)
	serveraddr.sin_family = AF_INET;             //设置服务器地址家族
	serveraddr.sin_port = htons(4600);           //设置服务器端口号
	serveraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	//本机既作为服务器有作为客户端的话，ip为127.0.0.1或localhost

	//int socket(int domain, int type, int protocol);协议域 socket类型 指定协议0 auto:type
	int  m_SockServer;            //创建socket对象映射 本质是int型  SOCKET    
	m_SockServer = socket(AF_INET, SOCK_STREAM, 0);    //创建一个临时变量并赋值给m_SockServer
	//std::cout << m_SockServer << std::endl;

	//int bind( int sockfd, const struct sockaddr addr, socklen_t addrlen)； sockfd为socket映射参数，唯一确定一个socket
	int i = bind(m_SockServer, (sockaddr*)&serveraddr, sizeof(serveraddr));    //把映射和socket绑定
	std::cout << "bind:" << i << std::endl;//0 表示成功

	int iMaxConnect = 20;           //最大连接数
	char buf[] = "Server Connection Successful\0";
	char WarnBuf[] = "It is voer Max connect\0";

	sockaddr_in serveraddrfrom;
	int m_Server[20];  	//创建socket数组来存放来自客户端的信息最大连接数为20
	while (true) 
	{
		//int listen(int sockfd, int backlog);
		//第一个参数即为要监听的socket套接字，第二个参数为相应socket可以排队的最大连接个数。
		int iLisRet = listen(m_SockServer, 0);    //进行监听
		int temp = 0;
		int Len = sizeof(serveraddrfrom);
		int len = sizeof(serveraddr);            //serveraddr所占的字节大小
		//int accept(int sockfd, struct sockaddr addr, socklen_t addrlen);
		//accept函数的第一个参数为服务器的socket描述字，第二个参数为指向struct sockaddr *的指针，用于返回客户端的协议地址，第三个参数为协议地址的长度。
		m_Server[iConnect] = accept(m_SockServer, (sockaddr*)&serveraddrfrom, &len);

		if (m_Server[iConnect] != INVALID_SOCKET)  //INVALID_SOCKET表示无效 
		{
			//int getsockname(int,struct sockaddr addr, *len) -1表示创建失败
			if (getsockname(m_Server[iConnect], (struct sockaddr*)&serveraddrfrom, &Len) != -1)
			{   // serveraddrfrom 接受到客户端的socket信息
				printf("listen address = %s:%d\n", inet_ntoa(serveraddrfrom.sin_addr), ntohs(serveraddrfrom.sin_port));
				printf(Ip[iConnect], "%s", inet_ntoa(serveraddrfrom.sin_addr));
			}
			else 
			{
				printf("getsockname error\n");
				exit(0);
			}
			//int send (int socket_id,char [],int lens,0);
			int ires = send(m_Server[iConnect], buf, sizeof(buf), 0);   //发送字符过去
			std::cout << "accept" << ires << std::endl << std::endl;    //显示已经连接次数                         
			iConnect++;//当前连接数
			if (iConnect > iMaxConnect) 
			{
				//判断连接数是否大于最大连接数 
				int ires = send(m_Server[iConnect], WarnBuf, sizeof(WarnBuf), 0);
			}
			else 
			{
				std::thread th1(test_threadpro, (void*)m_Server[--iConnect]); //启动线程
				th1.detach();
			}
		}
	}

	WSACleanup();     //用于释放ws2_32.dll动态链接库初始化时分配的资源
}
