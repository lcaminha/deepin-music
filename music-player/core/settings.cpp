#include "settings.h"

#include <DSettingsOption>

#include <QFile>
#include <QFileInfo>
#include <qsettingbackend.h>
#include "util/global.h"
#include "util/threadpool.h"

#include <QDebug>

AppSettings::AppSettings(QObject *parent) :
    QObject(parent)
{

}

void AppSettings::init()
{
    m_settings = Dtk::Core::DSettings::fromJsonFile(":/data/deepin-music-settings.json");
    auto configFilepath = Global::configPath() + "/config.ini";
    auto backend = new Dtk::Core::QSettingBackend(configFilepath);
    m_settings->setBackend(backend);
//    ThreadPool::instance()->manager(backend->thread());
}

QPointer<Dtk::Core::DSettings> AppSettings::settings() const
{
    return m_settings;
}

void AppSettings::sync()
{
    m_settings->sync();
}

QVariant AppSettings::value(const QString &key) const
{
    return m_settings->value(key);
}

void AppSettings::setOption(const QString &key, const QVariant &value)
{
    m_settings->setOption(key, value);
}
