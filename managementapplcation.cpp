#include "managementapplcation.h"
#include "ui_managementapplcation.h"

ManagementApplcation::ManagementApplcation(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ManagementApplcation)
{   
	ui->setupUi(this);


	settings= new QSettings("settings.ini",QSettings::IniFormat);
	loadConfigs();

	ui->tWManagementPanel->setStyleSheet("QTabBar::tab { height: 30px; width: 120px; }");
	ui->lWConnectedList->setSelectionMode(QAbstractItemView::MultiSelection);
	ui->lWImageList->setSelectionMode(QAbstractItemView::MultiSelection);
	ui->lWImageList_2->setSelectionMode(QAbstractItemView::MultiSelection);

}

ManagementApplcation::~ManagementApplcation()
{
	delete ui;
}

void ManagementApplcation::loadConfigs()
{
	if(!(settings->value("ipRange").toString().isEmpty()))
	{
		ipRange=settings->value("ipRange").toString();

	}
	else
	{
		ipRange="127.0.0.1";

	}
}


void ManagementApplcation::StartUP()
{
	FindServerApplications();

}




void ManagementApplcation::FindServerApplications()
{
	if(ipRange!="127.0.0.1")
	{
		QString itprifix=ipRange.split(' ').at(0).mid(0,ipRange.split(' ').at(0).lastIndexOf('.'));
		int start=ipRange.split(' ').at(0).mid(ipRange.split(' ').at(0).lastIndexOf('.')+1,ipRange.split(' ').at(0).length()-ipRange.split(' ').at(0).lastIndexOf('.')).toInt();
		int end=ipRange.split(' ').at(1).mid(ipRange.split(' ').at(1).lastIndexOf('.')+1,ipRange.split(' ').at(1).length()-ipRange.split(' ').at(1).lastIndexOf('.')).toInt();

		for (int x=start;x<end;x++)
		{
			serverFindingThread = new ServerFindingThread("192.168.1."+QString::number(x),this);
			connect(serverFindingThread,SIGNAL(treadReadyRead(QString)),this,SLOT(ServerreadyRead(QString)));
			serverFindingThread->start();

		}

	}
	else
	{
		serverFindingThread = new ServerFindingThread("127.0.0.1",this);
		connect(serverFindingThread,SIGNAL(treadReadyRead(QString)),this,SLOT(ServerreadyRead(QString)));
		serverFindingThread->start();

	}



}


void ManagementApplcation::ServerreadyRead(QString data)
{
	QString serverName=data.trimmed();
	ui->lWConnectedList->addItem(serverName);
	ui->lWConnectedList_2->addItem(serverName);

}


void ManagementApplcation::ConnectToServerApplications(QString ip)
{
	socket = new QTcpSocket(this);
	connect(socket,SIGNAL(connected()),this,SLOT(connected()));
	connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
	connect(socket,SIGNAL(bytesWritten(qint64 )),this,SLOT(bytesWritten(qint64 )));
	connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

	socket->connectToHost(ip,1111);

	if(!socket->waitForConnected(5000))
	{
		qDebug()<<"Error:"<<socket->errorString();
	}

}



void ManagementApplcation::connected()
{

}

void ManagementApplcation::disconnected()
{
	qDebug()<<"disconnected";
}



void ManagementApplcation::bytesWritten(qint64 bytes)
{
	qDebug()<<"we wrote:"<<bytes;

}


void ManagementApplcation::readyRead()
{

	//Append to the command buffer
	mCommandBuffer.append(socket->readAll());

	if(mCommandBuffer.endsWith('\n'))
	{
		ProcessCommand(mCommandBuffer);
		qDebug()<<mCommandBuffer;
		mCommandBuffer.clear();
	}

}

void ManagementApplcation::ProcessCommand(QString ClientCommand)
{
	QStringList Commands = ClientCommand.split("\n");

	foreach(QString Command, Commands)
	{
		QString ClientCommand = Command.trimmed();
		if(ClientCommand != "")
		{
			//Execute the command
			ExecuteCommand(ClientCommand);
		}
	}
}




void ManagementApplcation::ExecuteCommand(QString ClientCommand)
{
	QString Command = "";
	QString Arg = "";

	if(ClientCommand.contains(" "))
	{
		//Contains arquments
		int pos = ClientCommand.indexOf(" ");
		Command = ClientCommand.mid(0,pos).trimmed();
		Arg = ClientCommand.mid(pos + 1).trimmed();
	}
	else
	{
		//No arquments
		Command = ClientCommand.trimmed();
	}
	qDebug() << "Client: " << Command << " " << Arg;


	if (Command=="LIST")
	{

		DoLIST(Arg);
	}

}

void ManagementApplcation::SendResponse(QString data)
{
	QByteArray datasend;
	datasend = data.toLatin1();
	socket->write(datasend);
	socket->waitForBytesWritten(5000);
}



void ManagementApplcation::DoLIST(QString List)
{
	QStringList imagelist=List.split(',');

	for(int x=0;x<imagelist.size()-1;x++)
	{
		ui->lWImageList_2->addItem(imagelist.at(x));
	}

	socket->disconnectFromHost();
	socket->waitForDisconnected();


}


void ManagementApplcation::addImagesClicked()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();

	for (int x=0;x<fileNames.length();x++)
	{
		QFileInfo fi( fileNames.at(x));
		QString fileName = fi.fileName(); 
		if(!imageList.contains(fileName))
		{
			imageList.insert(fileName,fileNames.at(x));
			ui->lWImageList->addItem(fileName);
		}
		else
		{
			QMessageBox::information(this,"Error",fileName+"already exists in list",0);

		}
	}


}

void ManagementApplcation::removeImagesClicked()
{
	QList<QListWidgetItem *> list=ui->lWImageList->selectedItems();
	for(int x=0;x<list.length();x++)
	{
		imageList.remove(list.at(x)->data(0).toString());
		qDebug()<<list.at(x)->data(0).toString();
		delete list.at(x);

	}

}

void ManagementApplcation::uploadImagesClicked()
{
	QList<QListWidgetItem *> list=ui->lWConnectedList->selectedItems();
	for(int x=0;x<list.length();x++)
	{
		QString ip=list.at(x)->data(0).toString().split("@").at(0);

		QMap<QString, QString>::iterator i;
		for (i = imageList.begin(); i != imageList.end(); ++i)
		{

			ConnectToServerApplications(ip);

			QString imagepath=i.value();
			QFile file(i.value());
			file.open(QIODevice::ReadOnly);
			SendResponse("NEWIMAGE "+i.key()+"NAMEEND");
			socket->waitForBytesWritten(5000);
			while (!file.atEnd()) 
			{
				QByteArray buffer = file.read(1024 * 8);
				socket->write(buffer);
				socket->waitForBytesWritten(5000);
			}
			SendResponse("FILEEND");
			file.close();

			socket->disconnectFromHost();
			socket->waitForDisconnected();


		}
	}

	QMessageBox::information(this,"INFO","IMAGES UPLOAD DONE",0);




}

void ManagementApplcation::controlPanelConnectedListClicked(QModelIndex index)
{
	ui->lWImageList_2->clear();
	if(ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").size()>1)
	{
		QString ipaddress=ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").at(0);
		ConnectToServerApplications(ipaddress);
		SendResponse("LIST\n");

	}	


}





void ManagementApplcation::deleteSelectedImagesButtonClicked()
{

	QString imageListToDelete;
	QList<QListWidgetItem *> list=ui->lWImageList_2->selectedItems();
	for(int x=0;x<list.length();x++)
	{
		imageListToDelete.append(list.at(x)->data(0).toString());
		if(x!=(list.length()-1))
			imageListToDelete.append(',');
		delete list.at(x);

	}

	qDebug()<<imageListToDelete;



	if(ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").size()>1)
	{
		QString ipaddress=ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").at(0);
		ConnectToServerApplications(ipaddress);
		SendResponse("DELETE "+imageListToDelete+'\n');

	}	


}


void ManagementApplcation::setDefaultImageButtonClicked()
{
	QList<QListWidgetItem *> list=ui->lWImageList_2->selectedItems();

	if(list.size()!=1)
	{
		QMessageBox::information(this,"Error","Please reselect Only one default image ");

	}
	else
	{


		if(ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").size()>1)
		{
			QString ipaddress=ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").at(0);
			ConnectToServerApplications(ipaddress);
			SendResponse("SETDEFAULT "+list.at(0)->data(0).toString()+'\n');

		}		

	}


}


void ManagementApplcation::showOnlyDefaultButtonClicked()
{
	if(ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").size()>1)
	{
		QString ipaddress=ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").at(0);
		ConnectToServerApplications(ipaddress);
		SendResponse("SHOWDEFAULT \n");

	}


}


void ManagementApplcation::showSlideShowButtonClicked()
{
	if(ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").size()>1)
	{
		QString ipaddress=ui->lWConnectedList_2->selectedItems().at(0)->data(0).toString().split("@").at(0);
		ConnectToServerApplications(ipaddress);
		SendResponse("SHOWSLIDESHOW \n");

	}

}