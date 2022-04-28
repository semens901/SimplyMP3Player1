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
#include <QListWidget>

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
    QPushButton *btnToggleRepeatMusic;

    QList<QString> files; // Хранит список файлов
    QList<QString>::iterator  music_name_it; // Итератор для files

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

    QListWidget *listMusicFiles;
    QListWidgetItem* selectedItem = nullptr;


    long long musicLength;

    // ФЛАГИ
    static bool flagPause; // Флаг который отображает, играет сейчас музыка или нет
    static bool flagRepeat; // Флаг отвечающий за повторение трека
    static bool flagRandom; // Флаг отвечающий за рандомное переключение трека

public:
    Player(QWidget *pwgt =0);
    QPushButton *createButton(const QString&);
    QList<QString> SearchFiles(QString&);
    void UpdateListMusicFiles(QString&);

public slots:
    void startStopMusic();
    void stepLeftMusic();
    void stepRightMusic();
    void ToggleRepeatMusic(bool);
    void sliderMovedMusicPosition(int);
    void sliderMovedVolumePosition(int);
    void volumeChanged(int);
    void durationChanged(qint64);
    void positionChanged(qint64);
    void triggeredFile(bool);
    void triggeredFolder();
    void listItemClicked(QListWidgetItem*);


};

#endif // PLAYER_H
