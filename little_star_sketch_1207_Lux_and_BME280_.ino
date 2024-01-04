#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VEML7700.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define BUZZER_PIN    A1  // Change as per your connection

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme;
Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();

// Music variables
int speakerPin = A0; // Replace with the actual pin connected to the speaker
int length = 15; // the number of notes
char notes[] = "ccggaagffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize I2C
  Wire.begin();

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();

  // Initialize BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1);
  }

  // Initialize VEML7700 sensor
  if (!veml7700.begin()) {
    Serial.println(F("Could not find a valid VEML7700 sensor, check wiring!"));
    while (1);
  }

  // Initialize Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  // Read data from BME280
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Convert to hPa

  // Read data from VEML7700
  float lux = veml7700.readLux();

  // Display data on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  display.print(F("Temp: "));
  display.print(temperature);
  display.println(F(" C"));

  display.print(F("Pressure: "));
  display.print(pressure);
  display.println(F(" hPa"));

  display.print(F("Humidity: "));
  display.print(humidity);
  display.println(F(" %"));

  display.print(F("Light: "));
  display.print(lux);
  display.println(F(" Lux"));

  display.display();

  // Buzzer plays music when light level is below a threshold
  if (lux < 5.0) { // Adjust this threshold as needed
    playMusic();
  }

  delay(1000); // Update every 2 seconds
}

void playMusic() {
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }

    // pause between notes
    delay(tempo / 2);
  }
}
