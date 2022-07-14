#include <Arduino.h>
#include "esp32/ulp.h"
// include ulp header you will create
#include "ulp_main.h"
// must include ulptool helper functions also
#include "ulptool.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"


#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10       /* Time ESP32 will go to sleep (in seconds) */
#define GPIO_LED GPIO_NUM_4     /* led pin (blinks with low reads) */
#define GPIO_ADC ADC1_CHANNEL_6 /* battery metter adc */
#define PIN_ADC A6


// Unlike the esp-idf always use these binary blob names
extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

static void init_run_ulp(uint32_t usec);
void wakeup_reason();
void sleep_now();
void blink();


void setup() {
  Serial.begin(115200);
  // wakeup reason
  wakeup_reason();
  int adc2 = analogRead(PIN_ADC);
  Serial.printf("adc: %i (%i)\n", ulp_adcval & 0xFFFF, adc2);
  // sleep 
  sleep_now();
}

// the loop function runs over and over again forever
// this one is intentend to run never
void loop() {
  delay(100);
}

//configures ulp program an launches it
static void init_run_ulp(uint32_t usec) {
  //init adc for ulc (hard way) for battery level read
  adc1_config_channel_atten(GPIO_ADC, ADC_ATTEN_DB_0);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_ulp_enable();
  //init gpio for writing (hard way) for led
  rtc_gpio_init(GPIO_LED);
  rtc_gpio_set_direction(GPIO_LED, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_level(GPIO_LED, 0);

  ulp_set_wakeup_period(0, usec);
  esp_err_t err = ulptool_load_binary(0, ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
  err = ulp_run((&ulp_entry - RTC_SLOW_MEM) / sizeof(uint32_t));

  if (err) Serial.println("Error Starting ULP Coprocessor");
}

void sleep_now(){
  //timer
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //button
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
  // ulp 
  esp_sleep_enable_ulp_wakeup();
  init_run_ulp(500 * 1000); // 500 msec
  
  //go sleep
  esp_deep_sleep_start();
}

void wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


