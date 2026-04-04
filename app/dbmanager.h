#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlError>
#include <memory>

/**
 * @brief La clase DBManager gestiona operaciones de base de datos para la aplicación Mergin Maps.
 * Proporciona funcionalidad para crear y gestionar múltiples tablas con campos personalizados.
 *
 * Conecta con QML a través de Q_INVOKABLE para permitir operaciones desde la UI.
 */
class DBManager : public QObject
{
    Q_OBJECT

    // Propiedades expuestas a QML
    Q_PROPERTY(QString databasePath READ getDatabasePath NOTIFY databasePathChanged)
    Q_PROPERTY(QString databaseName READ getDatabaseName NOTIFY databaseNameChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStatusChanged)
    Q_PROPERTY(QStringList tablesList READ getTableListAsStringList NOTIFY tablesListChanged)

public:
    /**
     * @brief Constructor de DBManager
     * @param parent Puntero al objeto padre (para gestión automática de memoria)
     */
    explicit DBManager(QObject *parent = nullptr);

    /**
     * @brief Destructor de DBManager
     */
    ~DBManager();

    /**
     * @brief Inicializa la conexión a la base de datos con un nombre personalizado
     * @param databaseName Nombre de la base de datos (ej: "merginmaps_data")
     * @param databasePath Ruta completa donde se almacenará la base de datos
     *                     Si está vacío, usará la carpeta de datos de la aplicación
     * @return true si la conexión fue exitosa, false en caso contrario
     *
     * Ejemplo desde QML:
     * @code
     * dbManager.initializeDatabase("miBaseDatos", "E:/MyData/")
     * @endcode
     */
    Q_INVOKABLE bool initializeDatabase(const QString &databaseName, const QString &databasePath = "");
    /**
     * @brief Crea una nueva tabla en la base de datos con los campos especificados
     * @param tableName Nombre de la tabla a crear
     * @param fields Lista de QVariantMap con la estructura de los campos:
     *        Cada campo debe contener: {"name": "...", "type": "...", "size": "..."}
     * @return true si la tabla fue creada exitosamente, false en caso contrario
     *
     * Ejemplo de uso desde QML:
     * @code
     * var fields = [
     *   { name: "id", type: "INT", size: "" },
     *   { name: "nombre", type: "TEXT", size: "100" },
     *   { name: "precio", type: "REAL", size: "" }
     * ]
     * dbManager.createTable("productos", fields)
     * @endcode
     */
    Q_INVOKABLE bool createTable(const QString &tableName, const QVariantList &fields);

    /**
     * @brief Obtiene el último error de la base de datos
     * @return Mensaje de error en formato QString
     */
    Q_INVOKABLE QString getLastError() const;

    /**
     * @brief Verifica si la tabla existe en la base de datos
     * @param tableName Nombre de la tabla a verificar
     * @return true si la tabla existe, false en caso contrario
     */
    Q_INVOKABLE bool tableExists(const QString &tableName) const;

    /**
     * @brief Obtiene la lista de tablas existentes en la base de datos
     * @return QVariantList con los nombres de todas las tablas
     */
    Q_INVOKABLE QVariantList getTablesList() const;

    /**
     * @brief Obtiene la lista de tablas como QStringList (para QML)
     * @return QStringList con los nombres de todas las tablas
     */
    QStringList getTableListAsStringList() const;

    /**
     * @brief Obtiene información detallada de una tabla específica
     * @param tableName Nombre de la tabla
     * @return QVariantList con información de cada columna
     */
    Q_INVOKABLE QVariantList getTableStructure(const QString &tableName) const;

    /**
     * @brief Obtiene el número de registros en una tabla
     * @param tableName Nombre de la tabla
     * @return Número de registros, -1 si hay error
     */
    Q_INVOKABLE int getTableRecordCount(const QString &tableName) const;

    /**
     * @brief Elimina una tabla de la base de datos
     * @param tableName Nombre de la tabla a eliminar
     * @return true si la tabla fue eliminada, false en caso contrario
     */
    Q_INVOKABLE bool dropTable(const QString &tableName);

    /**
     * @brief Obtiene la ruta completa donde se almacena la base de datos
     * @return QString con la ruta completa del archivo .db
     */
    QString getDatabasePath() const { return m_fullDatabasePath; }

    /**
     * @brief Obtiene el nombre de la base de datos actual
     * @return QString con el nombre de la base de datos
     */
    QString getDatabaseName() const { return m_databaseName; }

    /**
     * @brief Verifica si hay conexión activa a la base de datos
     * @return true si está conectada, false en caso contrario
     */
    bool isConnected() const { return m_isConnected; }

    /**
     * @brief Cierra la conexión a la base de datos
     */
    Q_INVOKABLE void closeDatabase();

    /**
     * @brief Obtiene información de la base de datos como QString para mostrar
     * @return QString con información formateada (nombre, ruta, tablas, etc.)
     */
    Q_INVOKABLE QString getDatabaseInfo() const;

signals:
    /**
     * @brief Señal emitida cuando se crea una tabla exitosamente
     * @param tableName Nombre de la tabla creada
     */
    void tableCreated(const QString &tableName);

    /**
     * @brief Señal emitida cuando ocurre un error
     * @param errorMessage Mensaje descriptivo del error
     */
    void errorOccurred(const QString &errorMessage);

    /**
     * @brief Señal emitida cuando la conexión a la base de datos cambia de estado
     * @param connected true si está conectada, false si se desconectó
     */
    void connectionStatusChanged(bool connected);

    /**
     * @brief Señal emitida cuando se crea una nueva base de datos
     * @param databasePath Ruta completa de la base de datos creada
     */
    void databaseCreated(const QString &databasePath);

    /**
     * @brief Señal emitida cuando cambia la ruta de la base de datos
     */
    void databasePathChanged();

    /**
     * @brief Señal emitida cuando cambia el nombre de la base de datos
     */
    void databaseNameChanged();

    /**
     * @brief Señal emitida cuando cambia la lista de tablas
     */
    void tablesListChanged();

private:
    /**
     * @brief Valida que el nombre de la tabla sea válido (sin caracteres especiales, etc.)
     * @param tableName Nombre a validar
     * @return true si es válido, false en caso contrario
     */
    bool isValidTableName(const QString &tableName) const;

    /**
     * @brief Valida que el nombre de un campo sea válido
     * @param fieldName Nombre del campo a validar
     * @return true si es válido, false en caso contrario
     */
    bool isValidFieldName(const QString &fieldName) const;

    /**
     * @brief Valida que el nombre de la base de datos sea válido
     * @param dbName Nombre a validar
     * @return true si es válido, false en caso contrario
     */
    bool isValidDatabaseName(const QString &dbName) const;

    /**
     * @brief Convierte el tipo de dato especificado en QML a su equivalente SQL
     * @param type Tipo en formato QML (INT, TEXT, REAL, DATE, BOOLEAN)
     * @return Tipo SQL correspondiente
     */
    QString convertTypeToSQL(const QString &type) const;

    /**
     * @brief Construye la cláusula SQL para un campo individual
     * @param fieldMap QVariantMap con la información del campo
     * @return QString con la cláusula SQL del campo
     */
    QString buildFieldClause(const QVariantMap &fieldMap) const;

    /**
     * @brief Obtiene la ruta estándar de datos de la aplicación
     * @return QString con la ruta
     */
    QString getDefaultDataPath() const;

    // Miembros privados
    QSqlDatabase m_database;           ///< Conexión a la base de datos
    QString m_lastError;               ///< Almacena el último error ocurrido
    QString m_databaseName;            ///< Nombre de la base de datos (sin extensión)
    QString m_fullDatabasePath;        ///< Ruta completa con nombre (ej: E:/data/mibase.db)
    QString m_databaseDirectory;       ///< Directorio donde se almacena
    bool m_isConnected;                ///< Estado de la conexión
};

#endif // DBMANAGER_H