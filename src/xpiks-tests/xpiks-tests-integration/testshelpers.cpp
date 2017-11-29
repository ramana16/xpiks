#include "testshelpers.h"
#include <QThread>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

void sleepWaitUntil(int seconds, const std::function<bool ()> &condition) {
    int times = 0;
    bool becameTrue = false;

    while (times < seconds) {
        if (!condition()) {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            QThread::sleep(1);
            times++;
        } else {
            qDebug() << "Condition became true in" << (times + 1) << "try out of" << seconds;
            becameTrue = true;
            break;
        }
    }

    if (!becameTrue) {
        qDebug() << "Condition never was true";
    }
}

QString findFullPathForTests(const QString &prefix) {
    QFileInfo fi(prefix);
    int tries = 6;
    QStringList parents;
    while (tries--) {
        if (!fi.exists()) {
            parents.append("..");
            fi.setFile(parents.join('/') + "/" + prefix);
        } else {
            return fi.absoluteFilePath();
        }
    }

    return QFileInfo(prefix).absoluteFilePath();
}
