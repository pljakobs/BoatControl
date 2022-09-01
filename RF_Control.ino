#include <RCSwitch.h>

#define ESP32_RCSWITCH_PIN GPIO_NUM_5  // Set RCSWITCH port to 5 (RXB6 receiver)

// key definitions
#define STBD10  5592332
#define PORT10 16736113
#define STBD1  16736120
#define PORT1  16736114

#define HEATER_OFF 3746563336
#define HEATER_ON  3746566576
#define HEATER_DN  3746562696
#define HEATER_UP  3746564336

#define DEBUG_STATE 1

#define K_STATE_IDLE 0
#define K_STATE_FRST 1
#define K_STATE_WAIT 2
#define K_STATE_REPT 3

// define key repeat timeouts
#define K_REPEAT_DELAY 1500
#define K_REPEAT_PERIOD 500

TaskHandle_t Receiver;
RCSwitch mySwitch = RCSwitch();

int now;
void handle_key(int key){
  switch(key){
    case STBD10:
      Serial.println("steuerbord 10");
      break;
    case PORT10:
      Serial.println("backbord 10");
      break;
    case STBD1:
      Serial.println("steuerbord 1");
      break;
    case PORT1:
      Serial.println("backbord 1");
      break;
    case HEATER_OFF:
      Serial.println("Heizung aus");
      break;
    case HEATER_ON:
      Serial.println("Heizung an");
      break;
    case HEATER_DN:
      Serial.println("Heizung -");
      break;
    case HEATER_UP:
      Serial.println("Heizung +");
      break;
    default: 
      Serial.println("unknown");
    }
}

void ReceiveFunc(void* param) {
  unsigned long key, last, t_press, t_repeat;
  int k_state,k_idle;
  
  while (true){
    while (!mySwitch.available()){
      vTaskDelay(50 / portTICK_PERIOD_MS);
      if(k_idle++==3 && k_state!=K_STATE_IDLE) {          // if there was no key data available for 200ms, we consider 
        k_state=K_STATE_IDLE;                             // all keys released and reset the state machine to K_STATE_IDLE
        last=0;                                           // the last key is none
        #ifdef DEBUG_STATE
          Serial.println("K_STATE_IDLE");
        #endif
      }
      if(millis()-now>4500) {
        esp_task_wdt_reset();                             //reset watchdog
        now=millis();
      }
    }
    k_idle=0;                                             // if a key has been pressed, we'll reset the idle counter
    key = mySwitch.getReceivedValue();                    // get the numerical key value
    if(key==last){                                        // same key as last time ?
      switch(k_state){          
        case K_STATE_FRST:                                // first time we see the same key
          k_state=K_STATE_WAIT;                           // next state is to wait K_REPEAT_DELAY ms
          #ifdef DEBUG_STATE
            Serial.println("K_STATE_WAIT");
          #endif
          t_press=millis(); //start timer                 // remember the time when we first saw the key for the 2nd time
          break;
        case K_STATE_WAIT:                                // waiting state
          if (millis()-t_press>=K_REPEAT_DELAY) {         // have we been in this state for K_REPEAT_DELAY ms? 
            k_state=K_STATE_REPT;                         // if so, we'll progress to the repeating state
            #ifdef DEBUG_STATE
              Serial.println("K_STATE_REPT");
            #endif
            handle_key(key);                              // but we need to handle the current key first
            t_repeat=millis();                            // and start the repeat timer
          }
          break;
        case K_STATE_REPT:                                // we're in repeat state
          if(millis()-t_repeat>=K_REPEAT_PERIOD){         // if the repeat timer has run for K_REPEAT_PERIOD ms 
            handle_key(key);                              // we'll take this as a key re-press
            t_repeat=millis();                            // and restart the timer
          }
          break;
        default:
          k_state=K_STATE_IDLE;                           // if, for whatever reason, an invalid state exists, reset to idle
        }
      }else{
        k_state=K_STATE_FRST;                             // if another key has been pressed, we'll jump back to K_STATE_FRST
        #ifdef DEBUG_STATE
          Serial.println("K_STATE_FRST");
        #endif
        handle_key(key);                                  // handle that key
        last=key;                                         // and then store it as our current last key
      }
      mySwitch.resetAvailable();
    
    esp_task_wdt_reset();
  }
}

void setup_RF_control(int core, bool watchdog){
  pinMode(ESP32_RCSWITCH_PIN,INPUT);
  digitalWrite(ESP32_RCSWITCH_PIN, HIGH);
  mySwitch.enableReceive(digitalPinToInterrupt(ESP32_RCSWITCH_PIN));  // Receiver on GPIO15 on ESP32
  xTaskCreatePinnedToCore(
    ReceiveFunc,
    "receive",
    10000,
    NULL,
    core,
    &Receiver,
    0);
  now=millis();
  if(watchdog) esp_task_wdt_add(Receiver);
}
