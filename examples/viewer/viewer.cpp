/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <QMetaMethod>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Mifsa::Ota::Client client(argc, argv);
    MainWindow window;
    client.subscibeDetail([&window](const Mifsa::Ota::DetailMessage& detail, bool stateChanged) {
        QMetaObject::invokeMethod(&window, "processVariant",
            Qt::QueuedConnection,
            Q_ARG(QVariant, QVariant::fromValue<Mifsa::Ota::DetailMessage>(detail)),
            Q_ARG(bool, stateChanged));
    });
    client.asyncExec(Mifsa::Ota::Client ::CHECK_SINGLETON);
    window.show();
    Mifsa::Ota::DetailMessage detailMessage;
    detailMessage.state = Mifsa::Ota::MR_OFFLINE;
    window.processDetail(detailMessage, true);
    int reval = a.exec();
    client.exit(reval);
    return reval;
}
