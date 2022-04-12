#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include<dirent.h>

#include<QWidget>
#include<QtWidgets>
#include<QMediaPlayer>
#include<QPushButton>
#include<QFileDialog>
#include<QMenu>
#include<QMenuBar>

class Player : public QWidget
{
    Q_OBJECT
private:

    QString dirName; // Хранит путь выбранный пользователем
    QString dirFile; // Хранит путь до файла
    QString musicPlay; // Хранит название файла который проигрывается

    QPushButton *btnStartMusic;
    QPushButton *btnStepLeft;
    QPushButton *btnStepRight;

    QVector<QString> files; // Хранит список файлов
    QVector<QString>::iterator  music_name_it = nullptr; // Итератор для files

    QLabel *lblNameFile = nullptr; // Лейбл который выводит название песни
    QLabel *musicTime; // На этом лейбле будет отображаться, сколько длится музыка

    QGridLayout *frameGridLayout = nullptr; // Каркас приложения
    QVBoxLayout *frameVBoxLayout = nullptr;
    QVBoxLayout *menuVBoxLayout = nullptr;

    QMediaPlayer *player = nullptr; // Плеер который проигрывает музыку

    QSlider *slMusicPosition; // Ползунок отвечающий за регулировку позиции музыки
    QSlider *volumePosition; // Ползунок отвечающий за регулировку громкости

    QMenuBar *menuBar; // Меню бар
    QMenu *fileMenu; // Менюшка для вкладки "Файл"
    QMenu *PlaybackMenu; // Менюшка для вкладки "Воспроизведение"

    QAction *fileSelectAction;
    QAction *folderSelectAction;

    long long musicLength;

    // ФЛАГИ
    bool flag_pause; // Флаг который отображает, играет сейчас музыка или нет

public:
    Player(QWidget *pwgt =0);
    QPushButton *createButton(const QString &str);
    QVector<QString> SearchFiles(QString &folder);

public slots:
    void startStopMusic();
    void stepLeftMusic();
    void stepRightMusic();
    void sliderMovedMusicPosition(int);
    void sliderMovedVolumePosition(int);
    void volumeChanged(int);
    void durationChanged(qint64);
    void positionChanged(qint64);
    void triggeredFile(bool);
    void triggeredFolder(bool);


};

#endif // PLAYER_H
