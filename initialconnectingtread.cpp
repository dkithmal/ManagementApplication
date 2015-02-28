#include "initialconnectingtread.h"

InitialConnectingTread::InitialConnectingTread(QString ipRange,QObject *parent)
	: QThread(parent)
{
	//this->ipAddress=ip;
	socket= new QTcpSocket();
	//this->socket=socket;
	
}

InitialConnectingTread::~InitialConnectingTread()
{

}


void InitialConnectingTread::run()
{
	
	socket->connectToHost(ipAddress,1111);
	if(!socket->waitForConnected(5000))
	{
		qDebug()<<"Error:"<<socket->errorString();
	}

	socket->write("INIT 192.168.1.103\n");
	socket->waitForBytesWritten(5000);

}

void InitialConnectingTread::ConnectToServerApplications(QString ip)
{
	socket = new QTcpSocket(this);

	connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

	socket->connectToHost(ip,1111);

	if(!socket->waitForConnected(5000))
	{
		qDebug()<<"Error:"<<socket->errorString();
	}

}