#ifdef _ULPCC_ // Do not add anything above this def
// must include ulpcc helper functions
#include <ulp_c.h>

// global variable that the main processor can see
unsigned counter = 0;
unsigned adcval = 0;
// all ulpcc programs have have this function
void entry() {
  unsigned acc = 0;
  //counter ++ ; 
  acc += adc (0,7); 
  acc += adc (0,7); 
  acc += adc (0,7); 
  acc += adc (0,7); 
  adcval = acc >> 2;

  //escribir high en rtcgpio10 (gpio 26 / touch0)  
  /* Disable hold of RTC_GPIO10 output */
  WRITE_RTC_REG(RTC_IO_TOUCH_PAD0_REG,RTC_IO_TOUCH_PAD0_HOLD_S,1,0);
  if (adcval < 2500 && counter == 0){
    /* Set the RTC_GPIO10 output HIGH to signal that battery is low */
	  WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG,RTC_GPIO_OUT_DATA_W1TS_S+10,1,1);  
    //sleep(1000);
  }  else {
    /* Set the RTC_GPIO10 output LOW (clear output)
      to signal that ULP is now going down */
    WRITE_RTC_REG(RTC_GPIO_OUT_W1TC_REG,RTC_GPIO_OUT_DATA_W1TC_S+10,1,1);
  }
  /* Enable hold on RTC_GPIO10 output */
  WRITE_RTC_REG(RTC_IO_TOUCH_PAD0_REG,RTC_IO_TOUCH_PAD0_HOLD_S,1,1);

  counter ++;
  if (counter >=3 ) counter = 0;
}
#endif // do not add anything after here
