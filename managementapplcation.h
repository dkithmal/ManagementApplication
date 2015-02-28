#ifndef MANAGEMENTAPPLCATION_H
#define MANAGEMENTAPPLCATION_H

#include <QWidget>
#include <QTcpServer>
#include <QMap>
#include <QString>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QFileInfo>
#include <QModelIndex>
#include <QSettings>
#include <QProgressDialog>
#include "initialconnectingtread.h"
#include "ui_managementapplcation.h"
#include "serverfindingthread.h"


#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#else
#include <time.h>
#endif


class ManagementApplcation : public QWidget
{
    Q_OBJECT
    
public:
    explicit ManagementApplcation(QWidget *parent = 0);
    ~ManagementApplcation();
	void StartUP();
    
private:
    Ui::ManagementApplcation *ui;
    QMap<QString,QString> serverApplications;
    QTcpSocket *socket;
	QTcpSocket *serchSocket;
    QString mCommandBuffer;
	QMap<QString,QString> imageList;
	QSettings* settings;
	QString ipRange;
	QProgressDialog *pd;
	ServerFindingThread *serverFindingThread;
    void ProcessCommand(QString ClientCommand);
    void ExecuteCommand(QString ClientCommand);
	void ConnectToServerApplications(QString ip);
	void DoLIST(QString List);
	void loadConfigs();
	void FindServerApplications();


public slots:
    void readyRead();
    void bytesWritten(qint64 bytes);
    void disconnected();
    void connected();
    void SendResponse(QString data);
	void addImagesClicked();
	void removeImagesClicked();
	void uploadImagesClicked();
	void controlPanelConnectedListClicked(QModelIndex index);
	void deleteSelectedImagesButtonClicked();
	void setDefaultImageButtonClicked();
	void showOnlyDefaultButtonClicked();
	void showSlideShowButtonClicked();
	void ServerreadyRead(QString data);
	
};

#endif // MANAGEMENTAPPLCATION_H
