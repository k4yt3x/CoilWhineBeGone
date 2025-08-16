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

#include "settingsmanager.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_trayIcon(nullptr),
      m_trayIconMenu(nullptr),
      m_trayIconHintShown(false),
      m_trayAvailable(false),
      m_restoreAction(nullptr),
      m_quitAction(nullptr),
      m_isClosing(false) {
    ui->setupUi(this);
    this->setWindowTitle("Coil Whine Be Gone " + kVersion);

    // Check if system tray is available
    m_trayAvailable = QSystemTrayIcon::isSystemTrayAvailable();

    // Initialize settings manager
    SettingsManager::instance().loadSettings();

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

    // Connect closeToTray checkbox to settings
    connect(ui->closeToTrayCheckBox, &QCheckBox::toggled, [this](bool checked) {
        SettingsManager::instance().setCloseToTray(checked);
    });

    // Connect minimizeNotify checkbox to settings
    connect(ui->minimizeNotifyCheckBox, &QCheckBox::toggled, [this](bool checked) {
        SettingsManager::instance().setMinimizeNotify(checked);
    });

    // Set the initial utilization percentage
    ui->utilizationPercentageSlider->setValue(35);

    // Initialize UI state from settings
    ui->closeToTrayCheckBox->setChecked(SettingsManager::instance().closeToTray());
    ui->minimizeNotifyCheckBox->setChecked(SettingsManager::instance().minimizeNotify());

    // Disable closeToTray checkbox if system tray is not available
    if (!m_trayAvailable) {
        ui->closeToTrayCheckBox->setEnabled(false);
        ui->closeToTrayCheckBox->setToolTip(tr("System tray is not available on this system"));
        ui->minimizeNotifyCheckBox->setEnabled(false);
        ui->minimizeNotifyCheckBox->setToolTip(tr("System tray is not available on this system"));
    }

    // Set the current process's priority to Low
    SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

    // Create tray icon if system tray is available
    if (m_trayAvailable) {
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
    if (SettingsManager::instance().closeToTray() && m_trayAvailable && m_trayIcon && m_trayIcon->isVisible()) {
        // Hide to system tray instead of closing
        hide();
        if (m_trayIcon->supportsMessages() && !m_trayIconHintShown && SettingsManager::instance().minimizeNotify()) {
            m_trayIcon->showMessage(
                tr("CoilWhineBeGone"), tr("Application was minimized to tray"), QSystemTrayIcon::Information, 2000
            );
            m_trayIconHintShown = true;
        }
        event->ignore();
    } else {
        // Close the application
        m_isClosing = true;
        event->accept();
        if (m_trayIcon) {
            m_trayIcon->hide();
        }
        qApp->quit();
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
    if (!m_trayAvailable) {
        return;
    }

    m_trayIconMenu = new QMenu(this);
    m_restoreAction = m_trayIconMenu->addAction(tr("&Restore"), this, &QWidget::showNormal);
    m_trayIconMenu->addSeparator();
    m_quitAction = m_trayIconMenu->addAction(tr("&Quit"), qApp, &QCoreApplication::quit);

    m_trayIcon = new QSystemTrayIcon(this);

    // Try to load custom icon, fallback to system icon if it fails
    QIcon icon = QIcon(":/resources/coilwhinebegone.ico");
    if (icon.isNull()) {
        icon = style()->standardIcon(QStyle::SP_ComputerIcon);
    }

    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip(tr("CoilWhineBeGone"));
    m_trayIcon->setContextMenu(m_trayIconMenu);

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    m_trayIcon->show();
}
