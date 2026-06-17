#include <jni.h>
#include <string>
#include "FinanceCore.h"
#include "CryptoEngine.h"
#include "DatabaseManager.h"

// Puntero global persistente durante el ciclo de vida de la app (Aislamiento Nativo)
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

// Inicializa la base de datos pasándole la ruta interna segura desde el almacenamiento de Android
JNIEXPORT jboolean JNICALL
Java_com_finance_app_NativeBridge_initDatabase(JNIEnv* env, jobject /* this */, jstring dbPathIn) {
    if (!dbPathIn) return JNI_FALSE;
    const char* pathChars = env->GetStringUTFChars(dbPathIn, nullptr);
    std::string pathStr(pathChars);
    env->ReleaseStringUTFChars(dbPathIn, pathChars);

    // Si ya existía una instancia, liberamos de forma segura
    if (dbManager != nullptr) {
        delete dbManager;
    }

    dbManager = new DatabaseManager(pathStr);
    return dbManager->initialize_schema() ? JNI_TRUE : JNI_FALSE;
}

// Inserta una transacción calculando de forma transparente su Hash e inmutabilidad
JNIEXPORT jboolean JNICALL
Java_com_finance_app_NativeBridge_saveTransaction(JNIEnv* env, jobject /* this */, 
                                                 jstring dateIn, jdouble amount, 
                                                 jstring categoryIn, jstring descIn, 
                                                 jstring prevHashIn) {
    if (dbManager == nullptr) return JNI_FALSE;

    // Conversión segura de strings entrantes
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

    // Generar el hash inmutable en memoria nativa antes de escribir
    // H (Fecha + Monto + Categoría + Descripción)
    std::string payload = date + std::to_string(amount) + category + description;
    CryptoEngine crypto;
    std::string currentHash = crypto.generate_sha256(payload);

    // Guardar en SQLite nativo
    bool success = dbManager->insert_transaction(date, amount, category, description, currentHash, prevHash);
    return success ? JNI_TRUE : JNI_FALSE;
}

}
