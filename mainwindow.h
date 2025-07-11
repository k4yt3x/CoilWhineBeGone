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
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
  void closeEvent(QCloseEvent *event) override;
  void changeEvent(QEvent *event) override;

private:
  void createTrayIcon();
  void setVisible(bool visible) override;
  Ui::MainWindow *ui;
  QSystemTrayIcon *m_trayIcon;
  QMenu *m_trayIconMenu;
  QAction *m_restoreAction;
  QAction *m_quitAction;
  CpuCycleBurner m_cpuCycleBurner;
  bool m_isClosing;
};

const QString kVersion = "1.1.0";

#endif // MAINWINDOW_H
