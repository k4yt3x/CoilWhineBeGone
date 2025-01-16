#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QKeySequence>
#include <QShortcut>

#include <Windows.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
}

MainWindow::~MainWindow()
{
    delete ui;
}
