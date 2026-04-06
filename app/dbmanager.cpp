#include "dbmanager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QRegularExpression>
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

DBManager::DBManager(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
{
}

DBManager::~DBManager()
{
    closeDatabase();
}

bool DBManager::initializeDatabase(const QString &databaseName, const QString &databasePath)
{
    // Validar nombre de base de datos
    if (databaseName.isEmpty())
    {
        m_lastError = "El nombre de la base de datos no puede estar vacío";
        qWarning() << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }

    if (!isValidDatabaseName(databaseName))
    {
        m_lastError = QString("Nombre de base de datos inválido: '%1'. Solo se permiten letras, números y guiones bajos.").arg(databaseName);
        qWarning() << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }

    // Determinar ruta de almacenamiento
    QString dbDirectory = databasePath;
    if (dbDirectory.isEmpty() || !QDir(dbDirectory).exists())
    {
        dbDirectory = getDefaultDataPath();
        qInfo() << "Usando ruta de datos predeterminada:" << dbDirectory;
    }

    // Crear directorio si no existe
    QDir dir(dbDirectory);
    if (!dir.exists())
    {
        if (!dir.mkpath("."))
        {
            m_lastError = QString("No se pudo crear el directorio: %1").arg(dbDirectory);
            qWarning() << m_lastError;
            emit errorOccurred(m_lastError);
            return false;
        }
        qInfo() << "Directorio creado:" << dbDirectory;
    }

    // Construir ruta completa
    QString fullPath = dbDirectory;
    if (!fullPath.endsWith("/") && !fullPath.endsWith("\\"))
    {
        fullPath += "/";
    }
    fullPath += databaseName + ".db";

    // Normalizar rutas (compatible con Windows y Linux)
    fullPath = QDir::toNativeSeparators(fullPath);

    // Cerrar conexión anterior si existe
    if (m_isConnected)
    {
        closeDatabase();
    }

    // Crear conexión a SQLite
    m_database = QSqlDatabase::addDatabase("QSQLITE", databaseName);
    m_database.setDatabaseName(fullPath);

    // Intentar conectar
    if (!m_database.open())
    {
        m_lastError = QString("No se pudo conectar a la base de datos: %1").arg(m_database.lastError().text());
        qWarning() << m_lastError;
        m_isConnected = false;
        emit errorOccurred(m_lastError);
        return false;
    }

    // Guardar información
    m_databaseName = databaseName;
    m_fullDatabasePath = fullPath;
    m_databaseDirectory = dbDirectory;
    m_isConnected = true;

    // Obtener información del archivo
    QFileInfo fileInfo(fullPath);
    bool isNewDatabase = !fileInfo.exists() || fileInfo.size() == 0;

    if (isNewDatabase)
    {
        qInfo() << QString("✓ Nueva base de datos creada: %1").arg(databaseName);
        qInfo() << QString("  Ubicación: %1").arg(fullPath);
        qInfo() << QString("  Fecha: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    else
    {
        qInfo() << QString("✓ Conectado a base de datos existente: %1").arg(databaseName);
        qInfo() << QString("  Ubicación: %1").arg(fullPath);
        qInfo() << QString("  Tamaño: %1 bytes").arg(fileInfo.size());
    }

    emit connectionStatusChanged(true);
    emit databasePathChanged();
    emit databaseNameChanged();
    emit databaseCreated(fullPath);

    return true;
}

bool DBManager::createTable(const QString &tableName, const QVariantList &fields)
{
    // Validaciones
    if (tableName.isEmpty())
    {
        m_lastError = "El nombre de la tabla no puede estar vacío";
        emit errorOccurred(m_lastError);
        return false;
    }

    if (!isValidTableName(tableName))
    {
        m_lastError = QString("Nombre de tabla inválido: '%1'. Solo se permiten letras, números y guiones bajos.")
                          .arg(tableName);
        emit errorOccurred(m_lastError);
        return false;
    }

    if (fields.isEmpty())
    {
        m_lastError = "Debe especificar al menos un campo para la tabla";
        emit errorOccurred(m_lastError);
        return false;
    }

    if (!m_isConnected)
    {
        m_lastError = "No hay conexión a la base de datos. Primero ejecute initializeDatabase()";
        emit errorOccurred(m_lastError);
        return false;
    }

    // Verificar si la tabla ya existe
    if (tableExists(tableName))
    {
        m_lastError = QString("La tabla '%1' ya existe en la base de datos '%2'").arg(tableName, m_databaseName);
        emit errorOccurred(m_lastError);
        return false;
    }

    // Construir comando SQL CREATE TABLE
    QString sqlCommand = QString("CREATE TABLE %1 (").arg(tableName);

    // Procesar cada campo
    for (int i = 0; i < fields.count(); ++i)
    {
        const QVariantMap field = fields[i].toMap();

        // Validar que el campo tiene la estructura esperada
        if (!field.contains("name") || !field.contains("type"))
        {
            m_lastError = QString("Campo %1 incompleto. Debe tener 'name' y 'type'").arg(i + 1);
            emit errorOccurred(m_lastError);
            return false;
        }

        const QString fieldName = field["name"].toString().trimmed();
        const QString fieldType = field["type"].toString().toUpper().trimmed();

        // Validar nombre del campo
        if (!isValidFieldName(fieldName))
        {
            m_lastError = QString("Nombre de campo inválido: '%1'").arg(fieldName);
            emit errorOccurred(m_lastError);
            return false;
        }

        // Validar tipo de dato
        if (fieldType.isEmpty())
        {
            m_lastError = QString("Campo '%1' sin tipo especificado").arg(fieldName);
            emit errorOccurred(m_lastError);
            return false;
        }

        // Agregar la cláusula del campo
        sqlCommand += QString("\n  %1 %2").arg(fieldName, convertTypeToSQL(fieldType));

        // Agregar tamaño si es aplicable
        if (fieldType == "TEXT")
        {
            const QString size = field["size"].toString().trimmed();
            if (!size.isEmpty() && size.toInt() > 0)
            {
                sqlCommand += QString("(%1)").arg(size);
            }
        }

        // Agregar coma si no es el último campo
        if (i < fields.count() - 1)
        {
            sqlCommand += ",";
        }
    }

    sqlCommand += "\n);";

    qDebug() << "SQL Command:" << sqlCommand;

    // Ejecutar comando
    QSqlQuery query(m_database);
    if (!query.exec(sqlCommand))
    {
        m_lastError = QString("Error al crear tabla '%1': %2")
        .arg(tableName, query.lastError().text());
        qWarning() << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }

    qInfo() << QString("✓ Tabla '%1' creada exitosamente en la base de datos '%2' con %3 campos")
                   .arg(tableName, m_databaseName).arg(fields.count());

    emit tableCreated(tableName);
    emit tablesListChanged();

    return true;
}

QString DBManager::getLastError() const
{
    return m_lastError;
}

bool DBManager::tableExists(const QString &tableName) const
{
    if (!m_isConnected)
        return false;

    QSqlQuery query(m_database);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue(tableName);

    if (query.exec() && query.next())
    {
        return true;
    }
    return false;
}

QVariantList DBManager::getTablesList() const
{
    QVariantList tableList;

    if (!m_isConnected)
        return tableList;

    QSqlQuery query(m_database);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");

    while (query.next())
    {
        tableList.append(query.value(0).toString());
    }

    return tableList;
}

QStringList DBManager::getTableListAsStringList() const
{
    QStringList tableList;

    if (!m_isConnected)
        return tableList;

    QSqlQuery query(m_database);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");

    while (query.next())
    {
        tableList.append(query.value(0).toString());
    }

    return tableList;
}

QVariantList DBManager::getTableStructure(const QString &tableName) const
{
    QVariantList structure;

    if (!m_isConnected || !tableExists(tableName))
        return structure;

    QSqlQuery query(m_database);
    query.exec(QString("PRAGMA table_info(%1)").arg(tableName));

    while (query.next())
    {
        QVariantMap column;
        column["name"] = query.value(1).toString();
        column["type"] = query.value(2).toString();
        column["notnull"] = query.value(3).toBool();
        column["pk"] = query.value(5).toInt();

        structure.append(column);
    }

    return structure;
}

int DBManager::getTableRecordCount(const QString &tableName) const
{
    if (!m_isConnected || !tableExists(tableName))
        return -1;

    QSqlQuery query(m_database);
    query.exec(QString("SELECT COUNT(*) FROM %1").arg(tableName));

    if (query.next())
    {
        return query.value(0).toInt();
    }

    return -1;
}

bool DBManager::dropTable(const QString &tableName)
{
    if (!m_isConnected)
    {
        m_lastError = "No hay conexión a la base de datos";
        emit errorOccurred(m_lastError);
        return false;
    }

    if (!tableExists(tableName))
    {
        m_lastError = QString("La tabla '%1' no existe").arg(tableName);
        emit errorOccurred(m_lastError);
        return false;
    }

    QSqlQuery query(m_database);
    if (!query.exec(QString("DROP TABLE %1").arg(tableName)))
    {
        m_lastError = QString("Error al eliminar tabla '%1': %2")
        .arg(tableName, query.lastError().text());
        emit errorOccurred(m_lastError);
        return false;
    }

    qInfo() << QString("✓ Tabla '%1' eliminada exitosamente de la base de datos '%2'")
                   .arg(tableName, m_databaseName);

    emit tablesListChanged();

    return true;
}

void DBManager::closeDatabase()
{
    if (m_isConnected && m_database.isOpen())
    {
        m_database.close();
        m_isConnected = false;
        qInfo() << QString("✓ Base de datos '%1' cerrada correctamente").arg(m_databaseName);
        emit connectionStatusChanged(false);
    }
}

QString DBManager::getDatabaseInfo() const
{
    if (!m_isConnected)
        return "Sin conexión a base de datos";

    QStringList tables = getTableListAsStringList();
    QString info;
    info += QString("╔════════════════════════════════════════════════════╗\n");
    info += QString("║             INFORMACIÓN DE BASE DE DATOS            ║\n");
    info += QString("╠════════════════════════════════════════════════════╣\n");
    info += QString("║ Nombre: %1\n").arg(m_databaseName.leftJustified(42));
    info += QString("║ Ubicación: %1\n").arg(m_fullDatabasePath.leftJustified(38));
    info += QString("║ Estado: Conectada ✓\n");
    info += QString("║ Número de tablas: %1\n").arg(QString::number(tables.count()).leftJustified(32));

    if (!tables.isEmpty())
    {
        info += QString("║\n");
        info += QString("║ Tablas existentes:\n");
        for (int i = 0; i < tables.count(); ++i)
        {
            int recordCount = getTableRecordCount(tables[i]);
            info += QString("║   %1. %2 (%3 registros)\n").arg(i + 1).arg(tables[i]).arg(recordCount);
        }
    }

    info += QString("╚════════════════════════════════════════════════════╝\n");

    return info;
}

bool DBManager::isValidTableName(const QString &tableName) const
{
    if (tableName.isEmpty())
        return false;

    QRegularExpression validTableName("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return validTableName.match(tableName).hasMatch();
}

bool DBManager::isValidFieldName(const QString &fieldName) const
{
    return isValidTableName(fieldName);
}

bool DBManager::isValidDatabaseName(const QString &dbName) const
{
    if (dbName.isEmpty())
        return false;

    QRegularExpression validDbName("^[a-zA-Z0-9_-]+$");
    return validDbName.match(dbName).hasMatch();
}

QString DBManager::convertTypeToSQL(const QString &type) const
{
    const QString upperType = type.toUpper().trimmed();

    if (upperType == "INT" || upperType == "INTEGER")
    {
        return "INTEGER";
    }
    else if (upperType == "TEXT")
    {
        return "TEXT";
    }
    else if (upperType == "REAL" || upperType == "FLOAT" || upperType == "DOUBLE")
    {
        return "REAL";
    }
    else if (upperType == "DATE")
    {
        return "DATE";
    }
    else if (upperType == "BOOLEAN" || upperType == "BOOL")
    {
        return "INTEGER"; // SQLite usa INTEGER para booleanos (0/1)
    }
    else if (upperType == "BLOB")
    {
        return "BLOB";
    }
    else
    {
        qWarning() << QString("Tipo de dato no reconocido '%1'. Se usará TEXT por defecto.").arg(type);
        return "TEXT";
    }
}

QString DBManager::buildFieldClause(const QVariantMap &fieldMap) const
{
    const QString fieldName = fieldMap["name"].toString();
    const QString fieldType = convertTypeToSQL(fieldMap["type"].toString());
    const QString size = fieldMap["size"].toString();

    QString clause = QString("%1 %2").arg(fieldName, fieldType);

    if (fieldType == "TEXT" && !size.isEmpty())
    {
        clause += QString("(%1)").arg(size);
    }

    return clause;
}

QString DBManager::getDefaultDataPath() const
{
    // Obtener ruta estándar de datos de la aplicación
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // En Windows: C:/Users/Usuario/AppData/Local/MerginMaps/
    // En Linux: ~/.local/share/MerginMaps/
    // En macOS: ~/Library/Application Support/MerginMaps/

    return dataPath;
}