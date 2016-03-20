void encender_led(int led_numero)
{
 if (led_numero==0)
  {
   output_low(PIN_C0);
   output_low(PIN_C1);
   output_low(PIN_D3);
   output_low(PIN_C2);
  }
  if (led_numero==1)
  {
   output_low(PIN_C0);
   output_high(PIN_C1);
   output_float(PIN_D3);
   output_float(PIN_C2);
  }
  if (led_numero==2)
  {
   output_float(PIN_C0);
   output_low(PIN_C1);
   output_high(PIN_D3);
   output_float(PIN_C2);
  }
  if (led_numero==3)
  {
   output_float(PIN_C0);
   output_float(PIN_C1);
   output_low(PIN_D3);
   output_high(PIN_C2);
  }
 /* if (led_numero==4)
  {
   output_high(PIN_C0);
   output_low(PIN_C1);
   output_float(PIN_D3);
   output_float(PIN_C2);
  }
  if (led_numero==5)
  {
   output_float(PIN_C0);
   output_high(PIN_C1);
   output_low(PIN_D3);
   output_float(PIN_C2);
  }
  if (led_numero==6)
  {
   output_float(PIN_C0);
   output_float(PIN_C1);
   output_high(PIN_D3);
   output_low(PIN_C2);
  }
  if (led_numero==7)
  {
   output_high(PIN_C0);
   output_float(PIN_C1);
   output_low(PIN_D3);
   output_float(PIN_C2);
  }
  if (led_numero==8)
  {
   output_low(PIN_C0);
   output_float(PIN_C1);
   output_high(PIN_D3);
   output_float(PIN_C2);
  }
  if (led_numero==9)
  {
   output_float(PIN_C0);
   output_low(PIN_C1);
   output_float(PIN_D3);
   output_high(PIN_C2);
  }
  if (led_numero==10)
  {
   output_float(PIN_C0);
   output_high(PIN_C1);
   output_float(PIN_D3);
   output_low(PIN_C2);
  }
  if (led_numero==11)
  {
   output_low(PIN_C0);
   output_float(PIN_C1);
   output_float(PIN_D3);
   output_high(PIN_C2);
  }
  if (led_numero==12)
  {
   output_high(PIN_C0);
   output_float(PIN_C1);
   output_float(PIN_D3);
   output_low(PIN_C2);
  } */
}
