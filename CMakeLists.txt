cmake_minimum_required(VERSION 3.18.1)

project("financer_asist")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    core
    crypto
    parser
    db
    bridge
)

# Compilación conjunta del motor nativo
# Compilación conjunta del motor nativo
add_library(
    financer_asist
    SHARED
    core/FinanceCore.cpp
    crypto/CryptoEngine.cpp
    db/DatabaseManager.cpp
    parser/DataParser.cpp
    verifier/LedgerVerifier.cpp   # <--- NUEVO MOTOR DE AUDITORÍA FORENSE
    bridge/native-lib.cpp
)

find_library(log-lib log)

target_link_libraries(
    financer_asist
    ${log-lib}
)
