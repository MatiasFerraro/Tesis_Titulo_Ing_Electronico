/////////////TECLADO/////////////
int16 valor_adc_leido;
int16 valor_adc_leido_anterior;
char tecla;
int bandera_presionada;
int8 r,Indice;
//const int16 valores_adc_teclas[17]={0,310,318,329,339,445,460,486,508,551,574,614,650,698,736,803,865};//valores teoricos
const int16 valores_adc_teclas[17]={0,310,316,327,336,438,453,477,497,542,563,601,633,683,718,780,834};//valores practicos
const char simbolo_tecla[17]={'Z','C','D','E','F','B','3','6','9','A','2','5','8','0','1','4','7'};
const int16 tolerancia=5;
int8 max_tecla=16;
const int INIT      = 0;
const int SUELTA    = 1;
const int VALIDA    = 2;
const int PRESIONA  = 3;
int estado_siguiente_maquina_teclado=0;
////////////TECLADO//////////////
void tarea_teclado_analogico(void){
int max;
switch (estado_siguiente_maquina_teclado){
 case INIT: 
 
 
  valor_adc_leido=0;
  valor_adc_leido_anterior=0;
  tecla ='1';
  bandera_presionada=0;
 
  valor_adc_leido_anterior=0;
  Indice=0;
  max_tecla=16;
  estado_siguiente_maquina_teclado=SUELTA;
 break;
 case SUELTA:
 /* for(r=0;r<=19;r++)
  {
  
  valor_adc_leido =(int16) (read_adc()+valor_adc_leido);
  delay_us(100);
  }
  */
  //valor_adc_leido= (int16) (valor_adc_leido/19);
  setup_adc_ports( AN0|VSS_VDD);delay_us(50);
  setup_adc(ADC_CLOCK_INTERNAL); //fuente de reloj rc
  enable_interrupts(global);
  set_adc_channel(0);
  delay_us(20);
  valor_adc_leido =(int16)read_adc();
  delay_us(50);
  max=max_tecla;
  estado_siguiente_maquina_teclado=PRESIONA;
  
  //lcd_gotoxy(1,1);
  //printf(lcd_putc,"%lu",valor_adc_leido); 
  //if((valor_adc_leido!=0))
  //{//&&( valor_adc_leido!= valor_adc_leido_anterior)&&(bandera_presionada==0)
  //  bandera_presionada=0;
  //  Indice=0;
  //  estado_siguiente_maquina_teclado=PRESIONA;
 // }
 break;
 case PRESIONA:
  if (bandera_presionada==0)
 {
  estado_siguiente_maquina_teclado=VALIDA;
 }
 break;
 case VALIDA:
 for(Indice=1;Indice<max;Indice++) 
 {//lcd_gotoxy(1+Indice,3);
  //printf(lcd_putc,"%u",Indice); 
  if((valor_adc_leido>(valores_adc_teclas[Indice]-tolerancia)) && (valor_adc_leido<(valores_adc_teclas[Indice]+tolerancia)))
  {//&&(valor_adc_leido!=valor_adc_leido_anterior)
   tecla=simbolo_tecla[Indice];   
   max=indice;
   estado_siguiente_maquina_teclado=SUELTA;
   bandera_presionada=0;
   lcd_gotoxy(1,4); lcd_putc(tecla);
  }
 }
 //if (Indice>max_tecla)
 // {
 //  tecla='Z'; 
 //  estado_siguiente_maquina_teclado=SUELTA;
  
 // }
 //valor_adc_leido=read_adc();
 //valor_adc_leido_anterior=valor_adc_leido;
break;
}}
