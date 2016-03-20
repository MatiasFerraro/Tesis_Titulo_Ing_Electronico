//////////////////////////////////////////////////////////////TECLADO ANALOGICO//////////////////////////////////////////////////////
int16 valor_adc_leido=0;
int16 valor_adc_tecla_presionada;
char tecla;
int bandera_presionada,i;
//const int16 valores_adc_teclas[17]={0,308,317,327,337,443,458,484,505,550,572,613,642,695,735,802,863};//valores teoricos
const int16 valores_adc_teclas[17]={0,308,315,326,335,540,563,601,633,438,454,477,497,27,63,126,176};//valores practicos181
const char simbolo_tecla[17]={'Z','C','D','E','F','B','3','6','9','A','2','5','8','0','1','4','7'};
const int16 tolerancia=5;
int max_tecla=18;
const int INIT      = 0;
const int SUELTA    = 1;
const int VALIDA    = 2;
const int PRESIONA  = 3;
int estado_siguiente_maquina_teclado=0;

void tarea_teclado_analogico(void){
switch(estado_siguiente_maquina_teclado)
{
 case INIT:
   int i,max=16;
   setup_adc_ports(AN0|VSS_VDD); 
   setup_adc(ADC_CLOCK_DIV_8);
   enable_interrupts(global);
   bandera_presionada=0;
   estado_siguiente_maquina_teclado=SUELTA;
 break;
 case SUELTA:
   valor_adc_leido=0; 
   set_adc_channel(0); //habilitacion canal 0
   delay_ms(2);
   valor_adc_leido =0;
   for(i=1;i<=100;i++)
   {
    valor_adc_leido =read_adc()+valor_adc_leido;
    if (i==100)valor_adc_leido =(valor_adc_leido/i);
   }
   max=16;
   if((valor_adc_leido>10)&&(bandera_presionada==0))
   {
    bandera_presionada=1;
    estado_siguiente_maquina_teclado=PRESIONA;
    valor_adc_tecla_presionada=valor_adc_leido;
   }
 break;
 case PRESIONA:
   set_adc_channel(0); //habilitacion canal 0
   delay_ms(2);
   valor_adc_leido =0;
   for(i=1;i<=100;i++)
   {
    valor_adc_leido =read_adc()+valor_adc_leido;
    if (i==100)valor_adc_leido =(valor_adc_leido/i);
   }
   if((bandera_presionada==1)&&(valor_adc_leido<10))
   estado_siguiente_maquina_teclado=VALIDA;
 break;
 case VALIDA:
   max=max_tecla;
   for(i=0;i<=max;i++)
   {
    if((valor_adc_tecla_presionada>(valores_adc_teclas[i]-tolerancia)) && (valor_adc_tecla_presionada<(valores_adc_teclas[i]+tolerancia)))
    { 
     max=i;
     tecla=simbolo_tecla[i]; 
    }  
   }
   if (i>=max_tecla) tecla='Z';   
   bandera_presionada=0;
   estado_siguiente_maquina_teclado=SUELTA;
 break;
}
}
