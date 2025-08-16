#include "settingsmanager.h"

#include <QCoreApplication>
#include <QDebug>

SettingsManager::SettingsManager(QObject* parent) : QObject(parent), m_settings(nullptr) {
    m_settings = new QSettings(
        QSettings::IniFormat,
        QSettings::UserScope,
        QCoreApplication::organizationName(),
        QCoreApplication::applicationName(),
        this
    );

    initializeDefaults();
    loadSettings();
}

SettingsManager& SettingsManager::instance() {
    static SettingsManager instance;
    return instance;
}

bool SettingsManager::closeToTray() const {
    return m_settings->value(kCloseToTrayKey, kDefaultCloseToTray).toBool();
}

void SettingsManager::setCloseToTray(bool enabled) {
    m_settings->setValue(kCloseToTrayKey, enabled);
    saveSettings();
}

bool SettingsManager::minimizeNotify() const {
    return m_settings->value(kMinimizeNotifyKey, kDefaultMinimizeNotify).toBool();
}

void SettingsManager::setMinimizeNotify(bool enabled) {
    m_settings->setValue(kMinimizeNotifyKey, enabled);
    saveSettings();
}

void SettingsManager::loadSettings() {
    m_settings->sync();
}

void SettingsManager::saveSettings() {
    m_settings->sync();

    if (m_settings->status() != QSettings::NoError) {
        qWarning() << "Failed to save settings to:" << m_settings->fileName();
    } else {
        qDebug() << "Settings saved successfully";
    }
}

void SettingsManager::initializeDefaults() {
    if (!m_settings->contains(kCloseToTrayKey)) {
        m_settings->setValue(kCloseToTrayKey, kDefaultCloseToTray);
    }
    if (!m_settings->contains(kMinimizeNotifyKey)) {
        m_settings->setValue(kMinimizeNotifyKey, kDefaultMinimizeNotify);
    }
}
