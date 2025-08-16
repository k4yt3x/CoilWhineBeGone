#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    // Set application metadata for proper settings storage
    QCoreApplication::setOrganizationName("CoilWhineBeGone");
    QCoreApplication::setApplicationName("CoilWhineBeGone");
    QCoreApplication::setApplicationVersion(kVersion);

    // Set application icon (serves as fallback for tray icon)
    a.setWindowIcon(QIcon(":/resources/coilwhinebegone.ico"));

    // Don't quit when the last window is closed (needed for system tray)
    QApplication::setQuitOnLastWindowClosed(false);

    // Check if system tray is available before showing the main window
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(
            nullptr, QObject::tr("System Tray"), QObject::tr("Unable to detect any system tray on this system.")
        );
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
