#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <string>
#include <vector>

// Estructura limpia para representar una fila de datos financieros extraídos
struct ParsedTransaction {
    std::string date;
    double amount;
    std::string category;
    std::string description;
    bool is_valid;
};

class DataParser {
public:
    DataParser();
    ~DataParser();

    // Limpia caracteres de control y espacios innecesarios (Sanitización básica)
    std::string sanitize_token(const std::string& token) const;

    // Parser offline para líneas con formato estándar CSV (Fecha,Monto,Categoría,Descripción)
    ParsedTransaction parse_csv_line(const std::string& line) const;
};

#endif // DATA_PARSER_H
