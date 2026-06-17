plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.finance.app"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.finance.app"
        minSdk = 24 // Android 7.0 (Baja superficie, alto rendimiento) [cite: 3, 13]
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        vectorDrawables {
            useSupportLibrary = true
        }

        // 1. Flags del compilador de C++
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++17" // Estándar moderno [cite: 4]
                arguments += "-DANDROID_STL=c++_static" // Evita dependencias dinámicas externas
            }
        }

        // 2. Arquitecturas objetivo recomendadas para producción y pruebas
        ndk {
            abiFilters += listOf("arm64-v8a", "armeabi-v7a") [cite: 4]
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = true // Ofuscación de la capa Kotlin
            isShrinkResources = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
    buildFeatures {
        compose = true // UI declarativa ultra-ligera [cite: 3, 18]
    }
    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.1"
    }
    
    // 3. Vinculación con el CMakeLists.txt de la raíz del proyecto
    externalNativeBuild {
        cmake {
            path = file("../CMakeLists.txt") [cite: 5]
        }
    }
}

dependencies {
    // Jetpack Compose Core
    implementation("androidx.core:core-ktx:1.12.0")
    implementation("androidx.lifecycle:lifecycle-runtime-ktx:2.7.0")
    implementation("androidx.activity:activity-compose:1.8.2")
    
    // UI Toolkit (Compose)
    implementation(platform("androidx.compose:compose-bom:22023.10.01"))
    implementation("androidx.compose.ui:ui")
    implementation("androidx.compose.ui:ui-graphics")
    implementation("androidx.compose.ui:ui-tooling-preview")
    implementation("androidx.compose.material3:material3") // Material 3 ligero 
}
