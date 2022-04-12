#include "Player.h"

Player::Player(QWidget *pwgt /*=0*/) : QWidget(pwgt), flag_pause(0), musicPlay("")
{
    fileSelectAction = new QAction("Выбрать файл");
    folderSelectAction = new QAction("Выбрать папку");

    menuBar = new QMenuBar();

    fileMenu = new QMenu("&Файл");
    fileMenu->addAction(fileSelectAction);
    connect(fileSelectAction, SIGNAL(triggered(bool)), SLOT(triggeredFile(bool)));

    fileMenu->addAction(folderSelectAction);
    connect(folderSelectAction, SIGNAL(triggered(bool)), SLOT(triggeredFolder(bool)));

    PlaybackMenu = new QMenu("&Воспроизведение");

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(PlaybackMenu);

    btnStartMusic = createButton("");
    btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));

    connect(btnStartMusic, SIGNAL(clicked()), SLOT(startStopMusic()));

    btnStepLeft = createButton("");
    btnStepLeft->setIcon(QPixmap("Buttons/btnStepLeft.png"));
    connect(btnStepLeft, SIGNAL(clicked()), SLOT(stepLeftMusic()));

    btnStepRight = createButton("");
    btnStepRight->setIcon(QPixmap("Buttons/btnStepRight.png"));
    connect(btnStepRight, SIGNAL(clicked()), SLOT(stepRightMusic()));

    musicTime = new QLabel("0 : 0");

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
    frameGridLayout->addWidget(lblNameFile, 0, 1);
    frameGridLayout->addWidget(btnStepLeft, 1, 0);
    frameGridLayout->addWidget(btnStartMusic, 1, 1);
    frameGridLayout->addWidget(btnStepRight, 1, 2);
    frameGridLayout->addWidget(slMusicPosition, 1, 3);
    frameGridLayout->addWidget(musicTime, 1, 4);
    frameGridLayout->addWidget(volumePosition, 1, 5);

    frameVBoxLayout = new QVBoxLayout;
    frameVBoxLayout->addLayout(menuVBoxLayout);
    frameVBoxLayout->addLayout(frameGridLayout);

    setLayout(frameVBoxLayout);
}


void Player::sliderMovedMusicPosition(int pos)
{
    slMusicPosition->setSliderPosition(pos);
    player->setPosition(pos);
}

void Player::sliderMovedVolumePosition(int pos)
{
    player->setVolume(pos);
}

void Player::volumeChanged(int pos)
{
    volumePosition->setSliderPosition(pos);
}

void Player::positionChanged(qint64 pos)
{
    slMusicPosition->setValue(pos);
    //musicTime->setText(QString::number((musicLength - pos)/1000));
    musicTime->setText(QString::number(((musicLength - pos)/1000)/60) + " : " + QString::number(((musicLength - pos)/1000)%60));
}

void Player::durationChanged(qint64 duration)
{
    slMusicPosition->setRange(0, duration);
    musicLength = duration;
}

void Player::stepLeftMusic()
{
    // Переключает музыку влево
    if(music_name_it != nullptr)
    {
        if(!music_name_it->isEmpty())
        {
            if(music_name_it != files.begin())
            {
                --music_name_it;
                lblNameFile->setText(*music_name_it);
            }

        }
    }
}

void Player::stepRightMusic()
{
    // Переключает музыку вправо
    if(music_name_it != nullptr)
    {
        if(!music_name_it->isEmpty())
        {
            if((music_name_it + 1) != files.end())
            {
                ++music_name_it;
                if(lblNameFile!= nullptr)
                {
                    lblNameFile->setText(*music_name_it);
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
        if(music_name_it != nullptr)
        {
            if(((*music_name_it) == musicPlay) || musicPlay == "")
            {
                if(player == nullptr) // Срабатывает при выборе папки
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
                    if(!flag_pause)
                    {
                        player->pause();
                        flag_pause = 1;
                        btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
                    }
                    else
                    {
                        player->play();
                        flag_pause = 0;
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
                if(!flag_pause)
                {
                    player->pause();
                    flag_pause = 1;
                    btnStartMusic->setIcon(QPixmap("Buttons/btnPlay.png"));
                }
                else
                {
                    player->play();
                    flag_pause = 0;
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

QVector<QString> Player::SearchFiles(QString &folder)
{
    // Функция написанная на C и модифицированная мной, она записыват названия файлов в каталоге и возвращает
    // Vector<QString> в котором записаны названия всех файлов
    DIR *d;
    struct dirent *dir;
    QVector<QString> result;
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


void Player::triggeredFile(bool flag)
{
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

void Player::triggeredFolder(bool flag)
{
    if(!flag) flag = 1;

    dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home/simon901/projects_for_Cpp/"
                                                "Simply MP3 Player/",
                                                QFileDialog::ShowDirsOnly |
                                                QFileDialog::DontResolveSymlinks);
    files = SearchFiles(dirName);
    if(!files.isEmpty())
    {
        lblNameFile->setText(files[0]);
        music_name_it = files.begin();
    }
}
