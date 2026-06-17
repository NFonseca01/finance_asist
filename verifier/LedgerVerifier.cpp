#include "LedgerVerifier.h"
#include "../crypto/CryptoEngine.h"
#include <sqlite3.h>
#include <iostream>

AuditResult LedgerVerifier::audit_ledger(DatabaseManager* db) {
    AuditResult result = {true, 0, "Ledger íntegro. Cadena ininterrumpida."};
    if (!db) {
        return {false, 0, "Error: Instancia de Base de Datos nula."};
    }

    sqlite3* handle = db->get_handle(); // Necesitaremos exponer el handle o crear un método de lectura secuencial
    if (!handle) {
        return {false, 0, "Error: No se pudo adquirir el handle de SQLite."};
    }

    const char* query = "SELECT date, amount, category, description, current_hash, prev_hash FROM transactions ORDER BY id ASC;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(handle, query, -1, &stmt, nullptr) != SQLITE_OK) {
        return {false, 0, "Error al preparar la consulta de auditoría."};
    }

    std::string expected_prev_hash = "0000000000000000000000000000000000000000000000000000000000000000";
    CryptoEngine crypto;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string date(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        double amount = sqlite3_column_double(stmt, 1);
        std::string category(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        std::string description(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        std::string db_current_hash(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        std::string db_prev_hash(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));

        // 1. Validar enlace con el bloque anterior
        if (db_prev_hash != expected_prev_hash) {
            result.is_intact = false;
            result.corruption_detail = "Ruptura detectada: El 'prev_hash' en la fecha " + date + " no coincide con el eslabón anterior.";
            break;
        }

        // 2. Regenerar el hash en frío para verificar si el contenido fue modificado
        std::string payload = date + std::to_string(amount) + category + description;
        std::string computed_hash = crypto.generate_sha256(payload);

        if (computed_hash != db_current_hash) {
            result.is_intact = false;
            result.corruption_detail = "Alteración detectada: Los datos del registro con fecha " + date + " fueron modificados externamente.";
            break;
        }

        // Avanzar el eslabón esperado
        expected_prev_hash = db_current_hash;
        result.verified_blocks++;
    }

    sqlite3_finalize(stmt);
    return result;
}
