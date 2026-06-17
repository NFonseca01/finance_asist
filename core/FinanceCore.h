#ifndef FINANCE_CORE_H
#define FINANCE_CORE_H

#include <string>

class FinanceCore {
public:
    FinanceCore();
    ~FinanceCore();
    std::string version() const;
};

#endif // FINANCE_CORE_H
