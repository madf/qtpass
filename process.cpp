#include "process.h"

Process::Process() : process(new QProcess(this))
{
//    connect(process.data(), SIGNAL(readyReadStandardOutput()), this, SLOT(readyRead()));
    connect(process.data(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(process.data(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
}

