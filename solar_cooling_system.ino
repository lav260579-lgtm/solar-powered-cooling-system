#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT22 sensor configuration
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Output pins
#define PELTIER_RELAY 8
#define COOLING_FAN 9

// Temperature settings
const float TEMP_ON = 15.0;
const float TEMP_OFF = 12.0;

// Cooling status
bool coolingState = false;

// Relay configuration (active LOW)
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

void setup() {

  Serial.begin(9600);

  // Configure output pins
  pinMode(PELTIER_RELAY, OUTPUT);
  pinMode(COOLING_FAN, OUTPUT);

  // Initially turn cooling OFF
  digitalWrite(PELTIER_RELAY, RELAY_OFF);
  digitalWrite(COOLING_FAN, LOW);

  // Start sensor
  dht.begin();

  // Start LCD
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Solar Cooling");

  lcd.setCursor(0, 1);
  lcd.print("System Starting");

  delay(2000);
  lcd.clear();
}

void loop() {

  // Read sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check sensor readings
  if (isnan(temperature) || isnan(humidity)) {

    coolingState = false;

    digitalWrite(PELTIER_RELAY, RELAY_OFF);
    digitalWrite(COOLING_FAN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");

    lcd.setCursor(0, 1);
    lcd.print("Check DHT22");

    Serial.println("Sensor Error");

    delay(2000);
    return;
  }

  // Automatic temperature control
  if (temperature >= TEMP_ON) {
    coolingState = true;
  }

  else if (temperature <= TEMP_OFF) {
    coolingState = false;
  }

  // Control Peltier relay
  digitalWrite(
    PELTIER_RELAY,
    coolingState ? RELAY_ON : RELAY_OFF
  );

  // Control cooling fan through its driver
  digitalWrite(
    COOLING_FAN,
    coolingState ? HIGH : LOW
  );

  // Display temperature
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("% ");

  // Display cooling status
  lcd.setCursor(0, 1);
  lcd.print("Cooling: ");

  if (coolingState) {
    lcd.print("ON ");
  }
  else {
    lcd.print("OFF");
  }

  // Serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);

  Serial.print(" C | Humidity: ");
  Serial.print(humidity);

  Serial.print(" % | Cooling: ");

  Serial.println(
    coolingState ? "ON" : "OFF"
  );

  delay(2000);
}
