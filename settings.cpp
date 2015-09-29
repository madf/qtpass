#include "settings.h"

Settings::Settings()
{
    if (!settings) {
        QString portable_ini = QCoreApplication::applicationDirPath() + QDir::separator() + "qtpass.ini";
        //qDebug() << "Settings file: " + portable_ini;
        if (QFile(portable_ini).exists()) {
           // qDebug() << "Settings file exists, loading it in";
            settings.reset(new QSettings(portable_ini, QSettings::IniFormat));
        } else {
            //qDebug() << "Settings file does not exist, use defaults";
            settings.reset(new QSettings("IJHack", "QtPass"));
        }
    }

    settings.beginGroup( "mainwindow" );
    restoreGeometry(settings.value( "geometry", saveGeometry() ).toByteArray());
    restoreState(settings.value( "savestate", saveState() ).toByteArray());
    move(settings.value( "pos", pos() ).toPoint());
    resize(settings.value( "size", size() ).toSize());
    QList<int> splitter = ui->splitter->sizes();
    int left = settings.value("splitterLeft", splitter[0]).toInt();
    int right= settings.value("splitterRight", splitter[1]).toInt();
    if (left > 0 || right > 0) {
        splitter[0] = left;
        splitter[1] = right;
        ui->splitter->setSizes(splitter);
    }
    if ( settings.value( "maximized", isMaximized() ).toBool() )
        showMaximized();
    settings.endGroup();
}

/**
 * @brief Settings::checkConfig
 */
bool Settings::checkConfig() {



    usePass = (settings.value("usePass") == "true");

    useClipboard = (settings.value("useClipboard") == "true");
    useAutoclear = (settings.value("useAutoclear") == "true");
    autoclearSeconds = settings.value("autoclearSeconds").toInt();
    useAutoclearPanel = (settings.value("useAutoclearPanel") == "true");
    autoclearPanelSeconds = settings.value("autoclearPanelSeconds").toInt();
    hidePassword = (settings.value("hidePassword") == "true");
    hideContent = (settings.value("hideContent") == "true");
    addGPGId = (settings.value("addGPGId") != "false");

    passStore = settings.value("passStore").toString();
    if (passStore.isEmpty()) {
        passStore = Util::findPasswordStore();
        settings.setValue("passStore", passStore);
    }
    passStore = Util::normalizeFolderPath(passStore);

    passExecutable = settings.value("passExecutable").toString();
    if (passExecutable.isEmpty()) {
        passExecutable = Util::findBinaryInPath("pass");
    }

    gitExecutable = settings.value("gitExecutable").toString();
    if (gitExecutable.isEmpty()) {
        gitExecutable = Util::findBinaryInPath("git");
    }

    gpgExecutable = settings.value("gpgExecutable").toString();
    if (gpgExecutable.isEmpty()) {
        gpgExecutable = Util::findBinaryInPath("gpg2");
    }

    pwgenExecutable = settings.value("pwgenExecutable").toString();
    if (pwgenExecutable.isEmpty()) {
        pwgenExecutable = Util::findBinaryInPath("pwgen");
    }

    gpgHome = settings.value("gpgHome").toString();

    useWebDav = (settings.value("useWebDav") == "true");
    webDavUrl = settings.value("webDavUrl").toString();
    webDavUser = settings.value("webDavUser").toString();
    webDavPassword = settings.value("webDavPassword").toString();

    profile = settings.value("profile").toString();
    settings.beginGroup("profiles");
    QStringList keys = settings.childKeys();
    foreach (QString key, keys) {
         profiles[key] = settings.value(key).toString();
    }
    settings.endGroup();

    useGit = (settings.value("useGit") == "true");
    usePwgen = (settings.value("usePwgen") == "true");
    useSymbols = (settings.value("useSymbols") == "true");
    passwordLength = settings.value("passwordLength").toInt();
    passwordChars = settings.value("passwordChars").toString();

    useTrayIcon = settings.value("useTrayIcon").toBool();
    hideOnClose = settings.value("hideOnClose").toBool();
    startMinimized = settings.value("startMinimized").toBool();

    autoPull = settings.value("autoPull").toBool();
    autoPush = settings.value("autoPush").toBool();

    if (useTrayIcon && tray == NULL) {
        initTrayIcon();
        if (freshStart && startMinimized) {
            // since we are still in constructor, can't directly hide
            QTimer::singleShot(10, this, SLOT(hide()));
        }
    } else if (!useTrayIcon && tray != NULL) {
        destroyTrayIcon();
    }

    passTemplate = settings.value("passTemplate").toString();
    useTemplate = settings.value("useTemplate").toBool();
    templateAllFields = settings.value("templateAllFields").toBool();

    //qDebug() << version;

    // Config updates
    if (version.isEmpty()) {
        qDebug() << "assuming fresh install";
        if (autoclearSeconds < 5) {
            autoclearSeconds = 10;
        }
        if (autoclearPanelSeconds < 5) {
            autoclearPanelSeconds = 10;
        }
        passwordLength = 16;
        passwordChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890~!@#$%^&*()_-+={}[]|:;<>,.?";
        if (!pwgenExecutable.isEmpty()) {
            usePwgen = true;
        } else {
            usePwgen = false;
        }
    } else {
//        QStringList ver = version.split(".");
//        qDebug() << ver;
//        if (ver[0] == "0" && ver[1] == "8") {
//            // upgrade to 0.9
//        }
        if (passwordChars.isEmpty()) {
            passwordChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890~!@#$%^&*()_-+={}[]|:;<>,.?";
        }
    }
    settings.setValue("version", VERSION);

    if (Util::checkConfig(passStore, passExecutable, gpgExecutable)) {
        config();
        if (freshStart && Util::checkConfig(passStore, passExecutable, gpgExecutable)) {
            return false;
        }
    }

    freshStart = false;

    // TODO: this needs to be before we try to access the store,
    // but it would be better to do it after the Window is shown,
    // as the long delay it can cause is irritating otherwise.
    if (useWebDav) {
        mountWebDav();
    }

    model.setNameFilters(QStringList() << "*.gpg");
    model.setNameFilterDisables(false);

    proxyModel.setSourceModel(&model);
    proxyModel.setModelAndStore(&model, passStore);
    selectionModel.reset(new QItemSelectionModel(&proxyModel));
    model.fetchMore(model.setRootPath(passStore));
    model.sort(0, Qt::AscendingOrder);

    ui->treeView->setModel(&proxyModel);
    ui->treeView->setRootIndex(proxyModel.mapFromSource(model.setRootPath(passStore)));
    ui->treeView->setColumnHidden(1, true);
    ui->treeView->setColumnHidden(2, true);
    ui->treeView->setColumnHidden(3, true);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setIndentation(15);
    ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));

    ui->textBrowser->setOpenExternalLinks(true);

    updateProfileBox();

    env = QProcess::systemEnvironment();
    if (!gpgHome.isEmpty()) {
        QDir absHome(gpgHome);
        absHome.makeAbsolute();
        env << "GNUPGHOME=" + absHome.path();
    }
#ifdef __APPLE__
    // If it exists, add the gpgtools to PATH
    if (QFile("/usr/local/MacGPG2/bin").exists()) {
        env.replaceInStrings("PATH=", "PATH=/usr/local/MacGPG2/bin:");
    }
    // Add missing /usr/local/bin
    if (env.filter("/usr/local/bin").isEmpty()) {
        env.replaceInStrings("PATH=", "PATH=/usr/local/bin:");
    }
#endif
    //QMessageBox::information(this, "env", env.join("\n"));

    updateEnv();

    if (!useGit || (gitExecutable.isEmpty() && passExecutable.isEmpty()))
    {
        ui->pushButton->hide();
        ui->updateButton->hide();
    } else {
        ui->pushButton->show();
        ui->updateButton->show();
    }

    startupPhase = false;
    return true;
}
