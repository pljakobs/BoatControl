#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_task_wdt.h"

#define CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER 
#define TWDT_TIMEOUT_MS         3000
#define TASK_RESET_PERIOD_MS    2000
#define MAIN_DELAY_MS           10000

#define WDT_TIMEOUT             5 //seconds

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  Serial.println("starting watchdog");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  Serial.println("starting tasks");
  Serial.println("...mark");
  setup_mark(0,false);
  Serial.println("...RC_Control");
  setup_RF_control(1,true);
  Serial.println("...BatteryMonitor");
  BatteryMonitor_setup(0,true);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000000);
 
}
