#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>

class SettingsManager : public QObject {
    Q_OBJECT

   public:
    static SettingsManager& instance();

    bool closeToTray() const;
    void setCloseToTray(bool enabled);

    bool minimizeNotify() const;
    void setMinimizeNotify(bool enabled);

    void loadSettings();
    void saveSettings();

   private:
    explicit SettingsManager(QObject* parent = nullptr);
    ~SettingsManager() = default;

    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    void initializeDefaults();

    QSettings* m_settings;

    // Setting keys
    static constexpr const char* kCloseToTrayKey = "closeToTray";
    static constexpr const char* kMinimizeNotifyKey = "minimizeNotify";

    // Default values
    static constexpr bool kDefaultCloseToTray = true;
    static constexpr bool kDefaultMinimizeNotify = true;
};

#endif  // SETTINGSMANAGER_H
