#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

class SoundManager : public QObject
{
    Q_OBJECT

public:
    static SoundManager& instance();

    void playCorrect();
    void playWrong();
    void playWarning();
    void playFinish();

private:
    explicit SoundManager(QObject *parent = nullptr);
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    QAudioOutput correctOutput;
    QAudioOutput wrongOutput;
    QAudioOutput warningOutput;
    QAudioOutput finishOutput;

    QMediaPlayer correctPlayer;
    QMediaPlayer wrongPlayer;
    QMediaPlayer warningPlayer;
    QMediaPlayer finishPlayer;
};

#endif // SOUNDMANAGER_H