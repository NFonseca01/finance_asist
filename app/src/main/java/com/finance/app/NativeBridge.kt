package com.finance.app

/**
 * Aduana de comunicación segura entre la JVM/ART de Android y el Core en C++.
 */
object NativeBridge {

    init {
        // Carga la librería compartida compilada por CMake
        System.loadLibrary("financer_asist")
    }

    /**
     * Devuelve la versión de compilación del motor nativo.
     */
    external fun getVersion(): String
}
