int lee_pulsador(byte pin)
{
  if (input(pin)==1) return(1); else return (0);
}
void activar_rele(byte pin)
{
 output_high(pin);
 }
void desactivar_rele(byte pin)
{
 output_low(pin);

}