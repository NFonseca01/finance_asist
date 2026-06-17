package com.finance.app

object NativeBridge {

    init {
        System.loadLibrary("financer_asist")
    }

    external fun getVersion(): String
    external fun hashData(data: String): String
    external fun initDatabase(dbPath: String): Boolean
    external fun saveTransaction(date: String, amount: Double, category: String, description: String, prevHash: String): Boolean
    external fun parseAndLoadCSV(csvContent: String, lastKnownHash: String): Int

    /**
     * Ejecuta una verificación criptográfica secuencial sobre todo el Ledger.
     * Retorna un string con formato "STATUS|BLOCKS|DETAIL"
     */
    external fun verifyLedger(): String
}
