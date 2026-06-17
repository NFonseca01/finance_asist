#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>

class DatabaseManager {
private:
    void* db_ptr; // Puntero opaco a sqlite3 para evitar exponer el header de sqlite de forma global
    std::string db_path;

public:
    DatabaseManager(const std::string& path);
    ~DatabaseManager();

    // Abre la conexión con el archivo local de la base de datos
    bool open_database();
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <sqlite3.h>

class DatabaseManager {
private:
    sqlite3* db;
    std::string db_path;

public:
    DatabaseManager(const std::string& path);
    ~DatabaseManager();

    bool initialize_schema();
    bool insert_transaction(const std::string& date, double amount, 
                            const std::string& category, const std::string& description, 
                            const std::string& current_hash, const std::string& prev_hash);

    // <--- NUEVO GETTER PARA EL VERIFICADOR CRIPTOGRÁFICO
    sqlite3* get_handle() const { return db; }
};

#endif // DATABASE_MANAGER_H
