
  void portReset() { 
   output_float(PIN_C0); 
   output_float(PIN_C1); 
   output_float(PIN_D3); 
   output_float(PIN_C2); 
    
} 

void led1() { 
   output_low(PIN_C0); 
   output_high(PIN_C1); 
} 

void led2() { 
   output_low(PIN_C1); 
   output_high(PIN_D3); 
} 

void led3() { 
   output_low(PIN_D3); 
   output_high(PIN_C2); 
} 

void led4() { 
   output_low(PIN_A5); 
   output_high(PIN_A1); 
} 

//Next matrix line 

void led5() { 
   output_low(PIN_A0); 
   output_high(PIN_A4); 
} 

void led6() { 
   output_low(PIN_A4); 
   output_high(PIN_A0); 
} 

void led7() { 
   output_low(PIN_A1); 
   output_high(PIN_A4); 
} 

void led8() { 
   output_low(PIN_A4); 
   output_high(PIN_A1); 
} 

//Next matrix line 

void led9() { 
   output_low(PIN_A0); 
   output_high(PIN_A2); 
} 

void led10() { 
   output_low(PIN_A2); 
   output_high(PIN_A0); 
} 

void led11() { 
   output_low(PIN_A1); 
   output_high(PIN_A2); 
} 

void led12() { 
   output_low(PIN_A2); 
   output_high(PIN_A1); 
} 
