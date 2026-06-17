package com.finance.app

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 1. Inicialización automática de la DB en el directorio seguro del Sandbox
        val dbPath = "${filesDir.absolutePath}/ledger.db"
        val dbInitialized = NativeBridge.initDatabase(dbPath)
        Log.i("FinancerApp", "Ledger nativo inicializado en $dbPath: $dbInitialized")

        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    var amountInput by remember { mutableStateOf("") }
                    var categoryInput by remember { mutableStateOf("General") }
                    var descriptionInput by remember { mutableStateOf("") }
                    var statusMessage by remember { mutableStateOf("Ledger listo e inmutable.") }
                    var lastHash by remember { mutableStateOf("0000000000000000000000000000000000000000000000000000000000000000") }

                    val coreVersion = remember { NativeBridge.getVersion() }

                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(24.dp),
                        verticalArrangement = Arrangement.spacedBy(16.dp),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Text(
                            text = "Geo-Financer Engine v$coreVersion",
                            style = MaterialTheme.typography.headlineSmall,
                            color = MaterialTheme.colorScheme.primary
                        )

                        Card(
                            modifier = Modifier.fillMaxWidth(),
                            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant)
                        ) {
                            Column(modifier = Modifier.padding(16.dp)) {
                                Text(text = "Último bloque encadenado (Prev Hash):", style = MaterialTheme.typography.labelMedium)
                                Text(text = lastHash, style = MaterialTheme.typography.bodySmall, maxLines = 1)
                            }
                        }

                        OutlinedTextField(
                            value = amountInput,
                            onValueChange = { amountInput = it },
                            label = { Text("Monto ($)") },
                            modifier = Modifier.fillMaxWidth()
                        )

                        OutlinedTextField(
                            value = categoryInput,
                            onValueChange = { categoryInput = it },
                            label = { Text("Categoría") },
                            modifier = Modifier.fillMaxWidth()
                        )

                        OutlinedTextField(
                            value = descriptionInput,
                            onValueChange = { descriptionInput = it },
                            label = { Text("Descripción") },
                            modifier = Modifier.fillMaxWidth()
                        )

                        Button(
                            onClick = {
                                val amount = amountInput.toDoubleOrNull()
                                if (amount != null) {
                                    val currentDate = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault()).format(Date())
                                    
                                    // Inserción directa en el Core C++ vía JNI con Hash Chaining
                                    val success = NativeBridge.saveTransaction(
                                        date = currentDate,
                                        amount = amount,
                                        category = categoryInput,
                                        description = descriptionInput,
                                        prevHash = lastHash
                                    )

                                    if (success) {
                                        // Generamos de forma idéntica el payload local para actualizar el estado del prevHash simulado
                                        val payload = currentDate + amount.toString() + categoryInput + descriptionInput
                                        lastHash = NativeBridge.hashData(payload)
                                        statusMessage = "Transacción firmada e indexada con éxito."
                                        amountInput = ""
                                        descriptionInput = ""
                                    } else {
                                        statusMessage = "Error crítico al escribir en el Ledger Nativo."
                                    }
                                } else {
                                    statusMessage = "Por favor, introduce un monto válido."
                                }
                            },
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Text("Indexar Evidencia Financiera")
                        }

                        Text(
                            text = statusMessage,
                            style = MaterialTheme.typography.bodyMedium,
                            color = MaterialTheme.colorScheme.secondary
                        )
                    }
                }
            }
        }
    }
}
