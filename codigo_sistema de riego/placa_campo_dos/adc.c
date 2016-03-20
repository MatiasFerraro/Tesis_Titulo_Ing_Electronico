//agregar en codigo principal ---->#device adc=10
int16 AN_0,AN_1,AN_2,AN_3;
float AN0_V,AN1_V,AN2_V,AN3_V;
int AN_0_porcentaje,AN_1_porcentaje,AN_2_porcentaje,AN_3_porcentaje;
adc_init()
{
  /*enable_interrupts(global);
  enable_interrupts(int_rda);
  setup_adc_ports(ALL_ANALOG);
  setup_adc(ADC_CLOCK_INTERNAL); 
  */
  setup_adc_ports(AN0_TO_AN4|VSS_VREF); 
  setup_adc(ADC_CLOCK_INTERNAL); 
  enable_interrupts(global);
  /* setup_adc(ADC_CLOCK_Div_8);
  setup_psp(PSP_DISABLED); 
  setup_spi(SPI_SS_DISABLED); 
  setup_wdt(WDT_OFF); 
  setup_timer_0(RTCC_INTERNAL); 
  setup_timer_1(T1_DISABLED); 
  setup_timer_2(T2_DIV_BY_1,60,5); 
  setup_timer_3(T3_DISABLED|T3_DIV_BY_1); 
  setup_comparator(NC_NC_NC_NC); 
 */

}
void leer_ADC0_ADC1_ADC2_ADC3()
{
   
  
  set_adc_channel(0); //habilitacion canal 0
  delay_ms(2);
  AN_0=read_adc();   //lectura canal 0
 
  set_adc_channel(1); //habilitacion canal 1
  delay_ms(2);
  AN_1=read_adc();   //lectura canal 0
  ;
  set_adc_channel(2); //habilitacion canal 1
  delay_ms(2);
  AN_2=read_adc();   //lectura canal 0
   
  set_adc_channel(4); //habilitacion canal 1
  delay_ms(2);
  AN_3=read_adc();   //lectura canal 0
 
  //delay_ms(100);
}
void convertir_ADC0_ADC1_ADC2_ADC3_a_tension()
{      
  AN0_V=2.5*AN_0 /1023.0;    //conversion a tension
  AN1_V=2.5*AN_1 /1023.0;    //conversion a tension
  AN2_V=2.5*AN_2 /1023.0;    //conversion a tension
  AN3_V=2.5*AN_3 /1023.0;    //conversion a tension
}        
void convertir_ADC0_ADC1_ADC2_ADC3_a_porcentaje()        
{
  AN_0_porcentaje=(int)((AN0_V*100)/0.58);
  AN_1_porcentaje=(int)((AN1_V*100)/0.58);
  AN_2_porcentaje=(int)((AN2_V*100)/0.58);
  AN_3_porcentaje=(int)((AN3_V*100)/0.58);
}   //512     
