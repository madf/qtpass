#ifndef PROCESS_H
#define PROCESS_H
#include <QScopedPointer>
#include <QProcess>

class Process
{
public:
    Process();
    void executePass(QString, QString = QString());
    void executeWrapper(QString, QString, QString = QString());

private:
    QScopedPointer<QProcess> process;
};

#endif // PROCESS_H
