//CAMPO uno
#include <18F4550.h>
#device adc=10
#FUSES NOMCLR
#FUSES PUT
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES PLL1                     //No PLL PreScaler
#FUSES CPUDIV1                  //No System Clock Postscaler
#FUSES NOUSBDIV                 //USB clock source comes from primary oscillator
#FUSES HS                       //High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8, stop=2,stream=PORT1,enable=PIN_D3)//56800
#include <adc.c>
#use standard_io (a)
#use standard_io (b) 
#use standard_io (c)
#use standard_io (d) 
#use standard_io (e)
//#define LCD_ENABLE_PIN  PIN_D7
//#define LCD_RS_PIN      PIN_D6                                    
//#define LCD_RW_PIN      PIN_D5                                   
//#define LCD_DATA4       PIN_D4                                   
//#define LCD_DATA5       PIN_D2                                    
//#define LCD_DATA6       PIN_D1                                   
//#define LCD_DATA7       PIN_D0 
//#include <LCD_nuevo.C>
#define char_int(c) ((int)((c)-48)) 
#byte UCON=0xF6D
#bit USBEN=UCON.3 
#byte UCFG = 0xF6F 
#bit UTRDIS = UCFG.3 
#INCLUDE <string.h>
#INCLUDE <stdlib.h>

#use rtos(timer = 0, minor_cycle =5ms)
int16 valor_humedad[4];
int16 hum_i_1,hum_i_2,hum_i_3,hum_i_4;
int16 hum_f_1,hum_f_2,hum_f_3,hum_f_4;
int16 item;
int16 n_menus;
int16 i_rx=0;
int16 dato_completo=0;
int regando_1=0,regando_2=0,regando_3=0,regando_4=0,regando_t=0;
int sube_1=0,sube_2=0,sube_3=0,sube_4=0;
int baja_1=0,baja_2=0,baja_3=0,baja_4=0;
char  dato_tx[30];
char  dato_rx[30];
char  cadena[6];
char  dato_aux[5],dato_aux2[5];
int numero=0,fin_pal=0,dat_comp=0,j=0,s=0,m=0;
const int STATE_INI    = 1;
const int STATE_SEND   = 2;
const int STATE_ACTIVO = 3;
int estado_siguiente=1;
const int STATE_INICIAL           = 0;
const int STATE_ACT               = 1;
const int STATE_MANUAL            = 2;
const int STATE_TIEMPO            = 3;
const int STATE_HUMEDAD           = 4;
const int STATE_ACTIVAR_VALVULA   = 5;
const int STATE_DESACTIVAR_VALVULA = 6;
int estado_siguiente_riego=0;
int hora=0, minutos=0;
int minima=0, maxima=0, hora_inicio=0, minutos_inicio=0;
int hora_fin=0, minutos_fin=0;
int salida[4];
int manual=0;
int16 comando_rx=10;
int16 comando_tx=1;
int16 estado_valvula_1_placa_1=0;
int16 estado_valvula_2_placa_1=0;
int16 humedad_ciclo_uno=0;
int16 humedad_ciclo_dos=0;
int tipo_riego=0;
int tipo_riego_anterior =0;
int minima_guardada=0;
int maxima_guardada=0;
int entro_manual=0;
int regando_1_anterior=0;
int regando_2_anterior=0;
int regando_t_anterior=0;
#int_rda
STATE_STORE()
{
 dato_rx[i_rx]=getc();
 if (dato_rx[i_rx]=='&')
 {
  dato_completo=1;
  i_rx=0;
 }
 else
   {
    dato_completo=0;
    i_rx++;
   }
}

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
void encender_led(byte pin)
{
 output_high(pin);
}
void apagar_led(byte pin)
{
 output_low(pin);
}
void envio_dato()
{
 int i_tx,fin_dto=40;
 for(i_tx=0;i_tx<=fin_dto;i_tx++)
 {
  putc(dato_tx[i_tx]);
  if (dato_tx[i_tx]=='&') fin_dto=i_tx;
 }
}
void armar_trama()
{
 dato_tx[0]='0';
 strcpy(dato_tx,"00000001");
 sprintf(cadena,"%lu",comando_tx);
 strcat(dato_tx,cadena);
 sprintf(cadena,"+");
 strcat(dato_tx,cadena);
 if (comando_tx==1) 
 {
  sprintf(cadena,"%lu",valor_humedad[0]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",valor_humedad[1]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",valor_humedad[2]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",valor_humedad[3]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"&");
  strcat(dato_tx,cadena);
 }
 if (comando_tx==2) 
 {
  sprintf(cadena,"%lu",estado_valvula_1_placa_1);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",estado_valvula_2_placa_1);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",tipo_riego);
  strcat(dato_tx,cadena);
  sprintf(cadena,"&");
  strcat(dato_tx,cadena);
  tipo_riego_anterior=0;
 }
}
void deco_tramaRx()
{
  numero=0;
  fin_pal=65;
  dat_comp=0;
  j=0;
  for(m=8;m<=fin_pal;m++)
  {
   if((dato_rx[m]=='&')&&(dat_comp==1))
   {
    dat_comp=1;
    fin_pal=m;
   }
   if ((dato_rx[m]!='+')&& (dato_rx[m]!='&'))
   {
    dato_aux[j]=dato_rx[m];
    j=j+1;
    dat_comp=0;
   } else {
            for(s=0;s<=2;s++) dato_aux2[s]='0';
            for(s=0;s<j;s++)  dato_aux2[(4-j)+s]=dato_aux[s];
            j=0;
            if ((numero==0)&&(dat_comp!=1))
            {
              comando_rx=atoi32(dato_aux2);
              numero=1; dat_comp=1;
            }
            if ((numero==1)&&(dat_comp!=1))
            {
             if (comando_rx==1) hora=atoi32(dato_aux2);
             if (comando_rx==2) hora=atoi32(dato_aux2);
             if (comando_rx==3) hora=atoi32(dato_aux2);
             if (comando_rx==4) hora=atoi32(dato_aux2);
             if (comando_rx==8) hora=atoi32(dato_aux2);
             numero=2; 
             dat_comp=1;
            }
            if ((numero==2)&&(dat_comp!=1)) 
            { 
             if (comando_rx==1) minutos=atoi32(dato_aux2);
             if (comando_rx==2) minutos=atoi32(dato_aux2);
             if (comando_rx==3) minutos=atoi32(dato_aux2);
             if (comando_rx==4) minutos=atoi32(dato_aux2);
             if (comando_rx==8) minutos=atoi32(dato_aux2);
             numero=3;
             dat_comp=1;
            }
            if ((numero==3)&&(dat_comp!=1))
            { 
             if (comando_rx==1) minima=atoi32(dato_aux2);
             if (comando_rx==2)hora_inicio=atoi32(dato_aux2);
             if (comando_rx==3);
             if (comando_rx==4) manual=atoi32(dato_aux2);
             if(comando_rx==8);
             numero=4; 
             dat_comp=1;
            }
            if ((numero==4)&&(dat_comp!=1))
            {
             if (comando_rx==1)maxima=atoi32(dato_aux2);
             if (comando_rx==2)minutos_inicio=atoi32(dato_aux2);
             if (comando_rx==3);
             if (comando_rx==4);
             if (comando_rx==8);
             numero=5;
             dat_comp=1;
            }
            if ((numero==5)&&(dat_comp!=1))
            {
             if (comando_rx==1);
             if (comando_rx==2) hora_fin=atoi32(dato_aux2);
             if (comando_rx==3);
             if (comando_rx==4);
             if (comando_rx==8);
             numero=6;
             dat_comp=1;
            }
            if ((numero==6)&&(dat_comp!=1))
            {
             if (comando_rx==1);
             if (comando_rx==2)minutos_fin=atoi32(dato_aux2);
             if (comando_rx==3) ;
             if (comando_rx==4);
             if (comando_rx==8);
             numero=0;
             dat_comp=1;fin_pal=m;
            }
       } 
    }
}

void ciclo_histeresis_control_tiempo() 
{
 if ((valor_humedad[0]!=0)&&(valor_humedad[1]!=0))  humedad_ciclo_uno=((valor_humedad[0]+valor_humedad[1])/2);
    else if(valor_humedad[0]==0) humedad_ciclo_uno=valor_humedad[1];
                    else if(valor_humedad[1]==0) humedad_ciclo_uno=valor_humedad[0];

 if ((valor_humedad[2]!=0)&&(valor_humedad[3]!=0))  humedad_ciclo_dos=((valor_humedad[2]+valor_humedad[3])/2);
    else if(valor_humedad[2]==0) humedad_ciclo_dos=valor_humedad[3];
                    else if(valor_humedad[3]==0) humedad_ciclo_dos=valor_humedad[2];
 ///////////CICLO DE HISTERESIS SENSOR UNO//////////////////////////////////////////////////////////////////////////////
 
 if ( humedad_ciclo_uno<minima)
 {
  regando_1=1;
  sube_1=1;
 }
 if ( humedad_ciclo_uno>maxima)
 {
  regando_1=0;
  baja_1=1;
 }
 if (minima<= humedad_ciclo_uno<=maxima) 
 {
  if (sube_1==1){regando_1=1; sube_1=0;}
  if (baja_1==1){regando_1=0; ;baja_1=0; }
 }  
///////////CICLO DE HISTERESIS SENSOR DOS////////////////////////////////////////////////////////////////////////////////  
 if (humedad_ciclo_dos<minima)
 {
  regando_2=1;
  sube_2=1;
 }
 if ( humedad_ciclo_dos>maxima)
 {
  regando_2=0;
  baja_2=1;
 }
 if (minima<= humedad_ciclo_dos<=maxima) 
 {
  if (sube_2==1){regando_2=1; sube_2=0;}
  if (baja_2==1){regando_2=0; ;baja_2=0; }
 }  
 if (minima==maxima)
 {
  regando_1=0;
  regando_2=0;
 }
 if ((valor_humedad[0]==0)&&(valor_humedad[1]==0))regando_1=0;
 if ((valor_humedad[2]==0)&&(valor_humedad[3]==0))regando_2=0;
/////////////////////////////CONTRO POR TIEMPO//////////////////////////////////////////////////////////////////////////////////////////////////
 if (((hora>=0)&&(hora<=23)&&(minutos>=0)&&(minutos<=60)&&(hora_inicio>=0)&&(hora_inicio<=23)&&(minutos_inicio>=0)&&(minutos_inicio<=60)&&
                             (hora_fin>=0)&&(hora_fin<=23)&&(minutos_fin>=0)&&(minutos_fin<=60)))
 {
  if ((hora==hora_inicio)&&(minutos==minutos_inicio))  regando_t=1;
  if ((hora==hora_fin)&&(minutos==minutos_fin))        regando_t=0;
 } 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

//#task(rate = 250ms)
//void maquina_de_estado_display();
#task(rate = 5ms)
void maquina_de_estado_de_aplicacion();
#task(rate = 5ms)
void maquina_de_estado_de_comunicacion();
void main()
{
 enable_interrupts(INT_RDA);
 enable_interrupts(GLOBAL);
 desactivar_rele(PIN_C1);
 desactivar_rele(PIN_C2);
 setup_counters(RTCC_INTERNAL,RTCC_DIV_32);
 delay_us(10);
 rtos_run();
}
void maquina_de_estado_de_comunicacion()
{
 switch (estado_siguiente){
  case STATE_INI:
    delay_ms(150);
    UTRDIS = 1; 
    adc_init(); 
   // lcd_init();
    dato_rx[0]='1';
    dato_rx[1]='1';
    dato_rx[2]='1';
    dato_rx[3]='1';
    dato_rx[4]='1';
    dato_rx[5]='1';
    dato_rx[6]='1';
    dato_rx[7]='1';
    AN_0=10;AN_1=20;AN_2=30;AN_3=40;
    AN0_V=0;AN1_V=0;AN2_V=0;AN3_V=0;
    hum_i_1=0;hum_i_2=0;hum_i_3=0;hum_i_4=0;
    hum_f_1=0,hum_f_2=0,hum_f_3=0,hum_f_4=0;
    regando_1=0;regando_2=0,regando_3=0,regando_4=0,regando_t=0;
    sube_1=0;sube_2=0;sube_3=0;sube_4=0;
    baja_1=0;baja_2=0;baja_3=0;baja_4=0;
    item=0;
    n_menus =4;
    estado_siguiente=STATE_ACTIVO;
    salida[0]=0;salida[1]=0;salida[2]=0;salida[3]=0;
    minima=0;maxima=0;
    valor_humedad[0]=0;
    valor_humedad[1]=0;
    valor_humedad[2]=0;
    valor_humedad[3]=0;
  break;
  case STATE_SEND:
    dato_completo=0;
    envio_dato();
    while(dato_completo=!1)
    {}
    deco_tramaRx();
    if (comando_rx==3) comando_tx=1;
    if (comando_rx==1) comando_tx=1;
    if (comando_rx==8) comando_tx=2;
    dato_completo=0;
    estado_siguiente=STATE_ACTIVO;
  break;
  case STATE_ACTIVO:
    leer_ADC0_ADC1_ADC2_ADC3();
    convertir_ADC0_ADC1_ADC2_ADC3_a_tension();
    convertir_ADC0_ADC1_ADC2_ADC3_a_porcentaje();
    valor_humedad[0] = AN_0_porcentaje;
    valor_humedad[1]  = AN_1_porcentaje;
    valor_humedad[2] = AN_2_porcentaje;
    valor_humedad[3]= AN_3_porcentaje;
    if ((dato_rx[0]=='0') && (dato_rx[1]=='0')&& (dato_rx[2]=='0') && (dato_rx[3]=='0')&&
                     (dato_rx[4]=='0') && (dato_rx[5]=='0')&& (dato_rx[6]=='0') && (dato_rx[7]=='1')&&( dato_completo==1) )
    {
     armar_trama();
     estado_siguiente=STATE_SEND;
   //lcd_putc('\f');
   //if (dato_completo==1)
   //{
   //for(m=8;m<=fin;m++)
   //{
   //lcd_gotoxy(1,1);
   //if(dato_rx[m]=='&')m=fin;
   //lcd_putc(dato_rx[m]);
   //}
   //}
   //delay_ms(500);
   //}
  break;
  }
 }
}
//void maquina_de_estado_display()
//{
// if (lee_pulsador(PIN_C4)==1)
// {
//  item++;
//  delay_ms(300);
// }
// if (item > (n_menus-1)) item=0;
//        switch (item) {
//           case 0:printf(lcd_putc,"\fADC0 =%4u",tipo_riego); 
//                  printf(lcd_putc,"\nVoltaje = %01.2fv",AN0_v); 
//                  break;
//           case 1:printf(lcd_putc,"\fADC1 =%4u",maxima);
//                  printf(lcd_putc,"\nVoltaje = %01.2fv",AN1_v);
//                  break; 
//           case 2:printf(lcd_putc,"\fADC2 =%4ld",valor_humedad[2]);
//                  printf(lcd_putc,"\nVoltaje = %01.2fv",AN2_v); 
//                  break;
//           case 3:printf(lcd_putc,"\fADC3 =%4ld",valor_humedad[3]);
//                  printf(lcd_putc,"\nVoltaje = %01.2fv",AN3_v);
//                  break;
//                      }
//}

void maquina_de_estado_de_aplicacion()
{
 switch (estado_siguiente_riego){
  case STATE_INICIAL:
    estado_siguiente_riego=STATE_ACT;
  break;
  case STATE_ACT :
    ciclo_histeresis_control_tiempo();
    if ((regando_t!=regando_t_anterior)) estado_siguiente_riego=STATE_TIEMPO;
    if ((manual==11)||(manual==13)||(manual==12)||(manual==44)||((manual==45)&&(entro_manual==0))||((manual==46)&&(entro_manual==1))) estado_siguiente_riego=STATE_MANUAL;
    if((regando_1!=regando_1_anterior)||(regando_2!=regando_2_anterior))estado_siguiente_riego=STATE_HUMEDAD;
    regando_1_anterior=regando_1;
    regando_2_anterior=regando_2;
    regando_t_anterior=regando_t;
  break;
  case STATE_MANUAL: 
    if ((manual==11)||(manual==13)||(manual==12)) estado_siguiente_riego=STATE_ACTIVAR_VALVULA;
    if ((manual==44)) estado_siguiente_riego=STATE_DESACTIVAR_VALVULA;
    if ((manual==45)&&(entro_manual==0))
    {
     minima_guardada=minima;
     maxima_guardada=maxima;
     minima=0;
     maxima=0;
     entro_manual=1;
     estado_siguiente_riego=STATE_DESACTIVAR_VALVULA;
    }
    if ((manual==46))
    {
     minima=minima_guardada;
     maxima=maxima_guardada;
     estado_siguiente_riego=STATE_DESACTIVAR_VALVULA;
     entro_manual=0;
    }
    tipo_riego=2;
    tipo_riego_anterior=tipo_riego;
  break;
  case STATE_TIEMPO:
    if  (regando_t==1) estado_siguiente_riego=STATE_ACTIVAR_VALVULA;
    if  (regando_t==0) estado_siguiente_riego=STATE_DESACTIVAR_VALVULA;
    tipo_riego=3;
  break;
  case STATE_HUMEDAD:
    if((regando_1==1)||(regando_2==1)) estado_siguiente_riego=STATE_ACTIVAR_VALVULA;
    if((regando_1==0)&&(regando_2==0))estado_siguiente_riego=STATE_DESACTIVAR_VALVULA;//aca&&
    if ( tipo_riego_anterior==2) tipo_riego=2; else tipo_riego=1;
    tipo_riego_anterior=1;
 break;
  case STATE_ACTIVAR_VALVULA:
    if((regando_1==1)||(regando_t==1)||(manual==11)||(manual==13))
    {
     activar_rele(PIN_C1);
     estado_valvula_1_placa_1=1;
    }
    if ((regando_2==1)||(regando_t==1)||(manual==12)||(manual==13))
    {
     activar_rele(PIN_C2);
     estado_valvula_2_placa_1=1;
    }
    if (tipo_riego==1) estado_siguiente_riego=STATE_DESACTIVAR_VALVULA; else estado_siguiente_riego=STATE_ACT;
  break;
  case STATE_DESACTIVAR_VALVULA:
    if(((regando_1!=1)&&(regando_t!=1)&&(manual!=11)&&(manual!=13))||((manual==44)||(manual==45)))
    {
     desactivar_rele(PIN_C1);
     estado_valvula_1_placa_1=0;
    }
    if (((regando_2!=1)&&(regando_t!=1)&&(manual!=12)&&(manual!=13))||((manual==44)||(manual==45)))
    {
     desactivar_rele(PIN_C2);
     estado_valvula_2_placa_1=0;
    }
      estado_siguiente_riego=STATE_ACT;
    
    break;
    }
}

