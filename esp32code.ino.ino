#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// ================== WIFI CREDENTIALS ==================
#define WIFI_SSID "ch muhammad rohan"
#define WIFI_PASSWORD "ifti1234"

// =============== FIREBASE PROJECT CREDENTIALS ===============
#define API_KEY "AIzaSyDCiNbqDNSLAd6c5CEp5sxYrS-vskjB2iQ"
#define DATABASE_URL "https://esp32-iot-dashboard-2b939-default-rtdb.firebaseio.com/"  // no https:// at end

#define USER_EMAIL "testuser@gmail.com"
#define USER_PASSWORD "12345678"

// ================== FIREBASE OBJECTS ==================
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// =============== SENSOR PIN CONFIGURATION ===============
#define SENSOR_PIN 34  // LM35 OUT connected to D34

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi");

  // Firebase Configuration
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase connected");
}

void loop() {
  // Read LM35 analog value
  int rawValue = analogRead(34);
  float voltage = rawValue * (3.3 / 4095.0);       // Convert ADC to voltage
  float temperatureC = voltage * 100.0;            // 10mV/°C for LM35

  Serial.print("Temp: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // ========== Send Temperature to Firebase ==========
  if (Firebase.RTDB.setFloat(&fbdo, "/temperature", temperatureC)) {
    Serial.println("Temperature sent to Firebase");
  } else {
    Serial.print("Firebase Temp Error: ");
    Serial.println(fbdo.errorReason());
  }

  // ========== Send Alert Message ==========
  String alertMsg = (temperatureC > 19.0) ? "⚠️  Temprature going beyond threshold ! Check Environment." : "✅ Temperature Normal";
  if (Firebase.RTDB.setString(&fbdo, "/alert", alertMsg)) {
    Serial.println("Alert message sent");
  } else {
    Serial.print("Firebase Alert Error: ");
    Serial.println(fbdo.errorReason());
  }

  delay(5000);  // Wait 5 seconds before next read
}

