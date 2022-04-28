#include "Player.h"

Player::Player(QWidget *pwgt /*=0*/) : QWidget(pwgt), musicPlay("")
{
    // Меню
    fileSelectAction = new QAction("Выбрать файл");
    folderSelectAction = new QAction("Выбрать папку");

    menuBar = new QMenuBar();

    fileMenu = new QMenu("&Файл");
    fileMenu->addAction(fileSelectAction);
    connect(fileSelectAction, SIGNAL(triggered(bool)), SLOT(triggeredFile(bool)));

    fileMenu->addAction(folderSelectAction);
    connect(folderSelectAction, SIGNAL(triggered(bool)), SLOT(triggeredFolder()));

    PlaybackMenu = new QMenu("&Воспроизведение");

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(PlaybackMenu);

    // Показывает музыку из списка
    listMusicFiles = new QListWidget();
    connect(listMusicFiles, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(listItemClicked(QListWidgetItem*)));

    // Кнопка запуска/паузы музыки
    btnStartMusic = createButton("");
    btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));

    connect(btnStartMusic, SIGNAL(clicked()), SLOT(startStopMusic()));

    // Кнопка переключения влево
    btnStepLeft = createButton("");
    btnStepLeft->setIcon(QPixmap("Buttons/btnStepLeft.png"));
    connect(btnStepLeft, SIGNAL(clicked()), SLOT(stepLeftMusic()));

    // Кнопка переключения вправо
    btnStepRight = createButton("");
    btnStepRight->setIcon(QPixmap("Buttons/btnStepRight.png"));
    connect(btnStepRight, SIGNAL(clicked()), SLOT(stepRightMusic()));

    // Кнопка отвечающая за повторение трека
    btnToggleRepeatMusic = createButton("");
    btnToggleRepeatMusic->setCheckable(true);
    btnToggleRepeatMusic->setChecked(false);
    btnToggleRepeatMusic->setIcon(QPixmap("Buttons/repeat_OFF.png"));
    connect(btnToggleRepeatMusic, SIGNAL(toggled(bool)), SLOT(ToggleRepeatMusic(bool)));

    // Показывает сколько будет длится музыка
    musicTime = new QLabel("0 : 0");

    // Показывает имя проигрываемого файла
    lblNameFile = new QLabel("");

    // Ползунок отвечающий за регулировку позиции музыки
    slMusicPosition = new QSlider(Qt::Horizontal);
    connect(slMusicPosition, SIGNAL(sliderMoved(int)), SLOT(sliderMovedMusicPosition(int)));
    slMusicPosition->setMinimumWidth(150);
    slMusicPosition->setTickInterval(1);

    // Ползунок отвечающий за регулировку громкости
    volumePosition = new QSlider(Qt::Horizontal);
    connect(volumePosition, SIGNAL(sliderMoved(int)), SLOT(sliderMovedVolumePosition(int)));
    volumePosition->setMinimumWidth(60);
    volumePosition->setRange(0, 100);
    volumePosition->setSliderPosition(100);

    // собираем все виджеты на нашем каркасе
    menuVBoxLayout = new QVBoxLayout;
    menuVBoxLayout->addWidget(menuBar);

    frameGridLayout = new QGridLayout;
    frameGridLayout->addWidget(btnToggleRepeatMusic, 0, 0);
    frameGridLayout->addWidget(lblNameFile, 1, 1);
    frameGridLayout->addWidget(btnStepLeft, 2, 0);
    frameGridLayout->addWidget(btnStartMusic, 2, 1);
    frameGridLayout->addWidget(btnStepRight, 2, 2);
    frameGridLayout->addWidget(slMusicPosition, 2, 3);
    frameGridLayout->addWidget(musicTime, 2, 4);
    frameGridLayout->addWidget(volumePosition, 2, 5);

    frameVBoxLayout = new QVBoxLayout;
    frameVBoxLayout->addLayout(menuVBoxLayout);
    frameVBoxLayout->addWidget(listMusicFiles);
    frameVBoxLayout->addLayout(frameGridLayout);

    setLayout(frameVBoxLayout);
}

bool Player::flagPause = 0;
bool Player::flagRepeat = 0;
bool Player::flagRandom = 0;

void Player::ToggleRepeatMusic(bool flag)
{
    // Отвечает за триггер повтора музыки
    if(!flag)
    {
        flagRepeat=0;
    }
    else
    {
        flagRepeat = 1;
    }
}

void Player::listItemClicked(QListWidgetItem* item)
{
    // Обрабатывает какой элемент выбран на listMusicFiles
    selectedItem = item;
    lblNameFile->setText(selectedItem->text());
    if(musicPlay != lblNameFile->text())
    {
        btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
    }
    else
    {
        btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
    }
    music_name_it = files.begin();

    // Временное решение
    while(*music_name_it != lblNameFile->text())
    {
        ++music_name_it;
    }


}

void Player::sliderMovedMusicPosition(int pos)
{
    // Меняет позицию ползунка проигрывания музыки
    if(player != nullptr)
    {
        slMusicPosition->setSliderPosition(pos);
        player->setPosition(pos);
    }
}

void Player::sliderMovedVolumePosition(int pos)
{
    // Изменяет громкость музыки
    if(player!=nullptr)
        player->setVolume(pos);
}

void Player::volumeChanged(int pos)
{
    // Обрабатывает изменения на самом ползунке регулирования громкости
    volumePosition->setSliderPosition(pos);
}

void Player::positionChanged(qint64 pos)
{
    // Музыка отправляя сигнал сама меняет позицию ползунка при проигрывании и при этом
    // Мы еще и автоматически переключаем трек при окончании музыки
    slMusicPosition->setValue(pos);
    //musicTime->setText(QString::number((musicLength - pos)/1000));
    musicTime->setText(QString::number(((musicLength - pos)/1000)/60) + " : " + QString::number(((musicLength - pos)/1000)%60));
    if(musicLength != 0)
    {
        if(((((musicLength - pos)/1000)/60) == 0) && ((((musicLength - pos)/1000)%60) == 0))
        {
            if(!files.isEmpty())
            {
                if(((music_name_it + 1) != files.end()) && !flagRepeat)
                {
                    stepRightMusic();
                    startStopMusic();
                }
                else if(flagRepeat)
                {
                    player->setPosition(0);
                    player->play();
                }
            }
            else
            {
                if(flagRepeat)
                {
                    player->setPosition(0);
                    player->play();
                }
            }
        }
    }

}

void Player::durationChanged(qint64 duration)
{
    // Устанавливаем радиус ползунка
    slMusicPosition->setRange(0, duration);
    musicLength = duration;
}

void Player::stepLeftMusic()
{
    // Переключает музыку влево
    if(!music_name_it->isEmpty())
    {
        if(!music_name_it->isEmpty())
        {
            if(music_name_it != files.begin())
            {
                --music_name_it;
                lblNameFile->setText(*music_name_it);

                if(musicPlay != lblNameFile->text())
                {
                    btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
                }
                else
                {
                    btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                }
            }

        }
    }
}

void Player::stepRightMusic()
{
    // Переключает музыку вправо
    if(!music_name_it->isEmpty())
    {
        if(!music_name_it->isEmpty())
        {
            if((music_name_it + 1) != files.end())
            {
                ++music_name_it;
                lblNameFile->setText(*music_name_it);

                if(musicPlay != lblNameFile->text())
                {
                    btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
                }
                else
                {
                    btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                }
            }

        }
    }
}

void Player::startStopMusic()
{
    // Запускает мызыку, ставит на паузу

    /*  КОММЕНТАРИИ НА БУДУЩЕЕ
     *  В будущем эту функцию лучше переписать
     */
    if(lblNameFile->text() != "")
    {
        if(!files.isEmpty())
        {
            // Срабатывает если мы выбираем папку
                if(((lblNameFile->text() == musicPlay) || (musicPlay == "")))
                {
                    if(player == nullptr)
                    {

                        player = new QMediaPlayer;
                        connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64))); // позиция трека на дорожке
                        connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64))); // длина трека
                        connect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));
                        player->setMedia(QUrl::fromLocalFile(dirName + "/" + lblNameFile->text()));
                        player->play();
                        musicPlay = lblNameFile->text();
                        btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                    }
                    else
                    {
                        if(!flagPause)
                        {
                            player->pause();
                            flagPause = 1;
                            btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
                        }
                        else
                        {
                            player->play();
                            flagPause = 0;
                            btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                        }
                    }
                }
                else
                {
                    if(player != nullptr)
                    {
                        disconnect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
                        disconnect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
                        disconnect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));
                        player->~QMediaPlayer();
                        player = nullptr;

                        if(player == nullptr)
                        {

                            player = new QMediaPlayer;
                            connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
                            connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
                            connect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));
                            player->setMedia(QUrl::fromLocalFile(dirName + "/" + lblNameFile->text()));
                            player->play();
                            musicPlay = lblNameFile->text();
                            btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                        }
                    }

                }
            }

        else
        {
            // Срабатывает если мы выбираем файл
            if(musicPlay == "")
            {
                if(player == nullptr) // Срабатывает при первом запуске программы
                {

                    player = new QMediaPlayer;
                    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64))); // позиция трека на дорожке
                    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64))); // длина трека
                    connect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));
                    player->setMedia(QUrl::fromLocalFile(dirName + "/" + lblNameFile->text()));
                    player->play();
                    musicPlay = lblNameFile->text();
                    btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                }
            }
            else if(player != nullptr && (lblNameFile->text() == musicPlay))
            {
                if(!flagPause)
                {
                    player->pause();
                    flagPause = 1;
                    btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
                }
                else
                {
                    player->play();
                    flagPause = 0;
                    btnStartMusic->setIcon(QPixmap("Buttons/btnStop.png"));
                }
            }
            else
            {
                if(player!= nullptr)
                {
                    disconnect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
                    disconnect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
                    disconnect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));
                    player->~QMediaPlayer();
                    player = nullptr;

                    if(player == nullptr)
                    {

                        player = new QMediaPlayer;
                        connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
                        connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
                        connect(player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));
                        player->setMedia(QUrl::fromLocalFile(dirName + "/" + lblNameFile->text()));
                        player->play();
                        musicPlay = lblNameFile->text();
                    }
                }

            }

        }
    }
}

QPushButton *Player::createButton(const QString &str)
{
    // Создает кнопки
    QPushButton *btn = new QPushButton(str);
    btn->setMinimumSize(30, 30);
    return btn;
}

QList<QString> Player::SearchFiles(QString &folder)
{
    // Функция написанная на C и модифицированная мной, она записыват названия файлов в каталоге и возвращает
    // Vector<QString> в котором записаны названия всех файлов
    DIR *d;
    struct dirent *dir;
    QList<QString> result;
    d = opendir(folder.toStdString().c_str());
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            if(dir->d_type == DT_REG)
            {
                result.push_back((QString)dir->d_name);
            }
        }
        closedir(d);
    }
    return result;
}

void Player::UpdateListMusicFiles(QString &dir)
{
    // Обновляет listMusicFiles
    listMusicFiles->clear();
    QList<QString> musicFiles = SearchFiles(dir);

    for(QString musicItem : musicFiles)
    {
        listMusicFiles->addItem(musicItem);
    }
}

void Player::triggeredFile(bool flag)
{
    //Получает путь до файла с музыкой от пользователя
    if(!flag) flag = 1;
    else flag = 0;

    if(flag)
    {
        QString fileName;
        dirFile = QFileDialog::getOpenFileName(this, tr("Open Directory"),
                                                   "/home/simon901/projects_for_Cpp/"
                                                   "Simply MP3 Player/",
                                                   tr("Music (*.mp3)"));
        if(!dirFile.isEmpty())
        {
            fileName = QFileInfo(dirFile).fileName();
            dirName = QFileInfo(dirFile).absolutePath();
            lblNameFile->setText(fileName);
        }
    }
}

void Player::triggeredFolder()
{
   // Получает путь до папки с музыкой от пользователя

   //if(!flag) flag = 1;

    dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home/simon901/projects_for_Cpp/"
                                                "Simply MP3 Player/",
                                                QFileDialog::ShowDirsOnly |
                                                QFileDialog::DontResolveSymlinks);
    UpdateListMusicFiles(dirName);
    files = SearchFiles(dirName);
    if(!files.isEmpty())
    {
        lblNameFile->setText(files[0]);
        music_name_it = files.begin();
    }
}
