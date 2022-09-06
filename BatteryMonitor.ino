#include <HardwareSerial.h>
#include <string.h>

#define MAXDEVS 4

typedef enum {
        reading,
        cr,
        crlf
}msgState_t;

typedef  struct{
    int 
        deviceAddress,
        checksum,
        maxVoltage,
        maxCurrent;

    enum sensorType{
        HALLSENSOR = 1,
        SAMPLER=2
    };
    
    int deviceVersion;
    int deviceSerialNumber;
} basicInfo_t;


typedef struct {
    int 
        deviceAddress,
        checksum,    
        uptime,
        batteryLifeLeft,
        temperature;

    float
        voltage,
        current,
        internalResistance,
        remainingCapacity,
        cumulativeCapacity;

    enum outputState{
        ON = 0,
        OVP = 1,
        OCP = 2,
        LVP = 3,
        NCP = 4,
        OPP = 5,
        OTP = 6,
        OFF =255
    };

    enum currentDir{
        discharging = 0,
        charging =1
    };
}measuredValues_t;

typedef struct{
    int 
        deviceAddress,
        checksum,
        protectionTemperature,
        protectionRecoveryTime,
        protectionDelayTime,
        presetCapacity,
        voltageCalibration,
        currentCalibration,
        temperatureCalibration,
        voltageScale,
        currentScale;

    float        
        OVPVoltage,
        UVPVoltage,
        OCPForwardCurrent,
        OCPReverseCurrent,
        OPPPower;

    enum relayType{
        normallyOpen=0,
        normallyClosed=1
    };
}setValues_t;

HardwareSerial battMon(1);
basicInfo_t basicInfo[MAXDEVS];
setValues_t setValues[MAXDEVS];
measuredValues_t measuredValues[MAXDEVS];
bool _watchdog;

TaskHandle_t BatteryMonitor;

void BatteryMonitor_setup(int core, bool watchdog) {
  _watchdog=watchdog;
  
  battMon.begin(115200, SERIAL_8N1, 17, 16);
  xTaskCreatePinnedToCore(
    BatteryMonitor_loop,
    "BatteryMonitor",
    10000,
    NULL,
    core,
    &BatteryMonitor,
    0
  );
  Serial.println("....started BatteryMonitor Task");
  if(_watchdog) {
    esp_task_wdt_add(BatteryMonitor);
    //Serial.println("....enabled Watchdog");
  }
}

void BatteryMonitor_loop(void *param) {
  while(true){
    //char c;
    //char message[255];
    String message;
    //Serial.println("Battery Monitor Entry");
    
    battMon.printf("-->:R50=1,2,1,\r\n");
    Serial.printf(":R50=1,2,1,\r\n");
  
    while (!battMon.available()) vTaskDelay(100/portTICK_PERIOD_MS);
  
    //Serial.println("receiving msg");
    if (battMon.available()) {
      message=readMessage();
      Serial.printf("<--%s\n",message.c_str());
    }
  
    vTaskDelay(4500 / portTICK_PERIOD_MS);
    if(_watchdog) esp_task_wdt_reset();
  }
}

void sendMessage(String message){
  String _message=message;
  
}
void getBasicInfo(){
  
}
void getSetValues(){
  
}

void getMeasuredValues(){
  
}
String readMessage(){   
    String _message;
    msgState_t _msgState;
    char c;
    //Serial.println(".....start read loop");
    if (_msgState==crlf){
      //Serial.println(".....new message");
      _message="";
      _msgState=reading;
    }
    
    if (sizeof(_message)==0) Serial.print("[start]");
    while(battMon.available() && _msgState != crlf){
        c=battMon.read();
        //Serial.print(c);
        _message=_message+String(c);
        //Serial.print(c);
        if (c=='\r') {
          _msgState=cr;
          //Serial.print("{cr}");
        }
        if (_msgState==cr && c=='\n') _msgState=crlf;
        
        //if (_msgState==reading) Serial.print("[r]");
        //if (_msgState==cr) Serial.print("[CR]");
        //if (_msgState==crlf) Serial.println("[CRLF]");
    }
    //Serial.println(".....finished read loop");
    //Serial.println(_message);
    return _message;
}
