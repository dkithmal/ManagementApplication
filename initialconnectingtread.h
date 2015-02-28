#ifndef INITIALCONNECTINGTREAD_H
#define INITIALCONNECTINGTREAD_H

#include <QThread>
#include <QTcpSocket>


class InitialConnectingTread : public QThread
{
	Q_OBJECT

public:
	InitialConnectingTread(QString ipRange,QObject *parent);
	~InitialConnectingTread();
	
	QTcpSocket *socket;
	

protected:
	void run();

private:
	QString ipAddress;
	void ConnectToServerApplications(QString ip);

	
	
};

#endif // INITIALCONNECTINGTREAD_H
