#include "serverfindingthread.h"

ServerFindingThread::ServerFindingThread(QString ip,QObject *parent)
	: QThread(parent)
{
	ipAddress=ip;
}

ServerFindingThread::~ServerFindingThread()
{

}


void ServerFindingThread::run()
{
	socket = new QTcpSocket(this);
	connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
	connect(socket,SIGNAL(connected()),this,SLOT(connected()));

	socket->connectToHost(ipAddress,1111);
	if(!socket->waitForConnected(5000))
	{
		qDebug()<<"Error:"<<socket->errorString();
	}


	exec();
	
}

void ServerFindingThread::connected()
{
	QString command="INIT "+ipAddress+"\n";
	socket->write(command.toLatin1());
	socket->waitForBytesWritten(5000);
}

void ServerFindingThread::readyRead()
{
	QString data=socket->readAll();
	emit treadReadyRead(data);

}

