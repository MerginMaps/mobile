#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStringList>
#include <memory>

/**
 * @class DBManager
 * @brief Gestor de base de datos SQLite para la aplicación Mergin Maps
 *
 * Proporciona funcionalidad completa para:
 * - Inicializar y gestionar conexiones SQLite
 * - Listar todas las tablas disponibles
 * - Cargar dinámicamente tablas específicas
 * - Realizar operaciones CRUD (Create, Read, Update, Delete)
 * - Manejar errores y notificaciones
 *
 * Uso en QML:
 * @code
 * __dbManager.initializeDatabase("/path/to/db")
 * __dbManager.setCurrentTable("tabla_nombre")
 * __dbManager.addRow()
 * __dbManager.submitChanges()
 * @endcode
 */
class DBManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList tableList READ getTableList NOTIFY tableListChanged)
    Q_PROPERTY(QString currentTable READ getCurrentTable WRITE setCurrentTable NOTIFY currentTableChanged)
    Q_PROPERTY(QSqlTableModel* tableModel READ getTableModel NOTIFY tableModelChanged)
    Q_PROPERTY(QString lastError READ getLastError NOTIFY errorOccurred)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStatusChanged)

public:
    explicit DBManager(QObject *parent = nullptr);
    ~DBManager();

    // ============== INICIALIZACIÓN ==============
    /**
     * Inicializa la conexión con la base de datos SQLite
     * @param dbPath Ruta completa al archivo de BD
     * @return true si la inicialización fue exitosa
     */
    Q_INVOKABLE bool initializeDatabase(const QString &dbPath);

    /**
     * Cierra la conexión actual con la BD
     * @return true si se cerró correctamente
     */
    Q_INVOKABLE bool closeDatabase();

    // ============== GETTERS ==============
    QStringList getTableList() const { return m_tableList; }
    QString getCurrentTable() const { return m_currentTable; }
    QSqlTableModel* getTableModel() const { return m_tableModel.get(); }
    QString getLastError() const { return m_lastError; }
    bool isConnected() const { return m_database.isOpen(); }

    // ============== SETTERS ==============
    /**
     * Establece la tabla actual y carga sus datos
     * @param tableName Nombre de la tabla a cargar
     */
    Q_INVOKABLE void setCurrentTable(const QString &tableName);

    // ============== OPERACIONES CRUD ==============
    /**
     * Agrega una nueva fila vacía a la tabla actual
     * @return true si se agregó correctamente
     */
    Q_INVOKABLE bool addRow();

    /**
     * Elimina una fila específica de la tabla actual
     * @param row Índice de la fila a eliminar
     * @return true si se eliminó correctamente
     */
    Q_INVOKABLE bool removeRow(int row);

    /**
     * Confirma todos los cambios pendientes en la BD
     * @return true si los cambios se guardaron correctamente
     */
    Q_INVOKABLE bool submitChanges();

    /**
     * Revierte todos los cambios pendientes
     * @return true si se revirtieron correctamente
     */
    Q_INVOKABLE bool revertChanges();

    // ============== INFORMACIÓN DE TABLAS ==============
    /**
     * Obtiene los nombres de columnas de una tabla específica
     * @param tableName Nombre de la tabla
     * @return Lista de nombres de columnas
     */
    Q_INVOKABLE QStringList getColumnNames(const QString &tableName) const;

    /**
     * Obtiene el número total de filas en la tabla actual
     * @return Número de filas
     */
    Q_INVOKABLE int getRowCount() const;

    /**
     * Obtiene el número total de columnas en la tabla actual
     * @return Número de columnas
     */
    Q_INVOKABLE int getColumnCount() const;

    // ============== BÚSQUEDA Y FILTRADO ==============
    /**
     * Filtra la tabla usando una expresión WHERE SQL
     * @param filterExpression Expresión de filtro (ej: "edad > 18")
     */
    Q_INVOKABLE void filterTable(const QString &filterExpression);

    /**
     * Limpia todos los filtros aplicados
     */
    Q_INVOKABLE void clearFilter();

    // ============== UTILIDADES ==============
    /**
     * Exporta los datos actuales a un archivo CSV
     * @param filePath Ruta del archivo de destino
     * @return true si la exportación fue exitosa
     */
    Q_INVOKABLE bool exportToCSV(const QString &filePath);

    // ============== INFORMACIÓN ==============
    /*
    * Obtiene información completa de la base de datos (nombre y tablas)
    * @return String formateado con la información de la BD
    */
    Q_INVOKABLE QString getDatabaseInfo() const;

    /*
    * Obtiene la propiedad databaseName (nombre del archivo)
    * @return Nombre del archivo de la base de datos
    */
    Q_INVOKABLE QString getDatabaseName() const;

    /*
    * Verifica si una tabla existe
    * @param tableName Nombre de la tabla
    * @return true si existe
    */
    Q_INVOKABLE bool tableExists(const QString &tableName) const;

    /*
    * Crea una nueva tabla con los campos especificados
    * @param tableName Nombre de la tabla
    * @param fields Lista de campos {name, type}
    * @return true si se creó correctamente
    */
    Q_INVOKABLE bool createTable(const QString &tableName, const QVariantList &fields);

signals:
    // Señales de cambios en tabla
    void tableListChanged();
    void currentTableChanged();
    void tableModelChanged();
    void rowCountChanged();

    // Señales de notificación
    void errorOccurred(const QString &errorMessage);
    void tableCreated(const QString &tableName);
    void databaseCreated(const QString &databasePath);
    void connectionStatusChanged(bool connected);
    void dataChanged();

private:
    // ============== MÉTODOS PRIVADOS ==============
    /**
     * Carga la lista de todas las tablas de la BD
     * Ejecuta: SELECT name FROM sqlite_master WHERE type='table'
     */
    void loadTableList();

    /**
     * Crea un modelo de tabla dinámico para la tabla actual
     */
    void createTableModel();

    /**
     * Valida si el nombre de tabla existe
     * @param tableName Nombre de la tabla
     * @return true si la tabla existe
     */
    bool isValidTableName(const QString &tableName) const;

    /**
     * Establece el último error y emite la señal
     * @param errorMessage Mensaje de error
     */
    void setError(const QString &errorMessage);

    // ============== MIEMBROS ==============
    QSqlDatabase m_database;
    std::unique_ptr<QSqlTableModel> m_tableModel;
    QStringList m_tableList;
    QString m_currentTable;
    QString m_lastError;
    QString m_databasePath;
};