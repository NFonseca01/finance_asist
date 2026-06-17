#include "CryptoEngine.h"
#include <sstream>
#include <iomanip>
#include <vector>

CryptoEngine::CryptoEngine() {}
CryptoEngine::~CryptoEngine() {}

// Funciones auxiliares de manipulación de bits para SHA-256 estándar
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIGMA1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define sigma0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define sigma1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

std::string CryptoEngine::generate_sha256(const std::string& data) const {
    // Valores de inicialización de los 8 registros de estado (primeros 32 bits de las raíces cuadradas de los primeros 8 primos)
    unsigned int h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
    unsigned int h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;

    // Constantes K para SHA-256
    static const unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Pre-procesamiento y Padding
    std::vector<unsigned char> msg(data.begin(), data.end());
    unsigned long long bit_len = msg.size() * 8;
    
    msg.push_back(0x80); // Añadir bit 1 al final de los datos
    while ((msg.size() + 8) % 64 != 0) {
        msg.push_back(0x00); // Rellenar con ceros
    }
    
    // Añadir la longitud original en bits como un entero de 64 bits al final
    for (int i = 7; i >= 0; i--) {
        msg.push_back((bit_len >> (i * 8)) & 0xFF);
    }

    // Procesar el mensaje en bloques de 512 bits (64 bytes)
    for (size_t chunk = 0; chunk < msg.size(); chunk += 64) {
        unsigned int w[64];
        for (int i = 0; i < 16; i++) {
            w[i] = (msg[chunk + i*4] << 24) | (msg[chunk + i*4 + 1] << 16) | 
                   (msg[chunk + i*4 + 2] << 8) | (msg[chunk + i*4 + 3]);
        }
        for (int i = 16; i < 64; i++) {
            w[i] = sigma1(w[i-2]) + w[i-7] + sigma0(w[i-15]) + w[i-16];
        }

        unsigned int a = h0, b = h1, c = h2, d = h3, e = h4, f = h5, g = h6, h = h7;

        for (int i = 0; i < 64; i++) {
            unsigned int t1 = h + SIGMA1(e) + CH(e, f, g) + k[i] + w[i];
            unsigned int t2 = SIGMA0(a) + MAJ(a, b, c);
            h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }

        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e; h5 += f; h6 += g; h7 += h;
    }

    // Formatear la salida a hexadecimal estructurado
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << h0 << std::setw(8) << h1 << std::setw(8) << h2 << std::setw(8) << h3
       << std::setw(8) << h4 << std::setw(8) << h5 << std::setw(8) << h6 << std::setw(8) << h7;
    
    return ss.str();
}

std::string CryptoEngine::chain_block(const std::string& current_hash, const std::string& previous_hash) const {
    // Encadenamiento seguro: H( H(actual) + H(anterior) ) [cite: 17]
    return generate_sha256(current_hash + previous_hash);
}

bool CryptoEngine::verify_integrity(const std::string& data, const std::string& expected_hash) const {
    return generate_sha256(data) == expected_hash;
}
