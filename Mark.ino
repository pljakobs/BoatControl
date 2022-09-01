TaskHandle_t Mark;
int t_start;

void setup_mark(int core, bool watchdog){
    xTaskCreatePinnedToCore(
    MarkFunc,
    "MarkFunc",
    10000,
    NULL,
    core,
    &Mark,
    0);
    t_start=millis();
    
  if(watchdog) esp_task_wdt_add(Mark);
}
void MarkFunc(void* param){
  while(true){
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.print((millis()-t_start)/1000);
    Serial.println(" --mark--");
  }
}
