// Calibrating the load cell
#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 25; //data pin
const int LOADCELL_CLK_PIN = 26; //clk pin 
const double CALIBRATION_FACTOR = 0.0; 
const double KNOWN_WEIGHT = 0.0; 

HX711 scale;

bool doCalibrate = false; 
bool doRunTest = false; 

void setup() {
  Serial.begin(115200);
  rtc_cpu_freq_config_t config;

  //slow down the esp32 processor 
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);

  //initiate the load cell 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_CLK_PIN);

  if (doRunTest) {
    if (CALIBRATION_FACTOR == 0.0) {
      Serial.println("Set CALIBRATION_FACTOR first.");
      while (true) delay(1000);
    }
    scale.set_scale(CALIBRATION_FACTOR); 
    scale.tare();
    Serial.println("time_ms,tension");  // CSV header
  }
}

bool calibrateOnce() {
  if (!scale.wait_ready_timeout(2000)) {
    Serial.println("HX711 not found.");
    delay(1000);
    return false;                          // try again
  }
  scale.set_scale();                       
  Serial.println("Remove all weight...");
  delay(5000);
  scale.tare();
  Serial.println("Tare done. Hang the known weight now...");
  delay(8000);
  double reading = scale.get_units(10);
  Serial.print("Reading: ");
  Serial.println(reading);
  Serial.print("CALIBRATION_FACTOR = ");
  Serial.println(reading / KNOWN_WEIGHT, 4);
  return true;   
                            
}

void runTest() {

  if (scale.is_ready()) {
    Serial.print(millis());
    Serial.print(',');
    Serial.println(scale.get_units(1), 3);
  }

}

void loop() {

  static bool calibrated = false; 

  if (doCalibrate) {
    if (!calibrated) {
      calibrated = calibrateOnce();
    }  
  }

  else {
    runTest();
  }

}

