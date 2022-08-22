#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <mifsa/ota/client.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QFrame;
class QLabel;
class QTimer;
class QPushButton;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Mifsa::Ota::Detail)
Q_DECLARE_METATYPE(Mifsa::Ota::DetailMessage)

class QTreeWidgetItem;
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void processVariant(const QVariant& data, bool stateChanged);
    void processDetail(const Mifsa::Ota::DetailMessage& detail, bool stateChanged);
    void updateProperty(QTreeWidgetItem* listItem);

private slots:
    void on_actionQuit_Q_triggered();
    void on_actionAbout_A_triggered();
    void on_actionAboutQt_T_triggered();

private:
    QTreeWidgetItem* updateSubProperty(QTreeWidgetItem* item, const QString& property, const QVariant& value, bool useProgress = false);

private:
    Ui::MainWindow* ui = nullptr;
    QLabel* m_messageLabel = nullptr;
    QPushButton* m_acceptBtn = nullptr;
    QPushButton* m_rejectBtn = nullptr;
    QFrame* m_messageLine = nullptr;
    QWidget* m_statusFlag = nullptr;
    QTimer* m_flagTimer = nullptr;
    QTimer* m_messageTimer = nullptr;
    QMap<QTreeWidgetItem*, Mifsa::Ota::Detail> m_itemToDetail;
};
#endif // MAINWINDOW_H
