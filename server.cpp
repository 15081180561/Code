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
#pragma comment(lib,"ws2_32.lib")     //���ӿ��ļ�
using namespace std;

list<float> X;
list<float>::iterator X_itr = X.begin();
list<float> Y;
list<float>::iterator Y_itr = Y.begin();
list<float> Z;
list<float>::iterator Z_itr = Z.begin();

char Ip[20][200] = { '\0' };
int iConnect = 0;                     //��ǰ�ͻ�������

//ʹ��thread��ʱ����Ҫʹ��namespace std����Ϊstd::bind()�г�ͻ��
void test_threadpro(void* pParam)           //�������̺߳���;
{
	std::cout << "a new client connected..." << std::endl;
	SOCKET hsock = (SOCKET)pParam;
	char buffer[1024];
	char sendBuffer[1024];
	if (hsock != INVALID_SOCKET)                //INVALID_SOCKET��ʾ��Ч
		std::cout << "Start receive information from IP:" << Ip[iConnect] << std::endl << std::endl;
	while (true) {
		//int recv( SOCKET s, char FAR *buf, int len, int flags);
		/*��һ������ָ�����ն��׽�����������
			�ڶ�������ָ��һ�����������û������������recv�������յ������ݣ�
			����������ָ��buf�ĳ��ȣ�
			���ĸ�����һ����0��
		*/
		int num = recv(hsock, buffer, 1024, 0);
		//��������,�ȴ��������� �����ͣ������
		/*���� <0 ���� =0 ���ӹر� >0 ���յ����ݴ�С��*/
		if (num <= 0) {
			std::cout << "Client with IP:" << Ip[iConnect] << " disconnected!" << std::endl;
			break;
		}
		if (num >= 0)
			std::cout << "Information from:" << Ip[iConnect] << ":" << buffer << std::endl;

		if (!strcmp(buffer, "exit"))
		{
			//������ܵ� exit �����ý���
			std::cout << "Client with IP:" << Ip[iConnect] << " disconnected!" << std::endl;
			std::cout << "Server Process Close:  " << std::endl;
			return;
		}
		else {
			memset(sendBuffer, 0, 1024);
			strcpy(sendBuffer, buffer);
			strcat(sendBuffer, "���");
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
	//��������
	try
	{
		cout << "�������ݿ�" << endl;
		//����ʵ��
		m_conn_ptr.CreateInstance(__uuidof(Recordset));
		m_conn_ptr.CreateInstance("ADODB.Connection");
		//�����ַ���
		_bstr_t strConn = "Provider=OraOLEDB.Oracle.1;Data Source=";
		strConn += "orcl";
		strConn += " ;Persist Security Info=False", "mldn";
		//ʹ��Open�����������ݿ�
		//�����ֱ�Ϊ�������ͣ��˺ţ����룬(��֪����ɶ)��
		m_conn_ptr->Open(strConn, "system", "Yyl001220!", adModeUnknown);
		std::cout << "���ݿ����ӳɹ�" << std::endl;
	}
	catch (const std::exception&)
	{
		std::cout << "���ݿ�����ʧ��" << std::endl;
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
						
			//printf("%s\t%s\t%s\n", (LPSTR)(LPCSTR)(_bstr_t)x, (LPSTR)(LPCSTR)(_bstr_t)y, (LPSTR)(LPCSTR)(_bstr_t)z);    //��ӡ���е�����
			m_res_ptr->MoveNext(); //ָ����һ��
		}
		//copy(X.cbegin(), X.cend(), ostream_iterator<float>(cout, " \n"));	//��ӡlist
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
	
	WSADATA wsd;         //����WSADATA����
	//WSAData������:���windows socket��ʼ����Ϣ���ṹ�����£�
	/*
	 struct WSAData {
		WORD wVersion;
		WORD wHighVersion;
		char szDescription[WSADESCRIPTION_LEN+1];
		char szSystemStatus[WSASYSSTATUS_LEN+1];
		unsigned short iMaxSockets;
		unsigned short iMaxUdpDg;
		char FAR * lpVendorInfo;
		wVersionΪ�㽫ʹ�õ�Winsock�汾�ţ�

wHighVersionΪ�����Winsock��̬��֧�ֵ���߰汾��ע�⣬���ǵĸ��ֽڴ���ΰ汾�����ֽڴ������汾��
szDescription��szSystemStatus���ض��汾��Winsock���ã�ʵ����û��̫���ô���
iMaxSockets��ʾ��������Ĳ���Sockets����ֵ�����ڿ�ʹ�õ�Ӳ����Դ��
iMaxUdpDg��ʾ���ݱ�����󳤶�;Ȼ������ȡ���ݱ�����󳤶ȣ�����Ҫʹ��WSAEnumProtocols��Э����в�ѯ��
��������Ĳ���Sockets������ʲô���������,�����ɿ��õ�������Դ��������.
lpVendorInfo��ΪWinsockʵ�ֶ���������������Ϣ,�����Windowsƽ̨�ϲ�û��ʲô�ô�.
	};
	*/
	//�����WSAStartup����Ϊ�������ϵͳ˵��������Ҫ���ĸ����ļ����øÿ��ļ��뵱ǰ��Ӧ�ó���󶨣��Ӷ��Ϳ��Ե��øð汾��socket�ĸ��ֺ����ˡ�
	/*��������һ������Ҫʹ��2.1�汾��Socket,��ô�����������
wVersionRequested = MAKEWORD( 2, 1 );
MAKEWORD �� ������һ����ָ���������Ӷ��ɵ�WORD����������һ��WORD������
WORD ��16λ�����������Ա�ʾ0-65535֮���������    typedef unsigned short WORD;
err = WSAStartup( wVersionRequested, &wsaData );
	 */
	if (WSAStartup(MAKEWORD(2, 2), &wsd)) 
	{
		printf("Initlalization Error!");
		return -1;
	}

	sockaddr_in serveraddr; //����sockaddr_in���󴢴�������Ϣ(���ж���˿ڣ����Զ����)
	serveraddr.sin_family = AF_INET;             //���÷�������ַ����
	serveraddr.sin_port = htons(4600);           //���÷������˿ں�
	serveraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	//��������Ϊ����������Ϊ�ͻ��˵Ļ���ipΪ127.0.0.1��localhost

	//int socket(int domain, int type, int protocol);Э���� socket���� ָ��Э��0 auto:type
	int  m_SockServer;            //����socket����ӳ�� ������int��  SOCKET    
	m_SockServer = socket(AF_INET, SOCK_STREAM, 0);    //����һ����ʱ��������ֵ��m_SockServer
	//std::cout << m_SockServer << std::endl;

	//int bind( int sockfd, const struct sockaddr addr, socklen_t addrlen)�� sockfdΪsocketӳ�������Ψһȷ��һ��socket
	int i = bind(m_SockServer, (sockaddr*)&serveraddr, sizeof(serveraddr));    //��ӳ���socket��
	std::cout << "bind:" << i << std::endl;//0 ��ʾ�ɹ�

	int iMaxConnect = 20;           //���������
	char buf[] = "Server Connection Successful\0";
	char WarnBuf[] = "It is voer Max connect\0";

	sockaddr_in serveraddrfrom;
	int m_Server[20];  	//����socket������������Կͻ��˵���Ϣ���������Ϊ20
	while (true) 
	{
		//int listen(int sockfd, int backlog);
		//��һ��������ΪҪ������socket�׽��֣��ڶ�������Ϊ��Ӧsocket�����Ŷӵ�������Ӹ�����
		int iLisRet = listen(m_SockServer, 0);    //���м���
		int temp = 0;
		int Len = sizeof(serveraddrfrom);
		int len = sizeof(serveraddr);            //serveraddr��ռ���ֽڴ�С
		//int accept(int sockfd, struct sockaddr addr, socklen_t addrlen);
		//accept�����ĵ�һ������Ϊ��������socket�����֣��ڶ�������Ϊָ��struct sockaddr *��ָ�룬���ڷ��ؿͻ��˵�Э���ַ������������ΪЭ���ַ�ĳ��ȡ�
		m_Server[iConnect] = accept(m_SockServer, (sockaddr*)&serveraddrfrom, &len);

		if (m_Server[iConnect] != INVALID_SOCKET)  //INVALID_SOCKET��ʾ��Ч 
		{
			//int getsockname(int,struct sockaddr addr, *len) -1��ʾ����ʧ��
			if (getsockname(m_Server[iConnect], (struct sockaddr*)&serveraddrfrom, &Len) != -1)
			{   // serveraddrfrom ���ܵ��ͻ��˵�socket��Ϣ
				printf("listen address = %s:%d\n", inet_ntoa(serveraddrfrom.sin_addr), ntohs(serveraddrfrom.sin_port));
				printf(Ip[iConnect], "%s", inet_ntoa(serveraddrfrom.sin_addr));
			}
			else 
			{
				printf("getsockname error\n");
				exit(0);
			}
			//int send (int socket_id,char [],int lens,0);
			int ires = send(m_Server[iConnect], buf, sizeof(buf), 0);   //�����ַ���ȥ
			std::cout << "accept" << ires << std::endl << std::endl;    //��ʾ�Ѿ����Ӵ���                         
			iConnect++;//��ǰ������
			if (iConnect > iMaxConnect) 
			{
				//�ж��������Ƿ������������� 
				int ires = send(m_Server[iConnect], WarnBuf, sizeof(WarnBuf), 0);
			}
			else 
			{
				std::thread th1(test_threadpro, (void*)m_Server[--iConnect]); //�����߳�
				th1.detach();
			}
		}
	}

	WSACleanup();     //�����ͷ�ws2_32.dll��̬���ӿ��ʼ��ʱ�������Դ
}
