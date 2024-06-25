#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Kargaonkar";
const char* password = "8208087696";

const char* serverName = "http://192.168.29.63:3000/get_data"; // Update this to the correct server address

unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // Set the delay to 5 seconds
int counter = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP POST request every timerDelay ms
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status() == WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);
      String timeElapsed = formatTime(millis());

      // Create JSON payload
      String jsonPayload = "{\"counter\":" + String(counter) + ",\"timeElapsed\":" + timeElapsed + "}";

      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonPayload);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Increment the counter
      counter++;

      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String formatTime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  seconds = seconds % 60;
  minutes = minutes % 60;
  hours = hours % 24; // Optional: to wrap around after 24 hours

  char timeString[10]; // Enough space for "HH:MM:SS\0"
  sprintf(timeString, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  
  return String(timeString);
}