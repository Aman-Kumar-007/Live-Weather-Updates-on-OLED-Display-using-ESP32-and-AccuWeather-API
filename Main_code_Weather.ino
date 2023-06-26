#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define OLED_ADDR 0x3C // OLED display I2C address
#define OLED_SDA 21    // ESP32 SDA pin
#define OLED_SCL 22    // ESP32 SCL pin
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SH1106 display(21, 22);

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* apiKey = "your_accuWeather_API_key";
const String locationKey = "your_city_key";

const long utcOffsetInSeconds = 19800; // UTC offset for your time zone in seconds (5 hours and 30 minutes in this case)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SH1106_SWITCHCAPVCC);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connected to WiFi");
  display.display();
  delay(2000);

  // Initialize time client
  timeClient.begin();

  // Retrieve weather data
  String url = "http://dataservice.accuweather.com/currentconditions/v1/" + locationKey + "?apikey=" + apiKey;

  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    float temperature = doc[0]["Temperature"]["Metric"]["Value"];
    String weatherText = doc[0]["WeatherText"];

    // Update time
    timeClient.update();

    // Get time and date
    String time = timeClient.getFormattedTime();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Temperature: ");
    display.print(temperature);
    display.println(" C");
    display.print("Weather: ");
    display.println(weatherText);
    display.print("Time: ");
    display.println(time);
    display.display();
  } else {
    Serial.print("HTTP request failed with error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {
  // Update time
  timeClient.update();

  delay(1000); // Update the display every second
}
