#ifndef LEDGER_VERIFIER_H
#define LEDGER_VERIFIER_H

#include <string>
#include "../db/DatabaseManager.h"

struct AuditResult {
    bool is_intact;
    int verified_blocks;
    std::string corruption_detail;
};

class LedgerVerifier {
public:
    // Analiza toda la base de datos recreando los hashes para validar la cadena de custodia
    AuditResult audit_ledger(DatabaseManager* db);
};

#endif // LEDGER_VERIFIER_H
