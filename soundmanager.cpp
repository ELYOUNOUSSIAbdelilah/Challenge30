#include "soundmanager.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QUrl>

SoundManager::SoundManager(QObject *parent)
    : QObject(parent)
{
    QString appDir = QApplication::applicationDirPath();
    QString soundBasePath = appDir + "/../../sounds/";

    QString correctPath = soundBasePath + "correct.wav";
    QString wrongPath   = soundBasePath + "wrong.wav";
    QString warningPath = soundBasePath + "warning.wav";
    QString finishPath  = soundBasePath + "finish.wav";

    correctPlayer.setAudioOutput(&correctOutput);
    wrongPlayer.setAudioOutput(&wrongOutput);
    warningPlayer.setAudioOutput(&warningOutput);
    finishPlayer.setAudioOutput(&finishOutput);

    correctOutput.setVolume(0.8);
    wrongOutput.setVolume(0.8);
    warningOutput.setVolume(0.7);
    finishOutput.setVolume(0.9);

    correctPlayer.setSource(QUrl::fromLocalFile(correctPath));
    wrongPlayer.setSource(QUrl::fromLocalFile(wrongPath));
    warningPlayer.setSource(QUrl::fromLocalFile(warningPath));
    finishPlayer.setSource(QUrl::fromLocalFile(finishPath));
}

SoundManager& SoundManager::instance()
{
    static SoundManager instance;
    return instance;
}

void SoundManager::playCorrect()
{
    correctPlayer.stop();
    correctPlayer.setPosition(0);
    correctPlayer.play();
}

void SoundManager::playWrong()
{
    wrongPlayer.stop();
    wrongPlayer.setPosition(0);
    wrongPlayer.play();
}

void SoundManager::playWarning()
{
    warningPlayer.stop();
    warningPlayer.setPosition(0);
    warningPlayer.play();
}

void SoundManager::playFinish()
{
    finishPlayer.stop();
    finishPlayer.setPosition(0);
    finishPlayer.play();
}