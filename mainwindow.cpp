#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QKeySequence>
#include <QMenu>
#include <QMessageBox>
#include <QShortcut>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QTimer>

#include <Windows.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_isClosing(false) {
    ui->setupUi(this);
    this->setWindowTitle("Coil Whine Be Gone " + kVersion);

    ui->stopPushButton->setVisible(false);
    ui->statusbar->showMessage(tr("Status: Stopped"));

    connect(new QShortcut(QKeySequence("Ctrl+Q"), this), &QShortcut::activated, &QApplication::quit);
    connect(ui->startPushButton, &QPushButton::clicked, [&]() {
        m_cpuCycleBurner.start();
        ui->startPushButton->setVisible(false);
        ui->stopPushButton->setVisible(true);
        ui->statusbar->showMessage(tr("Status: Running"));
    });
    connect(ui->stopPushButton, &QPushButton::clicked, [&]() {
        m_cpuCycleBurner.stop();
        ui->startPushButton->setVisible(true);
        ui->stopPushButton->setVisible(false);
        ui->statusbar->showMessage(tr("Status: Stopped"));
    });
    connect(ui->utilizationPercentageSlider, &QSlider::valueChanged, [this](int value) {
        ui->utilizationPercentageSpinBox->setValue(value);
    });
    connect(ui->utilizationPercentageSlider, &QSlider::sliderReleased, [this]() {
        m_cpuCycleBurner.setUtilizationPercent(ui->utilizationPercentageSlider->value());
    });

    // Set the initial utilization percentage
    ui->utilizationPercentageSlider->setValue(35);

    // Set the current process's priority to Low
    SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(this, tr("System Tray"), tr("System tray is not available on this system."));
    } else {
        createTrayIcon();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setVisible(bool visible) {
    if (m_restoreAction) {
        m_restoreAction->setEnabled(!visible);
    }
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // Set flag to prevent minimize-to-tray behavior
    m_isClosing = true;

    // Ensure the application actually closes when X button is clicked
    event->accept();
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    qApp->quit();
}

void MainWindow::changeEvent(QEvent* event) {
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && m_trayIcon && m_trayIcon->isVisible() && !m_isClosing) {
            // Hide to system tray when minimized (but not when closing)
            hide();
            if (m_trayIcon->supportsMessages()) {
                m_trayIcon->showMessage(
                    tr("CoilWhineBeGone"), tr("Application minimized to tray"), QSystemTrayIcon::Information, 2000
                );
            }
        }
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (isVisible() && !isMinimized()) {
                hide();
            } else {
                showNormal();
                raise();
                activateWindow();
            }
            break;
        default:
            break;
    }
}

void MainWindow::createTrayIcon() {
    m_trayIconMenu = new QMenu(this);
    m_restoreAction = m_trayIconMenu->addAction(tr("&Restore"), this, &QWidget::showNormal);
    m_trayIconMenu->addSeparator();
    m_quitAction = m_trayIconMenu->addAction(tr("&Quit"), qApp, &QCoreApplication::quit);

    m_trayIcon = new QSystemTrayIcon(this);

    // Try to load custom icon, fallback to system icon if it fails
    QIcon icon = QIcon(":/coilwhinebegone.ico");
    if (icon.isNull()) {
        icon = style()->standardIcon(QStyle::SP_ComputerIcon);
    }

    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip(tr("CoilWhineBeGone"));
    m_trayIcon->setContextMenu(m_trayIconMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    m_trayIcon->show();
}
