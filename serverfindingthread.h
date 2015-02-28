#ifndef SERVERFINDINGTHREAD_H
#define SERVERFINDINGTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QString>

class ServerFindingThread : public QThread
{
	Q_OBJECT

public:
	ServerFindingThread(QString ip,QObject *parent);
	~ServerFindingThread();
	QTcpSocket *socket;
	QString ipAddress;
protected:
	void run();


public slots:
		void readyRead();
		void connected();

signals:
		void treadReadyRead(QString value);



	
};

#endif // SERVERFINDINGTHREAD_H
