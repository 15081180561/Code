#pragma once
#include<iostream>

//µº»Îadoø‚
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

class Server
{
public:
	Server() {};
	~Server();

private:

public:
	_ConnectionPtr m_conn_ptr;

	bool Conn_DB();
	bool ReadData();

};

