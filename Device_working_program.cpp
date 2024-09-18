#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <BluetoothSerial.h>
#include <IRsensor.h>  // Custom IR sensor library
#include <math.h>

BluetoothSerial SerialBT;   // Bluetooth communication
Adafruit_SSD1306 display(128, 64, &Wire); // OLED display object

#define IR_EMITTER_PIN 25
#define IR_RECEIVER_PIN 26
#define AIR_PUMP_PIN 32
#define PRESSURE_SENSOR_PIN 34
#define BUTTON_PIN 12

// Constants
float cctValue = 0;  // Holds CCT value
float iopValue = 0;  // Holds IOP value

// CCT and IOP formulas
float calculateCCT(float irReadings[]) {
  // CCT calculation logic using IR sensor readings
  float cct = /* formula */; 
  return cct;
}

float calculateIOP(float pressure, float cct) {
  // IOP calculation logic using pressure and CCT value
  float iop = pressure + (factor * cct); // Insert IOP formula
  return iop;
}

void setup() {
  // Initialize Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  // Initialize Bluetooth
  SerialBT.begin("IOP_Device"); // Bluetooth name
  display.println("Bluetooth pairing...");
  display.display();

  // Wait for Bluetooth to connect
  while (!SerialBT.connected()) {
    delay(1000);
  }
  display.clearDisplay();
  display.println("Bluetooth connected");
  display.display();

  // Initialize sensors and air pump
  pinMode(IR_EMITTER_PIN, OUTPUT);
  pinMode(IR_RECEIVER_PIN, INPUT);
  pinMode(AIR_PUMP_PIN, OUTPUT);
  pinMode(PRESSURE_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  // Step 3: Measure CCT
  display.clearDisplay();
  display.println("Press button to start CCT measurement");
  display.display();

  if (digitalRead(BUTTON_PIN) == HIGH) {
    // Turn on IR sensors
    digitalWrite(IR_EMITTER_PIN, HIGH);
    
    float irReadings[10];  // Store multiple readings

    // Collect IR sensor data for CCT calculation
    for (int i = 0; i < 10; i++) {
      irReadings[i] = analogRead(IR_RECEIVER_PIN);
      delay(50);
    }
    
    // Calculate CCT
    cctValue = calculateCCT(irReadings);
    
    // Display CCT value
    display.clearDisplay();
    display.print("CCT: ");
    display.print(cctValue);
    display.display();
    
    // Send CCT data to mobile app via Bluetooth
    SerialBT.print("CCT: ");
    SerialBT.println(cctValue);
  }

  // Step 6: Measure IOP
  display.clearDisplay();
  display.println("Starting IOP measurement...");
  display.display();

  float pressure = 0;
  float maxIntensity = 0;

  // Turn on IR sensors and air pump
  digitalWrite(IR_EMITTER_PIN, HIGH);
  digitalWrite(AIR_PUMP_PIN, HIGH);

  // Start measuring IOP
  while (pressure <= 40) {  // Pressure increases until flat cornea is detected
    pressure = analogRead(PRESSURE_SENSOR_PIN);  // Simulated pressure reading
    float intensity = analogRead(IR_RECEIVER_PIN);  // IR intensity reading

    if (intensity > maxIntensity) {
      maxIntensity = intensity;  // Store peak intensity
    }
    
    if (intensity == maxIntensity) {
      // Flat cornea detected, stop air pump
      digitalWrite(AIR_PUMP_PIN, LOW);
      break;
    }
  }

  // Calculate IOP using pressure and CCT
  iopValue = calculateIOP(pressure, cctValue);

  // Display IOP value
  display.clearDisplay();
  display.print("IOP: ");
  display.print(iopValue);
  display.display();

  // Send IOP value to mobile app
  SerialBT.print("IOP: ");
  SerialBT.println(iopValue);

  // Reset for next reading
  delay(1000);
}