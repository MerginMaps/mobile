#include "dbmanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QVariant>

DBManager::DBManager(QObject *parent)
    : QObject(parent)
    , m_lastError("")
    , m_databasePath("")
{
    qDebug() << "DBManager constructor invoked";
}

DBManager::~DBManager()
{
    closeDatabase();
    qDebug() << "DBManager destructor";
}

/**
 * Inicializa la conexión con la base de datos SQLite
 */
bool DBManager::initializeDatabase(const QString &dbPath)
{
    qDebug() << "Initializing database:" << dbPath;

    // Validar que la ruta no esté vacía
    if (dbPath.isEmpty()) {
        setError("Ruta de base de datos vacía");
        return false;
    }

    // Crear conexión con ID único
    static int connectionCounter = 0;
    QString connectionName = QString("DBManager_%1").arg(++connectionCounter);

    m_database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_database.setDatabaseName(dbPath);
    m_databasePath = dbPath;

    // Intentar abrir la conexión
    if (!m_database.open()) {
        setError(QString("No se pudo abrir BD: %1").arg(m_database.lastError().text()));
        return false;
    }

    qDebug() << "Database opened successfully at:" << dbPath;

    // Cargar lista de tablas
    loadTableList();

    // Emitir señal de conexión exitosa
    emit databaseCreated(dbPath);
    emit connectionStatusChanged(true);

    return true;
}

/**
 * Cierra la conexión con la base de datos
 */
bool DBManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
        qDebug() << "Database closed";
    }

    m_tableList.clear();
    m_currentTable.clear();
    m_tableModel.reset();
    m_databasePath.clear();

    emit connectionStatusChanged(false);
    return true;
}

/**
 * Carga la lista de todas las tablas de la BD
 * Ejecuta: SELECT name FROM sqlite_master WHERE type='table' ORDER BY name
 */
void DBManager::loadTableList()
{
    m_tableList.clear();

    if (!m_database.isOpen()) {
        setError("Base de datos no está abierta");
        return;
    }

    QSqlQuery query(m_database);

    // Ejecutar consulta para obtener todas las tablas
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")) {
        setError(QString("Error cargando tablas: %1").arg(query.lastError().text()));
        qDebug() << "SQL Error:" << query.lastError().text();
        return;
    }

    int tableCount = 0;
    while (query.next()) {
        QString tableName = query.value(0).toString();

        // Filtrar tablas de sistema de SQLite
        if (!tableName.startsWith("sqlite_")) {
            m_tableList.append(tableName);
            tableCount++;
        }
    }

    qDebug() << "Loaded" << tableCount << "tables from database";
    emit tableListChanged();
}

/**
 * Establece la tabla actual y carga sus datos
 */
void DBManager::setCurrentTable(const QString &tableName)
{
    qDebug() << "Setting current table:" << tableName;

    if (!m_database.isOpen()) {
        setError("Base de datos no está abierta");
        return;
    }

    if (tableName.isEmpty()) {
        setError("Nombre de tabla vacío");
        return;
    }

    if (!isValidTableName(tableName)) {
        setError(QString("Tabla inválida: %1").arg(tableName));
        return;
    }

    if (m_currentTable == tableName) {
        return; // Ya es la tabla actual
    }

    m_currentTable = tableName;
    createTableModel();

    emit currentTableChanged();
}

/**
 * Crea un modelo de tabla dinámico para la tabla actual
 */
void DBManager::createTableModel()
{
    qDebug() << "Creating table model for:" << m_currentTable;

    // Crear nuevo modelo
    m_tableModel.reset(new QSqlTableModel(this, m_database));
    m_tableModel->setTable(m_currentTable);

    // Configurar estrategia de edición
    m_tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    // Seleccionar todos los datos
    if (!m_tableModel->select()) {
        setError(QString("Error cargando tabla: %1").arg(m_tableModel->lastError().text()));
        m_tableModel.reset();
        return;
    }

    qDebug() << "Table model created with" << m_tableModel->rowCount() << "rows";

    emit tableModelChanged();
    emit rowCountChanged();
}

/**
 * Agrega una nueva fila vacía a la tabla
 */
bool DBManager::addRow()
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return false;
    }

    int newRow = m_tableModel->rowCount();

    if (!m_tableModel->insertRow(newRow)) {
        setError(QString("Error insertando fila: %1").arg(m_tableModel->lastError().text()));
        return false;
    }

    qDebug() << "New row added at index:" << newRow;

    emit rowCountChanged();
    emit dataChanged();
    return true;
}

/**
 * Elimina una fila específica
 */
bool DBManager::removeRow(int row)
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return false;
    }

    if (row < 0 || row >= m_tableModel->rowCount()) {
        setError(QString("Índice de fila inválido: %1").arg(row));
        return false;
    }

    if (!m_tableModel->removeRow(row)) {
        setError(QString("Error eliminando fila: %1").arg(m_tableModel->lastError().text()));
        return false;
    }

    qDebug() << "Row removed at index:" << row;

    emit rowCountChanged();
    emit dataChanged();
    return true;
}

/**
 * Confirma todos los cambios en la base de datos
 */
bool DBManager::submitChanges()
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return false;
    }

    if (!m_tableModel->submitAll()) {
        setError(QString("Error guardando cambios: %1").arg(m_tableModel->lastError().text()));
        m_tableModel->revertAll();
        return false;
    }

    qDebug() << "All changes submitted successfully";

    emit dataChanged();
    return true;
}

/**
 * Revierte todos los cambios pendientes
 */
bool DBManager::revertChanges()
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return false;
    }

    m_tableModel->revertAll();
    qDebug() << "All changes reverted";

    return true;
}

/**
 * Obtiene los nombres de columnas de una tabla
 */
QStringList DBManager::getColumnNames(const QString &tableName) const
{
    QStringList columnNames;

    if (!m_database.isOpen()) {
        return columnNames;
    }

    QSqlRecord record = m_database.record(tableName);

    for (int i = 0; i < record.count(); ++i) {
        columnNames.append(record.fieldName(i));
    }

    return columnNames;
}

/**
 * Obtiene el número total de filas
 */
int DBManager::getRowCount() const
{
    if (!m_tableModel) {
        return 0;
    }
    return m_tableModel->rowCount();
}

/**
 * Obtiene el número total de columnas
 */
int DBManager::getColumnCount() const
{
    if (!m_tableModel) {
        return 0;
    }
    return m_tableModel->columnCount();
}

/**
 * Filtra la tabla con una expresión SQL
 */
void DBManager::filterTable(const QString &filterExpression)
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return;
    }

    qDebug() << "Applying filter:" << filterExpression;

    m_tableModel->setFilter(filterExpression);
    if (!m_tableModel->select()) {
        setError(QString("Error filtrando tabla: %1").arg(m_tableModel->lastError().text()));
    }
}

/**
 * Limpia todos los filtros
 */
void DBManager::clearFilter()
{
    if (!m_tableModel) {
        return;
    }

    qDebug() << "Clearing filter";

    m_tableModel->setFilter("");
    m_tableModel->select();
}

/**
 * Exporta los datos a CSV
 */
bool DBManager::exportToCSV(const QString &filePath)
{
    if (!m_tableModel) {
        setError("No hay tabla cargada para exportar");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(QString("No se pudo crear archivo: %1").arg(filePath));
        return false;
    }

    QTextStream out(&file);

    // Escribir encabezados
    for (int col = 0; col < m_tableModel->columnCount(); ++col) {
        if (col > 0) out << ",";
        out << m_tableModel->headerData(col, Qt::Horizontal).toString();
    }
    out << "\n";

    // Escribir datos
    for (int row = 0; row < m_tableModel->rowCount(); ++row) {
        for (int col = 0; col < m_tableModel->columnCount(); ++col) {
            if (col > 0) out << ",";
            QVariant value = m_tableModel->data(m_tableModel->index(row, col));
            out << value.toString();
        }
        out << "\n";
    }

    file.close();
    qDebug() << "Data exported to CSV:" << filePath;

    return true;
}

/**
 * Valida si el nombre de la tabla existe
 */
bool DBManager::isValidTableName(const QString &tableName) const
{
    return m_tableList.contains(tableName);
}

/**
 * Establece el último error y emite la señal
 */
void DBManager::setError(const QString &errorMessage)
{
    m_lastError = errorMessage;
    qWarning() << "DBManager Error:" << errorMessage;
    emit errorOccurred(errorMessage);
}