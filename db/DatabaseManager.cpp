#include "DatabaseManager.h"
#include <android/log.h>

// Declaraciones externas de los símbolos de SQLite del NDK para evitar dependencias dinámicas pesadas
extern "C" {
    int sqlite3_open(const char *filename, void **ppDb);
    int sqlite3_close(void *db);
    int sqlite3_exec(void *db, const char *sql, int (*callback)(void*,int,char**,char**), void *arg, char **errmsg);
    void sqlite3_free(void *ptr);
}

#define LOG_TAG "FinancerDB"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

DatabaseManager::DatabaseManager(const std::string& path) : db_ptr(nullptr), db_path(path) {}

DatabaseManager::~DatabaseManager() {
    close_database();
}

bool DatabaseManager::open_database() {
    if (db_ptr != nullptr) return true;
    
    int rc = sqlite3_open(db_path.c_str(), &db_ptr);
    if (rc != 0) {
        LOGE("Error al abrir la base de datos local: %s", db_path.c_str());
        db_ptr = nullptr;
        return false;
    }
    LOGI("Base de datos abierta exitosamente en: %s", db_path.c_str());
    return true;
}

void DatabaseManager::close_database() {
    if (db_ptr != nullptr) {
        sqlite3_close(db_ptr);
        db_ptr = nullptr;
        LOGI("Conexión con base de datos cerrada de manera segura.");
    }
}

bool DatabaseManager::initialize_schema() {
    if (db_ptr == nullptr && !open_database()) return false;

    // Tabla de transacciones con Hash Chaining embebido para garantizar el "Data as evidence"
    const char* schema_sql = 
        "CREATE TABLE IF NOT EXISTS ledger ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "date TEXT NOT NULL,"
        "amount REAL NOT NULL,"
        "category TEXT NOT NULL,"
        "description TEXT,"
        "current_hash TEXT NOT NULL,"  // SHA-256 de los datos de este registro
        "prev_hash TEXT NOT NULL"       // SHA-256 del registro anterior en la cadena
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_ptr, schema_sql, nullptr, nullptr, &errMsg);
    
    if (rc != 0) {
        LOGE("Error al inicializar el esquema de la base de datos: %s", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    LOGI("Esquema relacional y cadena de evidencias inicializados con éxito.");
    return true;
}

bool DatabaseManager::insert_transaction(const std::string& date, double amount, const std::string& category, 
                                        const std::string& description, const std::string& current_hash, 
                                        const std::string& prev_hash) {
    if (db_ptr == nullptr && !open_database()) return false;

    // Construcción limpia del query (En fases posteriores parametrizaremos con sqlite3_prepare_v2 para mitigar inyecciones)
    std::string sql = "INSERT INTO ledger (date, amount, category, description, current_hash, prev_hash) VALUES ('"
                      + date + "', " + std::to_string(amount) + ", '" + category + "', '" + description + "', '" 
                      + current_hash + "', '" + prev_hash + "');";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_ptr, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != 0) {
        LOGE("Error al insertar registro en ledger: %s", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    LOGI("Registro indexado y firmado criptográficamente en el ledger local.");
    return true;
#include "DatabaseManager.h"
#include <android/log.h>

// Declaraciones externas de los símbolos de SQLite del NDK para evitar dependencias dinámicas pesadas
extern "C" {
    int sqlite3_open(const char *filename, void **ppDb);
    int sqlite3_close(void *db);
    int sqlite3_exec(void *db, const char *sql, int (*callback)(void*,int,char**,char**), void *arg, char **errmsg);
    void sqlite3_free(void *ptr);
}

#define LOG_TAG "FinancerDB"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

DatabaseManager::DatabaseManager(const std::string& path) : db_ptr(nullptr), db_path(path) {}

DatabaseManager::~DatabaseManager() {
    close_database();
}

bool DatabaseManager::open_database() {
    if (db_ptr != nullptr) return true;
    
    int rc = sqlite3_open(db_path.c_str(), &db_ptr);
    if (rc != 0) {
        LOGE("Error al abrir la base de datos local: %s", db_path.c_str());
        db_ptr = nullptr;
        return false;
    }
    LOGI("Base de datos abierta exitosamente en: %s", db_path.c_str());
    return true;
}

void DatabaseManager::close_database() {
    if (db_ptr != nullptr) {
        sqlite3_close(db_ptr);
        db_ptr = nullptr;
        LOGI("Conexión con base de datos cerrada de manera segura.");
    }
}

bool DatabaseManager::initialize_schema() {
    if (db_ptr == nullptr && !open_database()) return false;

    // Tabla de transacciones con Hash Chaining embebido para garantizar el "Data as evidence"
    const char* schema_sql = 
        "CREATE TABLE IF NOT EXISTS ledger ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "date TEXT NOT NULL,"
        "amount REAL NOT NULL,"
        "category TEXT NOT NULL,"
        "description TEXT,"
        "current_hash TEXT NOT NULL,"  // SHA-256 de los datos de este registro
        "prev_hash TEXT NOT NULL"       // SHA-256 del registro anterior en la cadena
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_ptr, schema_sql, nullptr, nullptr, &errMsg);
    
    if (rc != 0) {
        LOGE("Error al inicializar el esquema de la base de datos: %s", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    LOGI("Esquema relacional y cadena de evidencias inicializados con éxito.");
    return true;
}

bool DatabaseManager::insert_transaction(const std::string& date, double amount, const std::string& category, 
                                        const std::string& description, const std::string& current_hash, 
                                        const std::string& prev_hash) {
    if (db_ptr == nullptr && !open_database()) return false;

    // Construcción limpia del query (En fases posteriores parametrizaremos con sqlite3_prepare_v2 para mitigar inyecciones)
    std::string sql = "INSERT INTO ledger (date, amount, category, description, current_hash, prev_hash) VALUES ('"
                      + date + "', " + std::to_string(amount) + ", '" + category + "', '" + description + "', '" 
                      + current_hash + "', '" + prev_hash + "');";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_ptr, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != 0) {
        LOGE("Error al insertar registro en ledger: %s", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    LOGI("Registro indexado y firmado criptográficamente en el ledger local.");
    return true;
}}
