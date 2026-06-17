#include <jni.h>
#include <string>
#include <sstream> // <--- Asegúrate de tener este header para manejar los strings
#include "FinanceCore.h"
#include "CryptoEngine.h"
#include "DatabaseManager.h"
#include "DataParser.h" 
#include "LedgerVerifier.h" // <--- NUEVO HEADER

DatabaseManager* dbManager = nullptr;

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_finance_app_NativeBridge_getVersion(JNIEnv* env, jobject /* this */) {
    FinanceCore core;
    std::string versionStr = core.version();
    return env->NewStringUTF(versionStr.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_finance_app_NativeBridge_hashData(JNIEnv* env, jobject /* this */, jstring dataIn) {
    if (!dataIn) return env->NewStringUTF("");
    const char* nativeChars = env->GetStringUTFChars(dataIn, nullptr);
    std::string dataStr(nativeChars);
    env->ReleaseStringUTFChars(dataIn, nativeChars);

    CryptoEngine crypto;
    return env->NewStringUTF(crypto.generate_sha256(dataStr).c_str());
}

JNIEXPORT jboolean JNICALL
Java_com_finance_app_NativeBridge_initDatabase(JNIEnv* env, jobject /* this */, jstring dbPathIn) {
    if (!dbPathIn) return JNI_FALSE;
    const char* pathChars = env->GetStringUTFChars(dbPathIn, nullptr);
    std::string pathStr(pathChars);
    env->ReleaseStringUTFChars(dbPathIn, pathChars);

    if (dbManager != nullptr) {
        delete dbManager;
    }

    dbManager = new DatabaseManager(pathStr);
    return dbManager->initialize_schema() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_finance_app_NativeBridge_saveTransaction(JNIEnv* env, jobject /* this */, 
                                                 jstring dateIn, jdouble amount, 
                                                 jstring categoryIn, jstring descIn, 
                                                 jstring prevHashIn) {
    if (dbManager == nullptr) return JNI_FALSE;

    const char* dChars = env->GetStringUTFChars(dateIn, nullptr);
    const char* cChars = env->GetStringUTFChars(categoryIn, nullptr);
    const char* descChars = descIn ? env->GetStringUTFChars(descIn, nullptr) : "";
    const char* phChars = env->GetStringUTFChars(prevHashIn, nullptr);

    std::string date(dChars);
    std::string category(cChars);
    std::string description(descChars);
    std::string prevHash(phChars);

    env->ReleaseStringUTFChars(dateIn, dChars);
    env->ReleaseStringUTFChars(categoryIn, cChars);
    if (descIn) env->ReleaseStringUTFChars(descIn, descChars);
    env->ReleaseStringUTFChars(prevHashIn, phChars);

    std::string payload = date + std::to_string(amount) + category + description;
    CryptoEngine crypto;
    std::string currentHash = crypto.generate_sha256(payload);

    bool success = dbManager->insert_transaction(date, amount, category, description, currentHash, prevHash);
    return success ? JNI_TRUE : JNI_FALSE;
}

// === AQUÍ SE CORRIGE EL BLOQUE FALTANTE ===
JNIEXPORT jint JNICALL
Java_com_finance_app_NativeBridge_parseAndLoadCSV(JNIEnv* env, jobject /* this */, jstring csvContentIn, jstring lastKnownHashIn) {
    if (dbManager == nullptr || !csvContentIn || !lastKnownHashIn) return 0;

    const char* csvChars = env->GetStringUTFChars(csvContentIn, nullptr);
    const char* hashChars = env->GetStringUTFChars(lastKnownHashIn, nullptr);
    std::string csvContent(csvChars);
    std::string currentPrevHash(hashChars);
    env->ReleaseStringUTFChars(csvContentIn, csvChars);
    env->ReleaseStringUTFChars(lastKnownHashIn, hashChars);

    std::stringstream ss(csvContent);
    std::string line;
    DataParser parser;
    CryptoEngine crypto;
    int importedCount = 0;

    while (std::getline(ss, line)) {
        ParsedTransaction tx = parser.parse_csv_line(line);
        if (!tx.is_valid) continue;

        std::string payload = tx.date + std::to_string(tx.amount) + tx.category + tx.description;
        std::string currentHash = crypto.generate_sha256(payload);

        bool success = dbManager->insert_transaction(tx.date, tx.amount, tx.category, tx.description, currentHash, currentPrevHash);
        if (success) {
            currentPrevHash = currentHash; 
            importedCount++;
        }
    }

    return importedCount;
}

}
