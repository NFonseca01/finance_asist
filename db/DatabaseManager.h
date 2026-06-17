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

    // Cierra la conexión de manera segura liberando recursos
    void close_database();

    // Inicializa las tablas del MVP (Transacciones y Log de hashes inmutables)
    bool initialize_schema();

    // Inserta un bloque financiero con su hash de datos y el hash del bloque anterior
    bool insert_transaction(const std::string& date, double amount, const std::string& category, 
                            const std::string& description, const std::string& current_hash, 
                            const std::string& prev_hash);
};

#endif // DATABASE_MANAGER_H
