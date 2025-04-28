/*
   MIT License

  Copyright (c) 2021 Felix Biego

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#include "esp_timer.h"
#include <ESP32Time.h>
int SECONDS_TO_SLEEP = 60;
RTC_DATA_ATTR unsigned long epochSeconds = 0;
RTC_DATA_ATTR int bootNum = 0;

//ESP32Time rtc;
RTC_DATA_ATTR ESP32Time rtc(-21600);  // offset in seconds GMT+1

void setup() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();
  Serial.begin(115200);
  while(!Serial);
  if(bootNum == 0){
    Serial.println("Please input current time in epoch seconds");
    while(!Serial.available());
    epochSeconds = Serial.parseInt();
    Serial.println(epochSeconds);
    rtc.setTime(epochSeconds);
  }
  if( wakeup_reason == ESP_SLEEP_WAKEUP_TIMER){
    Serial.println("Timer Wakeup");
    epochSeconds += SECONDS_TO_SLEEP;
    rtc.setTime(epochSeconds);
    Serial.println(epochSeconds);
  }
  
  esp_sleep_enable_timer_wakeup(SECONDS_TO_SLEEP * 1000000);
  bootNum++;
//  epochSeconds = rtc.getEpoch();
  esp_deep_sleep_start();

}

void loop() {
  Serial.println(rtc.getEpoch());   // (String) returns time with specified format 


  struct tm timeinfo = rtc.getTimeStruct();
  
  delay(1000);
}
