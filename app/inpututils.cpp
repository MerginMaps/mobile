#include "inpututils.h"
#include <QFile>
#include <QFileInfo>

InputUtils::InputUtils(QObject* parent):QObject(parent)
{
}

bool InputUtils::removeFile(const QString &filePath)
{
    QFile file( filePath );
    return file.remove( filePath );
}

bool InputUtils::cpFile(const QString &srcPath, const QString &dstPath)
{
    QString modSrcPath = srcPath;
    if (srcPath.startsWith("file://")) {
        modSrcPath = modSrcPath.replace("file://", "");
    }

    return QFile::copy(modSrcPath, dstPath);
}

QString InputUtils::getFileName(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.fileName();
}
