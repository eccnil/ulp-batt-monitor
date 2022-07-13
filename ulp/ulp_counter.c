#ifdef _ULPCC_ // Do not add anything above this def
// must include ulpcc helper functions
#include <ulp_c.h>

// global variable that the main processor can see
//unsigned counter = 0;
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
  if (adcval > 2500){
    wake();
  }
}
#endif // do not add anything after here
