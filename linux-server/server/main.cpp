#include <iostream>
#include "Manager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
using namespace std;
#define message "The server is ready!"
#define SendBuf "Server Received!"

int socket_fd;
int client_fd;
int queryCount = 1; //查询次数
char recvBuf[1024]={0}; //接收
int BuildSock()
{
    //1.创建一个socket套接字
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if (socket_fd == -1)
	{
		cout << "Socket Error!" << endl;
		exit(-1);
	}
	cout << "Socket Ready!" << endl;

	//2.sockaddr_in结构体：可以存储一套网络地址（包括IP与端口）,此处存储本机IP地址与本地的一个端口
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8001);  //绑定端口
	local_addr.sin_addr.s_addr = inet_addr("10.20.28.173"); //绑定本机IP地址

	//3.bind()： 将一个网络地址与一个套接字绑定，此处将本地地址绑定到一个套接字上
	int Bind = bind(socket_fd, (struct sockaddr *)&local_addr, sizeof(local_addr)); //成功则返回0 ，失败返回-1
	if (Bind == -1)
	{
		cout << "Bind Error!" << endl;
		return -1;
	}
	cout << "Bind Ready!" << endl;
}

int ListenClient()
{    
    string strSendBuf;
    struct sockaddr_in client_addr;
    socklen_t len=sizeof(client_addr);
    
    client_fd=accept(socket_fd,(struct sockaddr *)&client_addr,&len);    //建立连接
    if (client_fd == -1)
    {
        cout<<"accept错误\n"<<endl;
        //exit(-1);
    }
    else
    {
        char *ip=inet_ntoa(client_addr.sin_addr);
        cout<<"客户端："<<ip<<" 连接到本服务器成功！"<<endl;
        send(client_fd,message,sizeof(message),0);  //发送初始化消息

        while (true)
        {               
            int Recv = recv(client_fd,recvBuf,sizeof(recvBuf),0);   //接受来自客户端的内容
            if (Recv<0)
            {
                cout<<"未接收到客户端的请求，或客户端已断开"<<endl;
                return -1;
            }
            cout<<"*********************************"<<endl;
            cout<<"查询次数:"<<queryCount<<endl;
            cout<<"receive data: "<<recvBuf<<endl;         
            char sendBuf[99999]={0};            
            queryCount++;     
            cout<<"给客户端发送数据中..."<<endl;
            int Write = write(client_fd,sendBuf,strlen(sendBuf));
            if (Write<0)
            {
                cout << "Error: Send info to server failed !"<<endl;
                return -1;
            }
            cout<<"给客户端发送完成!"<<endl;  
            cout<<"*********************************"<<endl;
        }
        
    }
    close(client_fd);
}

int main(int argc,char *argv[])
{
    cout<<"============================="<<endl;
    cout<<"     This is the server      "<<endl;
    cout<<"============================="<<endl;
       
    BuildSock();
    listen(socket_fd,10);
    while (1)
    {
        cout<<"等待客户端的连接..."<<endl;
        ListenClient();
    }
       
    close(socket_fd);
    return 0;
}
