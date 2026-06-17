#include "DataParser.h"
#include <sstream>
#include <algorithm>

DataParser::DataParser() {}
DataParser::~DataParser() {}

std::string DataParser::sanitize_token(const std::string& token) const {
    std::string sanitized = token;
    // Eliminar espacios al inicio y al final
    sanitized.erase(sanitized.begin(), std::find_if(sanitized.begin(), sanitized.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    sanitized.erase(std::find_if(sanitized.rbegin(), sanitized.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), sanitized.end());
    
    // Eliminar comillas dobles si el token venía envuelto en ellas
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\"'), sanitized.end());
    
    return sanitized;
}

ParsedTransaction DataParser::parse_csv_line(const std::string& line) const {
    ParsedTransaction tx{"", 0.0, "Otros", "", false};
    if (line.empty() || line[0] == '#') return tx; // Ignorar líneas vacías o comentarios

    std::stringstream ss(line);
    std::string item;
    std::vector<std::string> tokens;

    // Segmentar por comas
    while (std::getline(ss, item, ',')) {
        tokens.push_back(sanitize_token(item));
    }

    // Validar estructura mínima esperada: Fecha, Monto, Categoría, Descripción
    if (tokens.size() >= 4) {
        tx.date = tokens[0];
        try {
            tx.amount = std::stod(tokens[1]);
            tx.category = tokens[2];
            tx.description = tokens[3];
            tx.is_valid = true;
        } catch (...) {
            tx.is_valid = false; // Error de conversión en el monto
        }
    }

    return tx;
}
