#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "mainwindow.h"
#include <QTableWidgetItem>
#include <QCloseEvent>
#include "settings.h"
#include "process.h"

namespace Ui {

struct UserInfo;

class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(MainWindow *parent);
    ~Dialog();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_radioButtonNative_clicked();
    void on_radioButtonPass_clicked();
    void on_toolButtonGit_clicked();
    void on_toolButtonGpg_clicked();
    void on_toolButtonPwgen_clicked();
    void on_toolButtonPass_clicked();
    void on_toolButtonStore_clicked();
    void on_checkBoxClipboard_clicked();
    void on_checkBoxAutoclear_clicked();
    void on_checkBoxAutoclearPanel_clicked();
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_checkBoxUseTrayIcon_clicked();
    void on_checkBoxUseGit_clicked();
    void on_checkBoxUsePwgen_clicked();
    void on_checkBoxUseTemplate_clicked();

private:
    QScopedPointer<Ui::Dialog> ui;
    void setGroupBoxState();
    QString selectExecutable();
    QString selectFolder();
    // QMessageBox::critical with hack to avoid crashes with
    // Qt 5.4.1 when QApplication::exec was not yet called
    void criticalMessage(const QString &title, const QString &text);
    MainWindow *mainWindow;
    Settings *settings;
};

#endif // DIALOG_H
