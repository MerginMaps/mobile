#include "dynamicdatabasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>
#include <QFile>

DynamicDatabaseManager::DynamicDatabaseManager(QObject *parent)
    : QObject(parent)
    , m_lastError("")
{
}

DynamicDatabaseManager::~DynamicDatabaseManager()
{
    closeDatabase();
}

/**
 * Inicializa la conexión con la base de datos SQLite
 */
bool DynamicDatabaseManager::initializeDatabase(const QString &dbPath)
{
    // Crear conexión
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        setError(QString("Error al abrir BD: %1").arg(m_database.lastError().text()));
        return false;
    }

    loadTableList();
    return true;
}

/**
 * Cierra la conexión con la base de datos
 */
bool DynamicDatabaseManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    m_tableList.clear();
    m_currentTable.clear();
    m_tableModel.reset();
    return true;
}

/**
 * Carga la lista de todas las tablas de la BD
 * Ejecuta: SELECT name FROM sqlite_master WHERE type='table'
 */
void DynamicDatabaseManager::loadTableList()
{
    m_tableList.clear();

    if (!m_database.isOpen()) {
        setError("Base de datos no está abierta");
        return;
    }

    QSqlQuery query(m_database);
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")) {
        setError(QString("Error cargando tablas: %1").arg(query.lastError().text()));
        return;
    }

    while (query.next()) {
        QString tableName = query.value(0).toString();
        // Filtrar tablas de sistema
        if (!tableName.startsWith("sqlite_")) {
            m_tableList.append(tableName);
        }
    }

    emit tableListChanged();
}

/**
 * Establece la tabla actual y carga sus datos
 */
void DynamicDatabaseManager::setCurrentTable(const QString &tableName)
{
    if (!m_database.isOpen()) {
        setError("Base de datos no está abierta");
        return;
    }

    if (!isValidTableName(tableName)) {
        setError(QString("Tabla inválida: %1").arg(tableName));
        return;
    }

    if (m_currentTable == tableName) {
        return;
    }

    m_currentTable = tableName;
    createTableModel();
    emit currentTableChanged();
}

/**
 * Crea un modelo de tabla dinámico para la tabla actual
 */
void DynamicDatabaseManager::createTableModel()
{
    m_tableModel.reset(new QSqlTableModel(this, m_database));
    m_tableModel->setTable(m_currentTable);
    m_tableModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    if (!m_tableModel->select()) {
        setError(QString("Error cargando tabla: %1").arg(m_tableModel->lastError().text()));
        m_tableModel.reset();
        return;
    }

    emit tableModelChanged();
    emit rowCountChanged();
}

/**
 * Agrega una nueva fila vacía a la tabla
 */
bool DynamicDatabaseManager::addRow()
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

    emit rowCountChanged();
    emit dataChanged();
    return true;
}

/**
 * Elimina una fila específica
 */
bool DynamicDatabaseManager::removeRow(int row)
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return false;
    }

    if (row < 0 || row >= m_tableModel->rowCount()) {
        setError("Índice de fila inválido");
        return false;
    }

    if (!m_tableModel->removeRow(row)) {
        setError(QString("Error eliminando fila: %1").arg(m_tableModel->lastError().text()));
        return false;
    }

    emit rowCountChanged();
    emit dataChanged();
    return true;
}

/**
 * Confirma todos los cambios en la base de datos
 */
bool DynamicDatabaseManager::submitChanges()
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

    emit dataChanged();
    return true;
}

/**
 * Revierte todos los cambios pendientes
 */
bool DynamicDatabaseManager::revertChanges()
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return false;
    }

    m_tableModel->revertAll();
    return true;
}

/**
 * Obtiene los nombres de columnas de una tabla
 */
QStringList DynamicDatabaseManager::getColumnNames(const QString &tableName) const
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
int DynamicDatabaseManager::getRowCount() const
{
    if (!m_tableModel) {
        return 0;
    }
    return m_tableModel->rowCount();
}

/**
 * Filtra la tabla con una expresión SQL
 */
void DynamicDatabaseManager::filterTable(const QString &filterExpression)
{
    if (!m_tableModel) {
        setError("No hay tabla cargada");
        return;
    }

    m_tableModel->setFilter(filterExpression);
    if (!m_tableModel->select()) {
        setError(QString("Error filtrando tabla: %1").arg(m_tableModel->lastError().text()));
    }
}

/**
 * Limpia el filtro de la tabla
 */
void DynamicDatabaseManager::clearFilter()
{
    if (!m_tableModel) {
        return;
    }

    m_tableModel->setFilter("");
    m_tableModel->select();
}

/**
 * Valida si el nombre de la tabla existe
 */
bool DynamicDatabaseManager::isValidTableName(const QString &tableName) const
{
    return m_tableList.contains(tableName);
}

/**
 * Establece el último error y emite la señal
 */
void DynamicDatabaseManager::setError(const QString &errorMessage)
{
    m_lastError = errorMessage;
    qWarning() << "DatabaseManager Error:" << errorMessage;
    emit errorOccurred(errorMessage);
}