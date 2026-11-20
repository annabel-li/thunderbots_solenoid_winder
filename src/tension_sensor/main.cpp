// Calibrating the load cell
#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 25; //data pin
const int LOADCELL_CLK_PIN = 26; //clk pin 
const double CALIBRATION_FACTOR = 0.0; 

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
    scale.set_scale(CALIBRATION_FACTOR); 
    scale.tare();
    Serial.println("time_ms,tension");  // CSV header
  }
}

void calibrate() {

  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);

}
//next step: calibration factor = (reading)/(known weight)

void runTest() {

  if (scale.is_ready()) {
    Serial.print(millis());
    Serial.print(',');
    Serial.println(scale.get_units(1), 3);
  }

}

void loop() {

  if (doCalibrate) {
    calibrate(); 
  }
  else if (doRunTest) {
    runTest(); 
  }

}
