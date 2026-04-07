#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStringList>
#include <QSqlRecord>
#include <QSqlError>
#include <QtSql/qsqltablemodel.h>
#include <memory>

/**
 * @class DynamicDatabaseManager
 * @brief Gestor dinámico de base de datos SQLite para cargar tablas bajo demanda
 *
 * Proporciona funcionalidad para:
 * - Listar todas las tablas disponibles en la BD
 * - Cargar dinámicamente cualquier tabla
 * - Agregar, eliminar y modificar registros
 * - Manejar errores de BD de forma elegante
 */
class DynamicDatabaseManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList tableList READ getTableList NOTIFY tableListChanged)
    Q_PROPERTY(QString currentTable READ getCurrentTable WRITE setCurrentTable NOTIFY currentTableChanged)
    Q_PROPERTY(QSqlTableModel* tableModel READ getTableModel NOTIFY tableModelChanged)
    Q_PROPERTY(QString lastError READ getLastError NOTIFY errorOccurred)

public:
    explicit DynamicDatabaseManager(QObject *parent = nullptr);
    ~DynamicDatabaseManager();

    // Inicialización
    Q_INVOKABLE bool initializeDatabase(const QString &dbPath);
    Q_INVOKABLE bool closeDatabase();

    // Getters
    QStringList getTableList() const { return m_tableList; }
    QString getCurrentTable() const { return m_currentTable; }
    QSqlTableModel* getTableModel() const { return m_tableModel.get(); }
    QString getLastError() const { return m_lastError; }

    // Setters
    Q_INVOKABLE void setCurrentTable(const QString &tableName);

    // Operaciones CRUD
    Q_INVOKABLE bool addRow();
    Q_INVOKABLE bool removeRow(int row);
    Q_INVOKABLE bool submitChanges();
    Q_INVOKABLE bool revertChanges();

    // Información de tabla
    Q_INVOKABLE QStringList getColumnNames(const QString &tableName) const;
    Q_INVOKABLE int getRowCount() const;

    // Búsqueda y filtrado
    Q_INVOKABLE void filterTable(const QString &filterExpression);
    Q_INVOKABLE void clearFilter();

signals:
    void tableListChanged();
    void currentTableChanged();
    void tableModelChanged();
    void errorOccurred(const QString &errorMessage);
    void rowCountChanged();
    void dataChanged();

private:
    // Métodos privados
    void loadTableList();
    void createTableModel();
    bool isValidTableName(const QString &tableName) const;
    void setError(const QString &errorMessage);

    // Miembros
    QSqlDatabase m_database;
    std::unique_ptr<QSqlTableModel> m_tableModel;
    QStringList m_tableList;
    QString m_currentTable;
    QString m_lastError;
};