#include "merginapi.h"

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QByteArray>
#include <QSet>
#include <QMessageBox>

MerginApi::MerginApi(const QString& dataDir, QObject *parent)
  : QObject (parent)
  , mDataDir(dataDir + "/")
  , mCacheFile(".projectsCache.txt")
{
    QObject::connect(this, &MerginApi::syncProjectFinished,this, &MerginApi::setUpdateToProject);
    QObject::connect(this, &MerginApi::merginProjectsChanged,this, &MerginApi::cacheProjects);
    QObject::connect(this, &MerginApi::authChanged,this, &MerginApi::saveAuthData);

    loadAuthData();
}

void MerginApi::listProjects()
{
    if (!hasAuthData()) {
        emit authRequested();
        return;
    }

    QByteArray token = generateToken();
    QNetworkRequest request;
    // projects filtered by tag "input_use"
    QUrl url(mApiRoot + "/v1/project?tags=input_use");
    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + token));

    QNetworkReply *reply = mManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::listProjectsReplyFinished);
}

void MerginApi::downloadProject(QString projectName)
{
    if (!hasAuthData()) {
        emit authRequested();
        return;
    }

    QByteArray token = generateToken();
    QNetworkRequest request;
    QUrl url(mApiRoot + "/v1/project/download/" + projectName);

    if (mPendingRequests.contains(url)) {
        QString errorMsg = QStringLiteral("Download request for %1 is already pending.").arg(projectName);
        qDebug() << errorMsg;
        emit networkErrorOccurred( errorMsg, "Mergin API error: downloadProject" );
        return;
    }

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + token));

    QNetworkReply *reply = mManager.get(request);
    mPendingRequests.insert(url, projectName);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::downloadProjectReplyFinished);
}

void MerginApi::updateProject(QString projectName)
{

    if (!hasAuthData()) {
        emit authRequested();
        return;
    }

    QByteArray token = generateToken();
    QNetworkRequest request;
    QUrl url(mApiRoot + "/v1/project/" + projectName);

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + token));

    QNetworkReply *reply = mManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::updateInfoReplyFinished);

}

void MerginApi::uploadProject(QString projectName)
{

    bool onlyUpload = true;
    for (std::shared_ptr<MerginProject> project: mMerginProjects) {
        if(project->name == projectName) {
            if (project->updated < project->serverUpdated && project->serverUpdated > project->lastSync.toUTC()) {
                onlyUpload = false;
            }
        }
    }

    if (onlyUpload) {
       continueWithUpload(mDataDir + projectName, projectName, true);
    } else {
        QMessageBox msgBox;
        msgBox.setText("The project has been updated on the server in the meantime. Your files will be updated before upload.");
        msgBox.setInformativeText("Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel) {
            emit syncProjectFinished(mDataDir + projectName, projectName, false);
            return;
        }

        mWaitingForUpload.insert(projectName);
        updateProject(projectName);
        connect(this, &MerginApi::syncProjectFinished, this, &MerginApi::continueWithUpload);
    }
}

void MerginApi::authorize(QString username, QString password)
{
    mUsername = username;
    mPassword = password;
    emit authChanged();
}

void MerginApi::clearAuth()
{
    mUsername = "";
    mPassword = "";
    emit authChanged();
}

void MerginApi::resetApiRoot()
{
    QSettings settings;
    settings.beginGroup("Input/");
    setApiRoot(defaultApiRoot());
    settings.endGroup();
}

bool MerginApi::hasAuthData()
{
    return !mUsername.isEmpty() && !mPassword.isEmpty();
}

void MerginApi::downloadProjectFiles(QString projectName, QByteArray json)
{
    if (!hasAuthData()) {
        emit authRequested();
        return;
    }

    QByteArray token = generateToken();
    QNetworkRequest request;
    QUrl url(mApiRoot + "/v1/project/fetch/" + projectName);
    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + token));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");
    mPendingRequests.insert(url, projectName);

    QNetworkReply *reply = mManager.post(request, json);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::downloadProjectReplyFinished);
}

void MerginApi::uploadProjectFiles(QString projectName, QByteArray json, QList<MerginFile> files)
{
    if (!hasAuthData()) {
        emit networkErrorOccurred( "Auth token is invalid", "Mergin API error: fetchProject" );
        return;
    }

    QString projectPath = QString(mDataDir + projectName + "/");
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"changes\""));
    textPart.setBody(json);
    multiPart->append(textPart);

    for (MerginFile file: files) {
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"; filename=\"%2\"").arg(file.path).arg(file.path));
        QFile *f = new QFile(projectPath + file.path);
        f->open(QIODevice::ReadOnly);
        filePart.setBodyDevice(f);
        f->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
        multiPart->append(filePart);
    }

    QByteArray token = generateToken();
    QNetworkRequest request;
    QUrl url(mApiRoot + "/v1/project/data_sync/" + projectName);
    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + token));
    mPendingRequests.insert(url, projectName);

    QNetworkReply *reply = mManager.post(request, multiPart);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::uploadProjectReplyFinished);
}

ProjectList MerginApi::updateMerginProjectList(ProjectList serverProjects)
{
    QHash<QString, std::shared_ptr<MerginProject>> projectUpdates;
    for (std::shared_ptr<MerginProject> project: mMerginProjects) {
        projectUpdates.insert(project->name, project);
    }

    for (std::shared_ptr<MerginProject> project: serverProjects) {
        if (projectUpdates.contains(project->name)) {
            QDateTime localUpdate = projectUpdates.value(project->name).get()->updated;
            project->lastSync = projectUpdates.value(project->name).get()->lastSync;
            QDateTime lastModified = QFileInfo(mDataDir + project->name).lastModified();
            project->updated = localUpdate;
            project->status = getProjectStatus(project->updated, project->serverUpdated, project->lastSync, lastModified);
        }
    }
    return serverProjects;
}

void MerginApi::setUpdateToProject(QString projectDir, QString projectName, bool successfully)
{
    Q_UNUSED(projectDir);
    if (!successfully) return;

    for (std::shared_ptr<MerginProject> project: mMerginProjects) {
        if (projectName == project->name) {
            project->updated = project->serverUpdated;
            project->lastSync = QDateTime::currentDateTime();
            emit merginProjectsChanged();
            return;
        }
    }
}

void MerginApi::deleteObsoleteFiles(QString projectPath)
{
    if (!mObsoleteFiles.value(projectPath).isEmpty()) {
        for (QString filename: mObsoleteFiles.value(projectPath)) {
            QFile file (projectPath + filename);
            file.remove();
        }
       mObsoleteFiles.remove(projectPath);
    }
}

void MerginApi::saveAuthData()
{
    QSettings settings;
    settings.beginGroup("Input/");
    settings.setValue("username", mUsername);
    settings.setValue("password", mPassword);
    settings.setValue("apiRoot", mApiRoot);
    settings.endGroup();
}

void MerginApi::projectDeleted(QString projectName)
{
    for (std::shared_ptr<MerginProject> project: mMerginProjects) {
        if (project->name == projectName) {
            project->status = ProjectStatus::NoVersion;
            project->lastSync = QDateTime();
            project->updated = QDateTime();
            emit merginProjectsChanged();
        }
    }
}


void MerginApi::loadAuthData()
{
    QSettings settings;
    settings.beginGroup("Input/");
    setApiRoot(settings.value("apiRoot").toString());
    mUsername = settings.value("username").toString();
    mPassword = settings.value("password").toString();
}

QString MerginApi::apiRoot() const
{
    return mApiRoot;
}

void MerginApi::setApiRoot(const QString &apiRoot)
{
    QSettings settings;
    settings.beginGroup("Input/");
    if (apiRoot.isEmpty()) {
        mApiRoot = defaultApiRoot();
    } else {
        mApiRoot = apiRoot;
    }
    settings.setValue("apiRoot", mApiRoot);
    settings.endGroup();
    emit apiRootChanged();
}

QByteArray MerginApi::generateToken()
{
    QString concatenated = mUsername + ":" + mPassword;
    return concatenated.toLocal8Bit().toBase64();
}

QString MerginApi::username() const
{
    return mUsername;
}

ProjectList MerginApi::projects()
{
    return mMerginProjects;
}

void MerginApi::listProjectsReplyFinished()
{

    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    if (r->error() == QNetworkReply::NoError)
    {

        if (mMerginProjects.isEmpty()) {
            QFile file(mDataDir + mCacheFile);
            if (file.open(QIODevice::ReadOnly)) {
                QByteArray cachedData = file.readAll();
                file.close();

                mMerginProjects = parseProjectsData(cachedData);
            }
        }


      QByteArray data = r->readAll();
      ProjectList serverProjects = parseProjectsData(data, true);
      mMerginProjects = updateMerginProjectList(serverProjects);
      emit merginProjectsChanged();
    }
    else {
        QString message = QStringLiteral("Network API error: %1(): %2").arg("listProjects", r->errorString());
        qDebug("%s", message.toStdString().c_str());
        emit networkErrorOccurred( r->errorString(), "Mergin API error: listProjects" );

        if (r->errorString() == "Host requires authentication") {
            emit authRequested();
            return;
        }
    }

    r->deleteLater();
    emit listProjectsFinished(mMerginProjects);
}

void MerginApi::downloadProjectReplyFinished()
{

    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    QString projectName = mPendingRequests.value(r->url());
    QString projectDir = mDataDir + projectName;
    if (r->error() == QNetworkReply::NoError)
    {
        bool waitingForUpload = mWaitingForUpload.contains(projectName);

        deleteObsoleteFiles(projectDir + "/");
        handleDataStream(r, projectDir, !waitingForUpload);
        emit syncProjectFinished(projectDir, projectName);
        if (!waitingForUpload) {
            emit notify("Download successful");
        }
    }
    else {
        qDebug() << r->errorString();
        emit syncProjectFinished(projectDir, projectName, false);
        emit networkErrorOccurred( r->errorString(), "Mergin API error: downloadProject" );
    }
    mPendingRequests.remove(r->url());
    r->deleteLater();
}

void MerginApi::uploadProjectReplyFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    QString projectName = mPendingRequests.value(r->url());
    QString projectDir = mDataDir + projectName;
    if (r->error() == QNetworkReply::NoError)
    {
        emit syncProjectFinished(projectDir, projectName);
        emit notify("Upload successful");
    }
    else {
        qDebug() << r->errorString();
        emit syncProjectFinished(projectDir, projectName, false);
        emit networkErrorOccurred( r->errorString(), "Mergin API error: uploadProject" );
    }
    mPendingRequests.remove(r->url());
    r->deleteLater();
}

void MerginApi::updateInfoReplyFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    QHash<QString, QList<MerginFile>> files = parseAndCompareProjectFiles(r, true);

    QUrl url = r->url();
    QStringList res = url.path().split("/");
    QString projectName;
    projectName = res.last();
    QString projectPath = QString(mDataDir + projectName + "/");

    r->deleteLater();

    QJsonDocument jsonDoc;
    QJsonArray fileArray;
    for (QString key: files.keys()) {
        if (key == QStringLiteral("added")) {
            // no removal before upload
            if (mWaitingForUpload.contains(projectName)) continue;

            QSet<QString> obsolateFiles;
            for(MerginFile file: files.value(key)) {
                obsolateFiles.insert(file.path);
            }
            mObsoleteFiles.insert(projectPath, obsolateFiles);
        } else {
            for(MerginFile file: files.value(key)) {
                QJsonObject fileObject;
                fileObject.insert("path", file.path);
                fileObject.insert("checksum", file.checksum);
                fileArray.append(fileObject);
            }
        }
    }

    jsonDoc.setArray(fileArray);
    downloadProjectFiles(projectName, jsonDoc.toJson(QJsonDocument::Compact));
}

void MerginApi::uploadInfoReplyFinished()
{
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(r);

    QHash<QString, QList<MerginFile>> files = parseAndCompareProjectFiles(r, false);
    QUrl url = r->url();
    QStringList res = url.path().split("/");
    QString projectName;
    projectName = res.last();

    r->deleteLater();

    QJsonDocument jsonDoc;
    QJsonObject changes;

    QList<MerginFile> filesToUpload;
    for (QString key: files.keys()) {
        QJsonArray jsonArray;
        for (MerginFile file: files.value(key)) {
            QJsonObject fileObject;
            fileObject.insert("path", file.path);
            fileObject.insert("checksum", file.checksum);
            jsonArray.append(fileObject);

            if (key != "removed") {
                 filesToUpload.append(file);
            }
        }
        changes.insert(key, jsonArray);
    }

    jsonDoc.setObject(changes);
    uploadProjectFiles(projectName, jsonDoc.toJson(QJsonDocument::Compact), filesToUpload);
}

QHash<QString, QList<MerginFile>> MerginApi::parseAndCompareProjectFiles(QNetworkReply *r, bool isForUpdate)
{
    QList<MerginFile> added;
    QList<MerginFile> updatedFiles;
    QList<MerginFile> renamed;
    QList<MerginFile> removed;

    QHash<QString, QList<MerginFile>> files;

    QUrl url = r->url();
    QStringList res = url.path().split("/");
    QString projectName;
    projectName = res.last();
    QString projectPath = QString(mDataDir + projectName + "/");

    if (r->error() == QNetworkReply::NoError)
    {
      QByteArray data = r->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(data);
      if (doc.isObject()) {
          QJsonObject docObj = doc.object();
          QString updated = docObj.value("updated").toString();
          auto it = docObj.constFind("files");
          QJsonValue v = *it;
          Q_ASSERT( v.isArray() );
          QJsonArray vArray = v.toArray();

          QSet<QString> localFiles = listFiles(projectPath);
          for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it)
          {
            QJsonObject projectInfoMap = it->toObject();
            QString serverChecksum = projectInfoMap.value("checksum").toString();
            QString path = projectInfoMap.value("path").toString();
            QByteArray localChecksumBytes = getChecksum(projectPath + path);
            QString localChecksum = QString::fromLatin1(localChecksumBytes.data(), localChecksumBytes.size());

            // removed
            if (localChecksum.isEmpty()) {
                MerginFile file;
                file.checksum = serverChecksum;
                file.path = path;
                removed.append(file);
            }
            // updated
            else if (serverChecksum != localChecksum) {
                MerginFile file;
                // if updated file is required from server, it has to have server checksum
                // if updated file is going to be upload, it has to have local checksum
                if (isForUpdate) {
                    file.checksum = serverChecksum;
                } else {
                    file.checksum = localChecksum;
                }

                file.path = path;
                updatedFiles.append(file);
            }

            localFiles.remove(path);
          }

          // Rest of localFiles are newly added
          for (QString p: localFiles) {
              MerginFile file;
              QByteArray localChecksumBytes = getChecksum(projectPath + p);
              QString localChecksum = QString::fromLatin1(localChecksumBytes.data(), localChecksumBytes.size());
              file.checksum = localChecksum;
              file.path = p;
              added.append(file);
          }
      }

      files.insert("added", added);
      files.insert("updated", updatedFiles);
      files.insert("removed", removed);
      files.insert("renamed", renamed);
    }
    else {
        QString message = QStringLiteral("Network API error: %1(): %2").arg("listProjects", r->errorString());
        qDebug("%s", message.toStdString().c_str());
        emit networkErrorOccurred( r->errorString(), "Mergin API error: projectInfo" );
    }
    return files;
}

ProjectList MerginApi::parseProjectsData(const QByteArray &data, bool dataFromServer)
{
    ProjectList result;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isArray()) {
        QJsonArray vArray = doc.array();

        for ( auto it = vArray.constBegin(); it != vArray.constEnd(); ++it)
        {
            QJsonObject projectMap = it->toObject();
            MerginProject p;
            p.name = projectMap.value("name").toString();
            QJsonValue tags = projectMap.value("tags");
            if (tags.isArray()) {
                for (QJsonValueRef tag: tags.toArray()) {
                    p.tags.append(tag.toString());
                }
                tags.toArray().toVariantList();
            }
            p.created = QDateTime::fromString(projectMap.value("created").toString(), Qt::ISODateWithMs).toUTC();
            QDateTime updated = QDateTime::fromString(projectMap.value("updated").toString(), Qt::ISODateWithMs).toUTC();

            if (dataFromServer) {
                if (!updated.isValid()) {
                    updated = p.created;
                }
                p.serverUpdated = updated;
            } else {
                p.lastSync = QDateTime::fromString(projectMap.value("lastSync").toString(), Qt::ISODateWithMs);
                p.updated = updated;
            }
            result << std::make_shared<MerginProject>(p);
        }
    }
    return result;
}

bool MerginApi::cacheProjectsData(const QByteArray &data)
{
    QFile file(mDataDir + mCacheFile);
    createPathIfNotExists(mDataDir + mCacheFile);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(data);
    file.close();

    return true;
}

void MerginApi::cacheProjects()
{
    QJsonDocument doc;
    QJsonArray array;
    for (std::shared_ptr<MerginProject> p: mMerginProjects) {
        QJsonObject projectMap;
        projectMap.insert("created", p->created.toString(Qt::ISODateWithMs));
        projectMap.insert("updated", p->updated.toString(Qt::ISODateWithMs));
        projectMap.insert("lastSync", p->lastSync.toString(Qt::ISODateWithMs));
        projectMap.insert("name", p->name);
        QJsonArray tags;
        projectMap.insert("tags", tags.fromStringList(p->tags));
        array.append(projectMap);
    }
    doc.setArray(array);
    cacheProjectsData(doc.toJson());
}

void MerginApi::continueWithUpload(QString projectDir, QString projectName, bool successfully)
{
    Q_UNUSED(projectDir)

    disconnect(this, &MerginApi::syncProjectFinished, this, &MerginApi::continueWithUpload);
    mWaitingForUpload.remove(projectName);
    if (!hasAuthData()) {
        emit networkErrorOccurred( "Auth data is invalid", "Mergin API error: projectInfo" );
    }

    if (!successfully) {
        return;
    }

    QByteArray token = generateToken();
    QNetworkRequest request;
    QUrl url(mApiRoot + "/v1/project/" + projectName);

    request.setUrl(url);
    request.setRawHeader("Authorization", QByteArray("Basic " + token));

    QNetworkReply *reply = mManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MerginApi::uploadInfoReplyFinished);
}

void MerginApi::handleDataStream(QNetworkReply* r, QString projectDir, bool overwrite)
{
    // Read content type from reply's header
    QByteArray contentType;
    QString contentTypeString;
    QList<QByteArray> headerList = r->rawHeaderList();
    QString headString;
    foreach(QByteArray head, headerList) {
        headString = QString::fromLatin1(head.data(), head.size());
        if (headString == QStringLiteral("Content-Type")) {
            contentType = r->rawHeader(head);
            contentTypeString = QString::fromLatin1(contentType.data(), contentType.size());
        }
    }

    // Read boundary hash from content types
    QString boundary;
    QRegularExpression re;
    re.setPattern("[^;]+; boundary=(?<boundary>.+)");
    QRegularExpressionMatch match = re.match(contentTypeString);
    if (match.hasMatch()) {
        boundary = match.captured("boundary");
    }

    // Depends on boundary size itself + special chars which number may vary
    int boundarySize = boundary.length() + 8;
    QRegularExpression boundaryPattern("(\r\n)?--" + boundary + "\r\n");
    QRegularExpression headerPattern("Content-Disposition: form-data; name=\"(?P<name>[^'\"]+)\"(; filename=\"(?P<filename>[^\"]+)\")?\r\n(Content-Type: (?P<content_type>.+))?\r\n");
    QRegularExpression endPattern("(\r\n)?--" + boundary +  "--\r\n");

    QByteArray data;
    QString dataString;
    QString activeFilePath;
    QFile activeFile;

    while (true) {
        QByteArray chunk = r->read(CHUNK_SIZE);
        if (chunk.isEmpty()) {
            // End of stream - write rest of data to active file
            if (!activeFile.fileName().isEmpty()) {
                QRegularExpressionMatch endMatch = endPattern.match(data);
                int tillIndex = data.indexOf(endMatch.captured(0));
                saveFile(data.left(tillIndex), activeFile, true);
            }
            return;
        }

        data = data.append(chunk);
        dataString = QString::fromLatin1(data.data(), data.size());
        QRegularExpressionMatch boundaryMatch = boundaryPattern.match(dataString);

        while (boundaryMatch.hasMatch()) {
            if (!activeFile.fileName().isEmpty()) {
                int tillIndex = data.indexOf(boundaryMatch.captured(0));
                saveFile(data.left(tillIndex), activeFile, true);
            }

            // delete previously written data with next boundary part
            int tillIndex = data.indexOf(boundaryMatch.captured(0)) + boundaryMatch.captured(0).length();
            data = data.remove(0, tillIndex);
            dataString = QString::fromLatin1(data.data(), data.size());

            QRegularExpressionMatch headerMatch = headerPattern.match(dataString);
            if (!headerMatch.hasMatch()) {
                qDebug() << "Received corrupted header";
                data = data + r->read(CHUNK_SIZE);
                dataString = QString::fromLatin1(data.data(), data.size());
            }
            headerMatch = headerPattern.match(dataString);
            data = data.remove(0, headerMatch.captured(0).length());
            dataString = QString::fromLatin1(data.data(), data.size());
            QString filename = headerMatch.captured("filename");

            activeFilePath = projectDir + "/" + filename;
            activeFile.setFileName(activeFilePath);
            if (activeFile.exists()) {
                if (overwrite) {
                    // Remove file if want to override
                    activeFile.remove();
                }
                else {
                    int i = 0;
                    QString newPath =  activeFilePath + "_conflict_copy";
                    while (QFile::exists(newPath + QString::number(i)))
                        ++i;
                    if (!QFile::copy(activeFilePath, newPath + QString::number(i))) {
                        qDebug() << activeFilePath << " will be overwritten";
                    }
                    activeFile.remove();
                }
            } else {
                createPathIfNotExists(activeFilePath);
            }

            boundaryMatch = boundaryPattern.match(dataString);
        }

        // Write rest of chunk to file
        if (!activeFile.fileName().isEmpty()) {
            saveFile(data.left(data.size() - boundarySize), activeFile, false);
        }
        data = data.remove(0, data.size() - boundarySize);
    }
}

bool MerginApi::saveFile(const QByteArray &data, QFile &file, bool closeFile)
{
    if (!file.isOpen()) {
        if (!file.open(QIODevice::Append)) {
            return false;
        }
    }

    file.write(data);
    if (closeFile)
        file.close();

    return true;
}

void MerginApi::createPathIfNotExists(QString filePath)
{
    QDir dir;
    if (!dir.exists(mDataDir))
        dir.mkpath(mDataDir);

    QFileInfo newFile( filePath );
    if ( !newFile.absoluteDir().exists() )
    {
      if ( !QDir( dir ).mkpath( newFile.absolutePath() ) )
        qDebug() << "Creating folder failed";
    }
}

ProjectStatus MerginApi::getProjectStatus(QDateTime localUpdated, QDateTime updated, QDateTime lastSync, QDateTime lastModified)
{
    if (!localUpdated.isValid()) {
        return ProjectStatus::NoVersion;
    }

    if (lastSync < lastModified) {
        return ProjectStatus::Modified;
    }

    if (localUpdated < updated && updated > lastSync.toUTC()) {
        return ProjectStatus::OutOfDate;
    }

    return ProjectStatus::UpToDate;
}

QByteArray MerginApi::getChecksum(QString filePath) {
    QFile f(filePath);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        QByteArray chunk = f.read(CHUNK_SIZE);
        while (!chunk.isEmpty()) {
           hash.addData(chunk);
           chunk = f.read(CHUNK_SIZE);
        }
        f.close();
        return hash.result().toHex();
    }

    return QByteArray();
}

QSet<QString> MerginApi::listFiles(QString path)
{
    QSet<QString> files;
    QDirIterator it(path, QStringList() << QStringLiteral("*"), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        if (!mIgnoreFiles.contains(it.fileInfo().suffix())) {
            files << it.filePath().replace(path, "");
        }
    }
    return files;
}
