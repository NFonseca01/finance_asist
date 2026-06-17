#ifndef CRYPTO_ENGINE_H
#define CRYPTO_ENGINE_H

#include <string>

class CryptoEngine {
public:
    CryptoEngine();
    ~CryptoEngine();

    // Calcula el hash SHA-256 crudo de un bloque de datos (transacciones, metadatos) [cite: 17]
    std::string generate_sha256(const std::string& data) const;

    // Encadena el hash del registro actual con el del registro anterior (Hash Chaining) [cite: 17]
    std::string chain_block(const std::string& current_hash, const std::string& previous_hash) const;

    // Valida si los datos coinciden con un hash esperado (Verificación contra manipulaciones)
    bool verify_integrity(const std::string& data, const std::string& expected_hash) const;
};

#endif // CRYPTO_ENGINE_H
