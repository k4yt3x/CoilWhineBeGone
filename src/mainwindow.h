#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCloseEvent>
#include <QEvent>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>

#include "cpucycleburner.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void startCpuBurner();
    void stopCpuBurner();
    void updateTrayStatus();

   protected:
    void closeEvent(QCloseEvent* event) override;

   private:
    void createTrayIcon();
    void setVisible(bool visible) override;
    bool isTrayAvailable() const;

    Ui::MainWindow* ui;
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayIconMenu;
    bool m_trayIconHintShown = false;
    bool m_trayAvailable = false;
    QAction* m_restoreAction;
    QAction* m_quitAction;
    QAction* m_statusAction;
    QAction* m_startAction;
    QAction* m_stopAction;
    CpuCycleBurner m_cpuCycleBurner;
    bool m_isClosing;
};

const QString kVersion = "1.4.0";

#endif  // MAINWINDOW_H
