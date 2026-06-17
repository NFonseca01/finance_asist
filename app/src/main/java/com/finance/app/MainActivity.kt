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
                        verticalArrangement = Arrangement.spacedBy(14.dp),
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
                            Column(modifier = Modifier.padding(12.dp)) {
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
                                    val success = NativeBridge.saveTransaction(
                                        date = currentDate,
                                        amount = amount,
                                        category = categoryInput,
                                        description = descriptionInput,
                                        prevHash = lastHash
                                    )

                                    if (success) {
                                        val payload = currentDate + amount.toString() + categoryInput + descriptionInput
                                        lastHash = NativeBridge.hashData(payload)
                                        statusMessage = "Transacción firmada con éxito."
                                        amountInput = ""
                                        descriptionInput = ""
                                    } else {
                                        statusMessage = "Error crítico en el Ledger Nativo."
                                    }
                                } else {
                                    statusMessage = "Introduce un monto válido."
                                }
                            },
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Text("Indexar Evidencia Individual")
                        }

                        // --- NUEVA SECCIÓN: INGESTA POR LOTES (OFFLINE CSV PARSER) ---
                        ElevatedButton(
                            onClick = {
                                // Datos mockeados simulando la extracción de un archivo local
                                val mockCsvDump = """
                                    2026-06-15 10:00:00,45.50,Alimentacion,Supermercado Central
                                    2026-06-16 14:20:00,120.00,Servicios,Suscripcion Servidor Cloud
                                    2026-06-17 09:15:00,15.00,Transporte,Carga Tarjeta Metro
                                """.trimIndent()

                                val recordsImported = NativeBridge.parseAndLoadCSV(mockCsvDump, lastHash)
                                if (recordsImported > 0) {
                                    statusMessage = "Procesados e indexados $recordsImported registros CSV."
                                    // Forzamos un hash dummy o una consulta de actualización posterior para refrescar la UI
                                    lastHash = NativeBridge.hashData("RELOAD_CHAIN_STATE")
                                } else {
                                    statusMessage = "El Parser no encontró registros válidos."
                                }
                            },
                            modifier = Modifier.fillMaxWidth(),
                            colors = ButtonDefaults.elevatedButtonColors(containerColor = MaterialTheme.colorScheme.tertiaryContainer)
                        ) {
                            Text("Simular Importación de Archivo (CSV)")
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
