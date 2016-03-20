#include <18F4550.h>
#device adc=10
#FUSES NODEBUG
#FUSES VREGEN 
#FUSES NOPBADEN
#FUSES NOMCLR
#FUSES PUT
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES WDT128                   //Watch Dog Timer uses 1:128 Postscale
#FUSES PLL1                     //No PLL PreScaler
#FUSES CPUDIV1                  //No System Clock Postscaler
#FUSES NOUSBDIV                 //USB clock source comes from primary oscillator
#FUSES HS                       //High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES BROWNOUT                 //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8 ,stop=2,stream=PORT1,enable=PIN_E2)//,
#use rs232(baud=9600,parity=N,xmit=PIN_E0,rcv=PIN_B2,bits=8 ,stop=1, STREAM=PORT2,FORCE_SW)//
#use rtos(timer = 0, minor_cycle =5ms)
#use standard_io (a)
#use standard_io (b) 
#use standard_io (c) 
#use standard_io (d) 
#use standard_io (e)
#define RTC_SDA  PIN_B0
#define RTC_SCL  PIN_B1
#include <lcd_16x4.c>
#include <teclado_adc.c>
#include <_ds1307.c>
#include <string.h>
#include <stdlib.h>
#byte UCON=0xF6D
#bit USBEN=UCON.3 
#byte UCFG = 0xF6F 
#bit UTRDIS = UCFG.3
#include <manejo_leds.c>
#include <manejo_pulsadores_reles.c>
/////////////////////////////////////////////////////////
#define minima_placa_1_e  0  // Location in EEPROM
#define maxima_placa_1_e  1  // Location in EEPROM
#define minima_placa_2_e  2  // Location in EEPROM
#define maxima_placa_2_e  3  // Location in EEPROM
/////////////////////////////////////////////////////////
byte segundos;
byte minutos;
byte hora;
byte dia;
byte mes;
byte anio;
byte dow1;

int16 valor_humedad_uno[3],valor_humedad_dos[3],valor_humedad_tres[3],
                                                       valor_humedad_cuatro[3];
int16  minima[3], maxima[3];
int16  minima_anterior[3], maxima_anterior[3];
int16 manual[3];
int16 manual_anterior[3];
int16 hora_inicio[3],hora_fin[3],minutos_inicio[3],minutos_fin[3];
int16 valor_uno, valor_dos;
int   j=0,numero=0,s;
int   i_rx=0;
int   dato_completo=0,dat_comp,fin_pal;
int   tx_placa=1,tx_placa_anterior=1;

char dato_tx[30];
char dato_rx[40];
char dato_aux[5],dato_aux2[5];
char cadena[6];

const int STATE_INI    = 0;
const int STATE_SEND   = 1;
const int STATE_ACTIVO = 2;

const int          STATE_00      =0;
const int          STATE_01      =1;
const int          STATE_02      =2;
const int          STATE_03      =3;
const int          STATE_04      =4;
const int          STATE_01_A    =5;
const int          STATE_02_BCD_0=6;
const int          STATE_02_BCD_1=7;
const int          STATE_02_B1   =8;
const int          STATE_02_B2   =9;
const int          STATE_02_C0   =10;
const int          STATE_03_G    =11;
const int          STATE_03_G0   =12;
const int          STATE_03_H    =13;
const int          STATE_03_H0   =14;
const int          STATE_03_H1   =15;
const int          STATE_04_A    =16;
const int          STATE_04_B    =17;
const int          STATE_04_C    =18;
const int          STATE_04_D    =19;

char dato,dato1[3],dato2[3],dato3[3],dato4[3],dato5[3];
byte hora_riego_inicio=0,minutos_riego_inicio=0;
byte hora_riego_fin=0,minutos_riego_fin=0;
int16 comando_tx=1, comando_tx_anterior=0;

int bandera_modifica=0;
int estado_siguiente=0;
int estado_siguiente_maquina_lcd=0;
int resultado=0;
int num_veces=0, max_nodo=2;
int ind1=0,ind2=0,ind3=0,ind4=0,ind5=0;
int sem1;
int max_valvula = 2;
int resultado1=0, resultado2=0;
int placa=1;
int salida_1[3],salida_2[3],salida_3[3],salida_4[3];
int sube_1[3],sube_2[3],sube_3[3],sube_4[3];
int baja_1[3],baja_2[3],baja_3[3],baja_4[3];
int x;
int comando_rx=1;
int estado_valvula_1_placa_1=0;
int estado_valvula_2_placa_1=0;
int estado_valvula_1_placa_2=0;
int estado_valvula_2_placa_2=0;
int estado_valvula_1_placa_1_anterior=0;
int estado_valvula_2_placa_1_anterior=0;
int estado_valvula_1_placa_2_anterior=0;
int estado_valvula_2_placa_2_anterior=0;
int tipo_riego_placa_1=0;
int tipo_riego_anterior_placa_1=0;
int tipo_riego_placa_2=0;
int tipo_riego_anterior_placa_2=0;

char string[5];
int estado_siguiente_maquina_reportes=0;
const int ST_INI                                       =0;
const int ST_ACTIVO                                    =1; 
const int ST_APERTURA_VALVULA_MANUAL                   =2;
const int ST_APERTURA_VALVULA_HUMEDAD                  =3;
const int ST_REGISTRO_VALORES_HUMEDAD         =4;
const int ST_LIMITES_HUMEDAD                           =5;
const int ST_APERTURA_VALVULA_TIEMPO                   =6;

int veces_registro=0;
int humedad_valvula_uno_placa_uno=0;
int humedad_valvula_dos_placa_uno=0;
int humedad_valvula_uno_placa_dos=0;
int humedad_valvula_dos_placa_dos=0;
int estado_siguiente_maquina_blue    =  0;
const int ST_INI_BLUE                  =0;
const int ST_BLUE_ACTIVO               =1;
const int ST_BLUE_TX                   =2;
const int ST_BLUE_RX                   =3;
char caracter_rx_blue;
char caracter_anterior_rx_blue;
int tx_blue,rx_blue;
char c;
int leo_dato=0;
int veces_ver_humedad=0;
int espera_placa_uno=0;
int espera_placa_dos=0;
int max_espera=200;
int  inicializacion_1=0;
int  inicializacion_2=0;

void guardar_MEM( byte dato, long int direccion)
{
 short int estado;
 i2c_start();                  //inicializa la transmicion
 i2c_write(0xA0);              //escribe la palabra de control(direccion
                               //0h+0 para escritura
 i2c_write(direccion>>8);      //parte alta de la direccion a escribir en la
                               //EEPROM
 i2c_write(direccion);         //parte baja de la direccion a escribir en la
                               //EEPROM
 i2c_write(dato);               //dato a escribir
 i2c_stop();                   //finalizacion de la transmision
 i2c_start();                  //reinicio
 estado=i2c_write(0xa0);       //lectura del bit ACK, para evitar escrituras
                               //incorrectas
 while (estado==1)             //si es 1 espera a que responda el esclavo
 {
   i2c_start();
   estado=i2c_write(0xa0);
 }
}
BYTE leer_MEM(long int direccion)
{
 byte dato;    
 i2c_start();                  //inicializa la transmision
 i2c_write(0xA0);              //escribe la palabra de control (direccion 0h
                               //+ 0 para escritura
 i2c_write(direccion>>8);      //parte alta de la direccion a escribir en la
                               //EEPROM
 i2c_write(direccion);         //parte baja de la direccion a escribir en la
                               //EEPROM
 i2c_start();                  //reinicio
 i2c_write(0xa1);              //escribe la palabra de control (direccion 0h
                               //+1 para la lectura)
 dato=i2c_read(0);             //lectura del dato
 i2c_stop();                   //finalizacion de la transmision
 return(dato);
}

#int_ext2
void int_RB2() 
{    
  if(kbhit(PORT2))
  { 
   c=toupper(getc(PORT2));
   if(c=='L')
   {
    tx_blue=1;
    caracter_rx_blue=c;
   }
   if((c=='E')||(c=='S')||(c=='N'))
   {
    rx_blue=1;
    caracter_rx_blue=c;
   }
  }
}        

void interruptrb2_init()
{
 enable_interrupts(int_ext2);
 ext_int_edge(2, H_TO_L );
}
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
void envio_dato()
{
 int i_tx,fin_dto=60;
 for(i_tx=0;i_tx<=fin_dto;i_tx++)
  {
   putc(dato_tx[i_tx]);
   if (dato_tx[i_tx]=='&') fin_dto=i_tx;
  }
}

void armar_trama()
{
 for(i=0;i<=59;i++)
 dato_tx[0]='0';
 if (tx_placa==1) strcpy(dato_tx,"00000001");
 if (tx_placa==2) strcpy(dato_tx,"00000010");
 if ((comando_tx==3)||(comando_tx==8))
 {
  sprintf(cadena,"%lu",comando_tx);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",hora);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",minutos);
  strcat(dato_tx,cadena);
  sprintf(cadena,"&");
  strcat(dato_tx,cadena);
 }
 if (comando_tx==2)
 {
  sprintf(cadena,"%lu",comando_tx);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",hora);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",minutos);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",hora_inicio[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",minutos_inicio[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",hora_fin[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",minutos_fin[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"&");
  strcat(dato_tx,cadena);
 }
 if (comando_tx==4)
 {
  sprintf(cadena,"%lu",comando_tx);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",hora);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",minutos);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",manual[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"&");
  strcat(dato_tx,cadena);
 }
 if (comando_tx==1)
 {
  sprintf(cadena,"%lu",comando_tx);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",hora);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%u",minutos);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",minima[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"+");
  strcat(dato_tx,cadena);
  sprintf(cadena,"%lu",maxima[tx_placa]);
  strcat(dato_tx,cadena);
  sprintf(cadena,"&");
  strcat(dato_tx,cadena);
 }
}

void deco_tramaRx()
{   
     numero=0;
     fin_pal=50;dat_comp=0;
     for(i=8;i<=fin_pal;i++)
     {
      if((dato_rx[i]=='&')&&(dat_comp==1))
      {
       dat_comp=1;
       fin_pal=i;
      }
      if ((dato_rx[i]!='+')&& (dato_rx[i]!='&'))
      {
       dato_aux[j]=dato_rx[i];
       j=j+1;dat_comp=0;
      }else {
               for(s=0;s<=3;s++) dato_aux2[s]='0';
               for(s=0;s<j;s++)  dato_aux2[(4-j)+s]=dato_aux[s];
               j=0;
               if ((numero==0)&&(dat_comp!=1))
               {
                comando_rx=(atoi32(dato_aux2));
                numero=1; dat_comp=1;
               }
               if ((numero==1)&&(dat_comp!=1))
               {
                if (comando_rx==1) valor_humedad_uno[tx_placa]=(atoi32(dato_aux2));
                if ((comando_rx==2)&&(tx_placa==1)) 
                { 
                 estado_valvula_1_placa_1_anterior=estado_valvula_1_placa_1;
                 estado_valvula_1_placa_1=(atoi32(dato_aux2));
                }
                if ((comando_rx==2)&&(tx_placa==2))
                { 
                 estado_valvula_1_placa_2_anterior=estado_valvula_1_placa_2;
                 estado_valvula_1_placa_2=(atoi32(dato_aux2));
                }
                numero=2; dat_comp=1;
               }
               if ((numero==2)&&(dat_comp!=1)) 
               { 
                if (comando_rx==1)valor_humedad_dos[tx_placa]=(atoi32(dato_aux2));
                if ((comando_rx==2)&&(tx_placa==1)) 
                 {
                  estado_valvula_2_placa_1_anterior=estado_valvula_2_placa_1;
                  estado_valvula_2_placa_1=(atoi32(dato_aux2));
                 }
                 if ((comando_rx==2)&&(tx_placa==2))
                 {
                  estado_valvula_2_placa_2_anterior=estado_valvula_2_placa_2;
                  estado_valvula_2_placa_2=(atoi32(dato_aux2));
                 }
                 numero=3; dat_comp=1;
               }
               if ((numero==3)&&(dat_comp!=1))
               { 
                if (comando_rx==1)valor_humedad_tres[tx_placa]=(atoi32(dato_aux2));
                if ((comando_rx==2)&&(tx_placa==1)) 
                {
                 tipo_riego_anterior_placa_1=tipo_riego_placa_1;
                 tipo_riego_placa_1=(atoi32(dato_aux2));
                }
                if ((comando_rx==2)&&(tx_placa==2))
                {
                 tipo_riego_anterior_placa_2=tipo_riego_placa_2;
                 tipo_riego_placa_2=(atoi32(dato_aux2));
                }
                numero=4; dat_comp=1;
               }
               if ((numero==4)&&(dat_comp!=1))
               {
                if (comando_rx==1)valor_humedad_cuatro[tx_placa]=(atoi32(dato_aux2));
                if (comando_rx==2);
                numero=0;
                dat_comp=1;
                fin_pal=i;
               }
        }
    }    
}

#task(rate = 35ms)//no cambiar por eeprom pic
void maquina_de_estado_de_comunicacion();
#task(rate = 50ms)//no cambiar por eeprom pic
void reloj_tiempo_real();
#task(rate = 5ms)
void maquina_de_estado_pantallas();
#task(rate = 50ms)
void manejo_bombas();
#task(rate = 5ms)
void maquina_de_estado_teclado_analogico();
#task(rate = 55ms)
void maquina_de_estado_reportes();
#task(rate = 255ms)
void maquina_de_estado_bluetooth();
void main()
{ 
   delay_ms(150);
   UTRDIS = 1; //<----------------------ESTO SI O SI
   desactivar_rele(PIN_A5);
   desactivar_rele(PIN_A2);
   setup_timer_3(T3_DISABLED | T3_DIV_BY_1);
   lcd_init();
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);
   setup_psp(PSP_DISABLED);
   setup_spi(FALSE);
   SETUP_SPI(SPI_DISABLED);
   SETUP_SPI(SPI_SS_DISABLED);
   enable_interrupts(global);
   enable_interrupts(int_rda);
   interruptrb2_init();
   setup_counters(RTCC_INTERNAL,RTCC_DIV_32);
   ds1307_init(DS1307_OUT_ON_DISABLED_HIHG | DS1307_OUT_ENABLED | DS1307_OUT_32_KHZ);
   delay_us(10);
   rtos_run();
}

void manejo_bombas()
{ 
  x=estado_valvula_1_placa_1+estado_valvula_2_placa_1+estado_valvula_1_placa_2+estado_valvula_2_placa_2;
  if ((x<=2)&&(x!=0)) 
  {
   activar_rele(PIN_A2);
   desactivar_rele(PIN_A5);
  }
  if (x>2)
  {
   activar_rele(PIN_A2);
   activar_rele(PIN_A5); 
  }
  if (x==0) 
  {
   desactivar_rele(PIN_A5);
   desactivar_rele(PIN_A2); 
  }
}

void maquina_de_estado_de_comunicacion()
{  
 switch (estado_siguiente){
    case STATE_INI:
    {  
      minima[1]=read_EEPROM (minima_placa_1_e);
      maxima[1]=read_EEPROM (maxima_placa_1_e);
      minima[2]=read_EEPROM (minima_placa_2_e);
      maxima[2]=read_EEPROM (maxima_placa_2_e);
      for(i=0;i<=2;i++)
      {
       salida_1[i]=0;
       salida_2[i]=0;
       salida_3[i]=0;
       salida_4[i]=0;
       sube_1[i]=0;
       sube_2[i]=0;
       sube_3[i]=0;
       sube_4[i]=0;
       baja_1[i]=0;
       baja_2[i]=0;
       baja_3[i]=0;
       baja_4[i]=0;
      }
      veces_registro=0;
      hora_inicio[1]     =0;
      hora_inicio[2]     =0;
      minutos_inicio[1]  =0;
      minutos_inicio[2]  =0;
      hora_fin[1]        =0;
      hora_fin[2]        =0;
      minutos_fin[1]     =0;
      minutos_fin[2]     =0;
      manual_anterior[1]=44;
      manual_anterior[2]=44;
      manual[1]=44;
      manual[2]=44;
      valor_humedad_uno[1]=1;
      valor_humedad_uno[2]=1;
      valor_humedad_dos[1]=1;
      valor_humedad_dos[2]=1;
      valor_humedad_tres[1]=1;
      valor_humedad_tres[2]=1;
      valor_humedad_cuatro[1]=1;
      valor_humedad_cuatro[2]=1;
      comando_tx=1;
      
      tx_placa=2;
      dato_completo=0;
      armar_trama();
      envio_dato();
      while(dato_completo==0)
      {
       if (espera_placa_dos!=max_espera)  espera_placa_dos=(espera_placa_dos+1);
       if (espera_placa_dos==max_espera) dato_completo=1;
       delay_ms(10);
      }
    
      comando_tx=1;
      tx_placa=1;
      dato_completo=0;
      armar_trama();
      envio_dato();
      while(dato_completo==0)
      {
       if (espera_placa_uno!=max_espera)  espera_placa_uno=(espera_placa_uno+1);
       if (espera_placa_uno==max_espera) dato_completo=1;
       delay_ms(10);
      }
     estado_siguiente=STATE_ACTIVO;
     break;
     }
     case STATE_SEND:
     {
      if(( tx_placa==1)) tx_placa=2; else tx_placa=1;
      if ((comando_tx==3)&& (comando_rx==1)) comando_tx=8;
      armar_trama();
      envio_dato();
      if ((comando_tx_anterior==comando_tx)&&(tx_placa_anterior!=tx_placa))comando_tx=8;
      comando_tx_anterior=comando_tx;
      tx_placa_anterior=tx_placa;
      estado_siguiente=STATE_ACTIVO;
     break;
     }
     case STATE_ACTIVO:
     { 
     if ( ((dato_rx[0]=='0') && (dato_rx[1]=='0')&& (dato_rx[2]=='0') && (dato_rx[3]=='0')&&
            (dato_rx[4]=='0') && (dato_rx[5]=='0')&&  (dato_rx[6]=='0')&&(dato_rx[7]=='1')&&( dato_completo==1))||
                     ((((dato_rx[0]=='0') && (dato_rx[1]=='0')&& (dato_rx[2]=='0') && (dato_rx[3]=='0')&&
                  (dato_rx[4]=='0') && (dato_rx[5]=='0')&&  (dato_rx[6]=='1')&&(dato_rx[7]=='0')&&( dato_completo==1)) )))
    {
     deco_tramaRx();
     dato_completo=0;
     if ((tx_placa==1)&& (espera_placa_uno!=max_espera))encender_led(1);
     if ((tx_placa==2)&&(espera_placa_dos!=max_espera))encender_led(2); 
     estado_siguiente=STATE_SEND;
    }
    if ((espera_placa_uno==max_espera)&&(tx_placa==1))
    {
      estado_siguiente=STATE_SEND;
      dato_completo=0;encender_led(0);
    }
    if((espera_placa_dos==max_espera)&&(tx_placa==2))
    {
      estado_siguiente=STATE_SEND;
      dato_completo=0;encender_led(0);
    }
    break;
    }
   }
}

void maquina_de_estado_pantallas()
{ 
switch (estado_siguiente_maquina_lcd){
 case STATE_00:
       lcd_putc('\f');
       lcd_gotoxy(1,1);
       lcd_putc("BIENBENIDOS");
       lcd_gotoxy(1,2);
       lcd_putc("SIST. DE RIEGO");
       lcd_gotoxy(1,3);
       lcd_putc("Version 3.0");
       lcd_gotoxy(1,4);
       lcd_putc("UNCA");
       delay_ms(1050);
       estado_siguiente_maquina_lcd=STATE_01;
       num_veces=0;
       tecla='Z';
       comando_tx=8;
 break;
 case STATE_01:
    if (num_veces==0)
    {
       lcd_putc('\f');
       lcd_gotoxy(1,1);
       lcd_putc(" *FECHA Y HORA*");
       lcd_gotoxy(1,4);
       lcd_putc("Si[F1],No[F2]");
       ind1=0;
       ind2=0; 
       ind3=0;
       ind4=0;
       ind5=0;
       num_veces=1;  tecla='Z';
       for(i=0;i<=2;i++) dato1[i]='0';
       for(i=0;i<=2;i++) dato2[i]='0';
       for(i=0;i<=2;i++) dato3[i]='0'; 
       for(i=0;i<=2;i++) dato4[i]='0';
       for(i=0;i<=2;i++) dato5[i]='0';
       bandera_modifica=0;
       num_veces=1;   tecla='Z';
    }
    if (bandera_modifica==0)
    {
      lcd_gotoxy(1,2);
      lcd_putc("           ");
      lcd_gotoxy(1,2);
      if (dia<10)   printf(lcd_putc,"Fecha:0%u",dia);  else printf(lcd_putc,"Fecha:%u",dia);
      lcd_gotoxy(9,2);
      if (mes<10) printf(lcd_putc,"/0%u",mes); else  printf(lcd_putc,"/%u",mes);
      lcd_gotoxy(12,2);
      if (anio<10) printf(lcd_putc,"/0%u",anio); else printf(lcd_putc,"/%u",anio);
      lcd_gotoxy(1,3);
      lcd_putc("           ");
      lcd_gotoxy(1,3);
      if (hora<10)  printf(lcd_putc,"Hora:0%u:",hora);else printf(lcd_putc,"Hora:%u:",hora);
      lcd_gotoxy(9,3);
      if (minutos<10)  printf(lcd_putc,"0%u",minutos);else printf(lcd_putc,"%u",minutos);
    } 
    if (tecla=='B')
    {
      bandera_modifica=1;
      lcd_gotoxy(1,2);
      lcd_putc("              ");
      lcd_gotoxy(1,2);
      lcd_putc("Fecha:??");
      lcd_putc("/??");
      lcd_putc("/??");
      lcd_gotoxy(1,3);
      lcd_putc("          ");
      lcd_gotoxy(1,3);
      lcd_putc("Hora:??:??");
      ind1=0;
      ind2=0;
      ind3=0;
      ind4=0;
      ind5=0;
      tecla='Z';
    }
    if (bandera_modifica==1)
    {
     ////////////////////////////////////////////FECHA///////////////////////////////////////////////////////////////////// 
     if ((tecla!='Z')&&(ind3<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
     {
        dato3[ind3]=tecla;
        lcd_gotoxy(7+ind3,2);
        lcd_putc(tecla);
        ind3++;
        tecla='Z';
        if (ind3==1)  dato3[2]=0;
     }
     if ((tecla!='Z')&&(ind4<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
     {
        dato4[ind4]=tecla;
        lcd_gotoxy(10+ind4,2);
        lcd_putc(tecla);
        ind4++;
        tecla='Z';
        if (ind4==1)  dato4[2]=0;
     }
     if ((tecla!='Z')&&(ind5<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
     {
        dato5[ind5]=tecla;
        lcd_gotoxy(13+ind5,2);
        lcd_putc(tecla);
        ind5++;
        tecla='Z';
        if (ind5==1)  dato5[2]=0;
     }
     //////////////////////////////////////////HORA///////////////////////////////////////////////////////////////////
     if ((tecla!='Z')&&(ind1<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
     {
        dato1[ind1]=tecla;
        lcd_gotoxy(6+ind1,3);
        lcd_putc(tecla);
        ind1++;
        tecla='Z';
        if (ind1==1)  dato1[2]=0;
     }
     if ((tecla!='Z')&&(ind2<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
     {
        dato2[ind2]=tecla;
        lcd_gotoxy(9+ind2,3);
        lcd_putc(tecla);
        ind2++;
        tecla='Z';
        if (ind2==1)  dato2[2]=0;
       }
     }
     estado_siguiente_maquina_lcd=STATE_01;
     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     if ((tecla=='A')&&(bandera_modifica==1))
     { 
        hora  =atoi(dato1);
        minutos  =atoi(dato2);
        dia  = atoi(dato3);
        mes  = atoi(dato4);
        anio = atoi(dato5);
        dow1 = 2;
        segundos  =  0;
     if ((hora>=0)&&(hora<=23)&&(minutos>=0)&&(minutos<=60)&&(dia>=1)&&(dia<=31)&&(mes>=1)&&(mes<=12)&&(anio>=0)&&(anio<=99))
     {
        bandera_modifica==0;
        delay_ms(15);
        ds1307_set_date_time(dia,mes,anio,dow1,hora,minutos,segundos);
        delay_ms(15); 
        estado_siguiente_maquina_lcd=STATE_02;
        num_veces=0;tecla='Z';
        } else
          {
           lcd_gotoxy(1,3);
           lcd_putc("          ");
           lcd_gotoxy(1,3);
           lcd_putc("error");
           delay_ms(500);
           lcd_gotoxy(1,3);
           lcd_putc("Hora:??:??");
           lcd_gotoxy(1,2);
           lcd_putc("          ");
           lcd_gotoxy(1,2);
           lcd_putc("error");
           delay_ms(500);
           lcd_gotoxy(1,2);
           lcd_putc("Fecha:??");
           lcd_putc("/??");
           lcd_putc("/??");
           ind1=0;
           ind2=0;
           ind3=0;
           ind4=0;
           ind5=0;
           tecla='Z';
           bandera_modifica=1;
          } 
       }
       if ((tecla=='A')&&(bandera_modifica==0))
       {
        estado_siguiente_maquina_lcd=STATE_02;
        tecla='Z'; num_veces=0;
       }
 break;
 case STATE_02:
       if (num_veces==0)
       { 
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc("   **MENU**  1/3");
        lcd_gotoxy(1,2);
        lcd_putc("F1:Configura");
        lcd_gotoxy(1,3);
        lcd_putc("F2:Fecha y Hora");
        lcd_gotoxy(1,4);
        lcd_putc("F3:Mas");
        num_veces=1;  tecla='Z';
       }
       if (tecla=='A')
       {
        estado_siguiente_maquina_lcd=STATE_02_BCD_0;
        tecla='Z'; 
        num_veces=0;
       }
       if (tecla=='B')
       {
        estado_siguiente_maquina_lcd=STATE_01;
        tecla='Z'; 
        num_veces=0;
       }
       if (tecla=='C')
       {
        estado_siguiente_maquina_lcd=STATE_03;
        tecla='Z';
        num_veces=0;
       }
 break;
 case STATE_03:
      if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc("   **MENU**  2/3");
        lcd_gotoxy(1,2);
        lcd_putc("F1:Ver Estado");
        lcd_gotoxy(1,3);
        lcd_putc("F2:Manual ");
        lcd_gotoxy(1,4);
        lcd_putc("F3:Mas");
        num_veces=1; tecla='Z';
       }
       estado_siguiente_maquina_lcd=STATE_03;
       if (tecla=='A')
       {
        estado_siguiente_maquina_lcd=STATE_03_G;
        num_veces=0;
        tecla='Z';
       }
       if (tecla=='B')
       {
        estado_siguiente_maquina_lcd=STATE_03_H;
        num_veces=0;
        tecla='Z';
        manual[1]=45;manual[2]=45;
        comando_tx=4;
       }
       if (tecla=='C') 
       {
        estado_siguiente_maquina_lcd=STATE_04;
        num_veces=0;
        tecla='Z';
       }
 break;
 case STATE_04:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc("   **MENU**  3/3");
        lcd_gotoxy(1,2);
        lcd_putc("F1:Borrar");
        lcd_gotoxy(1,4);
        lcd_putc("F3:Mas");
        num_veces=1;  tecla='Z';
       }
       estado_siguiente_maquina_lcd=STATE_04;
       if (tecla=='A')
       {
        estado_siguiente_maquina_lcd= STATE_04_A;
        tecla='Z';
        num_veces=0;
       }
       if (tecla=='C')
       {
        estado_siguiente_maquina_lcd=STATE_02;
        tecla='Z';
        num_veces=0;
       }
 break;
case STATE_02_BCD_0:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc(" *CONFIGURA* 1/3");
        lcd_gotoxy(1,2);
        lcd_putc("Sector:?");
        lcd_gotoxy(1,3);
        lcd_putc("Si:[F1]");
        lcd_gotoxy(1,4);
        lcd_putc("No[F2]");
        ind1=0;
        num_veces=1;  
        tecla='Z';
        valor_uno=0;
        for(i=0;i<=2;i++) dato1[i]='0';
       }
       if ((tecla!='Z')&&(ind1==0)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
       {
        dato1[ind1]=tecla;
        lcd_gotoxy(8,2);
        lcd_putc(tecla);
        dato1[1]=0;
        ind1=1;
        tecla='Z';
       }
       estado_siguiente_maquina_lcd=STATE_02_BCD_0;
       if (tecla=='A')
       { 
        valor_uno=atoi(dato1);
        if ((valor_uno>0)&&(valor_uno<=max_nodo)&&(((espera_placa_uno!=max_espera)&&(valor_uno!=2))||((espera_placa_dos!=max_espera)&&valor_uno!=1)))
        {
         placa=valor_uno;
         estado_siguiente_maquina_lcd=STATE_02_BCD_1;
         num_veces=0;ind1=0;
    
        } else
        {
         lcd_gotoxy(1,2);
         lcd_putc("        ");
         lcd_gotoxy(1,2);
         lcd_putc("error");
         delay_ms(500);
         lcd_gotoxy(1,2);
         lcd_putc("     ");
         
         lcd_gotoxy(1,2);
         lcd_putc("Sector:?");
        ind1=0;resultado=5;
        } 
       }  
       if (tecla=='B')
       {
        estado_siguiente_maquina_lcd=STATE_02;
        num_veces=0; 
        tecla='Z';
       }
       tecla='Z';
 break;     
 case STATE_02_BCD_1:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc(" *CONFIGURA* 2/3");
        lcd_gotoxy(1,2);
        lcd_putc("F1:Lim. Humedad");
        lcd_gotoxy(1,4);
        lcd_putc("F2:Lim. Horario ");
        num_veces=1;
        tecla='Z';
       }
       estado_siguiente_maquina_lcd=STATE_02_BCD_1;
       if (tecla=='A')
       {
        estado_siguiente_maquina_lcd=STATE_02_B1;
        num_veces=0; 
        tecla='Z';
       }
       if (tecla=='B')
       {
        estado_siguiente_maquina_lcd=STATE_02_C0;
        num_veces=0; 
        tecla='Z';
       }
 break;
 case STATE_02_B1:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        printf(lcd_putc,"SECTOR:%d",placa);
        lcd_gotoxy(1,2);
        printf(lcd_putc,"Min:%lu", minima[placa]);
        lcd_gotoxy(7,2);
        lcd_putc("%");
        lcd_gotoxy(10,2);
        printf(lcd_putc,"Max:%lu",maxima[placa]);
        lcd_gotoxy(16,2);
        lcd_putc("%");
        lcd_gotoxy(1,3);
        lcd_putc("Modifica:[F1]");
        lcd_gotoxy(1,4);
        lcd_putc("Continua:[F2]");
        ind1=0;
        ind2=0;
        num_veces=1; 
        tecla='Z';
        for(i=0;i<=2;i++) dato1[i]='0'; 
        for(i=0;i<=2;i++) dato2[i]='0';
        bandera_modifica=0;valor_uno=0;valor_dos=0;
       }
       if (tecla=='A') 
       {
        bandera_modifica=1;
        lcd_gotoxy(1,2);
        lcd_putc("      ");
        lcd_gotoxy(1,2);
        lcd_putc("Min:??");
        lcd_gotoxy(10,2);
        lcd_putc("      ");
        lcd_gotoxy(10,2);
        lcd_putc("Max:??");
        ind1=0;
        ind2=0;
        tecla='Z';
       }
       if (bandera_modifica==1)
       {
        if ((tecla!='Z')&&(ind1<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
        {
         dato1[ind1]=tecla;
         lcd_gotoxy(5+ind1,2);
         lcd_putc(tecla);
         ind1++;
         tecla='Z';
         if (ind1==1)  dato1[2]=0;
        }
        if ((tecla!='Z')&&(ind2<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
        {
         dato2[ind2]=tecla;
         lcd_gotoxy(14+ind2,2);
         lcd_putc(tecla);
         ind2++;
         tecla='Z';
         if (ind2==1)  dato2[2]=0;
        }
       }
       estado_siguiente_maquina_lcd=STATE_02_B1;
       if (tecla=='B')
       { 
        valor_uno=atoi32(dato1);
        valor_dos=atoi32(dato2);
        if (((valor_uno>0)&&(valor_uno<=99)&&(valor_dos>=0)&&(valor_uno<=99)&&(valor_uno<valor_dos))||(valor_uno==valor_dos))
        {
         estado_siguiente_maquina_lcd=STATE_02_B2;
         num_veces=0;
         tecla='Z';
         if (bandera_modifica==1)
         {
          minima_anterior[placa]= minima[placa];
          maxima_anterior[placa]= maxima[placa];
          minima[placa]=valor_uno;
          maxima[placa]=valor_dos;
          comando_tx=1;//envio limites de humedad
          write_eeprom(minima_placa_1_e, minima[1]);
          write_eeprom(maxima_placa_1_e, maxima[1] );
          write_eeprom(minima_placa_2_e, minima[2]);
          write_eeprom(maxima_placa_2_e, maxima[2]);
         }
         if ((ind1==1)&&(ind2==1))bandera_modifica=0;
        }
          else
          {
            lcd_gotoxy(1,2);
            lcd_putc("      ");
            lcd_gotoxy(1,2);
            lcd_putc("error");
            delay_ms(500);
            lcd_gotoxy(1,2);
            lcd_putc("Min:??");
            lcd_gotoxy(10,2);
            lcd_putc("      ");
            lcd_gotoxy(10,2);
            lcd_putc("error");
            delay_ms(500);
            lcd_gotoxy(10,2);
            lcd_putc("Max:??");
            ind1=0;
            ind2=0;
            tecla='Z';
            bandera_modifica=1;
         } 
       }
       tecla='Z';
 break;
 case STATE_02_B2:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc("LIMITES HUMEDAD");
        lcd_gotoxy(1,2);
        printf(lcd_putc,"Minima:%lu",minima[placa]);
        lcd_gotoxy(10,2);
        lcd_putc('%');
        lcd_gotoxy(1,3);
        printf(lcd_putc,"Maxima:%lu",maxima[placa]);
        lcd_gotoxy(10,3);
        lcd_putc('%');
        lcd_gotoxy(1,4);
        lcd_putc("Continua:[F1]");
        num_veces=1;  tecla='Z';
       }
       estado_siguiente_maquina_lcd=STATE_02_B2;
       if (tecla=='A')
       {
        estado_siguiente_maquina_lcd=STATE_02;
        tecla='Z';
        num_veces=0;
       }
 break;
 case STATE_02_C0:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        printf(lcd_putc,"Sector:%d",placa);
        lcd_gotoxy(1,2);
        printf(lcd_putc,"Inicio:%ld:%ld",hora_inicio[placa], minutos_inicio[placa]);
        lcd_gotoxy(1,3);
        printf(lcd_putc,"   Fin:%ld:%ld",hora_fin[placa],minutos_fin[placa]);
        lcd_gotoxy(1,4);
        lcd_putc("Si:[F1]");
        lcd_gotoxy(9,4);
        lcd_putc("No:[F2]");
        ind1=0;
        ind2=0;
        ind3=0;
        ind4=0;
        num_veces=1;
        tecla='Z';
        for(i=0;i<=2;i++) dato1[i]='0'; 
        for(i=0;i<=2;i++) dato2[i]='0';
        for(i=0;i<=2;i++) dato3[i]='0';
        for(i=0;i<=2;i++) dato4[i]='0';
        bandera_modifica=0;
       }
       if (tecla=='B') 
       {
        bandera_modifica=1;
        lcd_gotoxy(1,2);
        lcd_putc("            ");
        lcd_gotoxy(1,2);
        lcd_putc("Inicio:??:??");
        lcd_gotoxy(1,3);
        lcd_putc("            ");
        lcd_gotoxy(1,3);
        lcd_putc("   Fin:??:??");
        ind1=0; 
        ind2=0; 
        ind3=0; 
        ind4=0;
        tecla='Z';
       }
       if (bandera_modifica==1)
       {
        //////////////////////////////////////////HORA INICIO///////////////////////////////////////////////////////////////////
        if ((tecla!='Z')&&(ind1<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
        {
         dato1[ind1]=tecla;
         lcd_gotoxy(8+ind1,2);
         lcd_putc(tecla);
         ind1++;
         tecla='Z';
         if (ind1==1)  dato1[2]=0;
        }
        if ((tecla!='Z')&&(ind2<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
        {
         dato2[ind2]=tecla;
         lcd_gotoxy(11+ind2,2);
         lcd_putc(tecla);
         ind2++;
         tecla='Z';
         if (ind2==1)  dato2[2]=0;
        }
        if ((ind1==1)&&(ind2==1)&&(ind3==1)&&(ind4==1)&&(ind5==1))bandera_modifica=0;
        estado_siguiente_maquina_lcd=STATE_01;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////HORA FIN////////////////////////////////////////////////////////////////////////
        if ((tecla!='Z')&&(ind3<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
        {
         dato3[ind3]=tecla;
         lcd_gotoxy(8+ind3,3);
         lcd_putc(tecla);
         ind3++;
         tecla='Z';
         if (ind3==1)  dato3[2]=0;
        }
        if ((tecla!='Z')&&(ind4<=1)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
        {
         dato4[ind4]=tecla;
         lcd_gotoxy(11+ind4,3);
         lcd_putc(tecla);
         ind4++;
         tecla='Z';
         if (ind4==1)  dato4[2]=0;
        }
       }
       estado_siguiente_maquina_lcd=STATE_02_C0;
       //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       estado_siguiente_maquina_lcd=STATE_02_C0;
       if (tecla=='A')
       { 
        if (bandera_modifica==1)
        {
         hora_riego_inicio   =atoi(dato1);
         minutos_riego_inicio=atoi(dato2);
         hora_riego_fin      =atoi(dato3);
         minutos_riego_fin   =atoi(dato4); 
        }
       if ((ind1==1)&&(ind2==1)&&(ind3==1)&&(ind4==1))bandera_modifica=0;
       if (((hora_riego_inicio>=0)&&(hora_riego_inicio<=23)&&(minutos_riego_inicio>=0)&&(minutos_riego_inicio<=60))&&
                            ( (hora_riego_fin>=0)&&(hora_riego_fin<=23)&&(minutos_riego_fin>=0)&&(minutos_riego_fin<=60)) &&
                           ((hora_riego_inicio<hora_riego_fin)||(((hora_riego_inicio==hora_riego_fin)&&(minutos_riego_inicio<=minutos_riego_fin)))))
       {
        hora_inicio[placa]    = hora_riego_inicio;
        hora_fin[placa]       = hora_riego_fin; 
        minutos_inicio[placa] = minutos_riego_inicio;
        minutos_fin[placa]    = minutos_riego_fin;
        comando_tx=2;
        estado_siguiente_maquina_lcd=STATE_02;
        num_veces=0;
        tecla='Z';
       } else
          {
           lcd_gotoxy(1,2);
           lcd_putc("      ");
           lcd_gotoxy(1,2);
           lcd_putc("error");
           delay_ms(500);
           lcd_gotoxy(1,2);
           lcd_putc("Inicio:??:??");
           lcd_gotoxy(1,3);
           lcd_putc("      ");
           lcd_gotoxy(1,3);
           lcd_putc("error");
           delay_ms(500);
           lcd_gotoxy(1,3);
           lcd_putc("   Fin:??:??");
           ind1=0;
           ind2=0;
           ind3=0;
           ind4=0;
           tecla='Z';
           bandera_modifica=1;
          } 
      }
      tecla='Z';
 break;
 case STATE_03_G:
       if (num_veces==0)
       {
        lcd_putc('\f');
        lcd_gotoxy(1,1);
        lcd_putc("*VER ESTADO* 1/2");
        lcd_gotoxy(1,2);
        lcd_putc("Sector:?");
        lcd_gotoxy(1,4);
        lcd_putc("Ver:[F1]");
        ind1=0;
        num_veces=1;  
        tecla='Z';
        for(i=0;i<=2;i++) dato1[i]='0'; 
       }
       if ((tecla!='Z')&&(ind1==0)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
       {
        dato1[ind1]=tecla;
        lcd_gotoxy(8,2);
        lcd_putc(tecla);
        dato1[1]=0;
        ind1=1;
        tecla='Z';
       }
      estado_siguiente_maquina_lcd=STATE_03_G;
      if (tecla=='A')
      { 
        resultado=atoi(dato1);
        lcd_gotoxy(1,1);
        if ((resultado>=1)&&(resultado<=max_nodo)&&(((espera_placa_uno!=max_espera)&&(resultado!=2))||
                                                                        ((espera_placa_dos!=max_espera)&&(resultado!=1))))
        {
         placa=resultado;
         estado_siguiente_maquina_lcd=STATE_03_G0;
         num_veces=0;ind1=0;
        } else
           {
            lcd_gotoxy(1,2);
            lcd_putc("        ");
            lcd_gotoxy(1,2);
            lcd_putc("error");
            delay_ms(500);
            lcd_gotoxy(1,2);
            lcd_putc("     ");
            lcd_gotoxy(1,2);
            lcd_putc("Sector:?");
            ind1=0;
            resultado=5;
           } 
      }  
      tecla='Z';
 break;
 case STATE_03_G0:
   if (num_veces==0)
   {
    lcd_putc('\f');
    lcd_gotoxy(1,1);
    lcd_putc("*VER ESTADO* 2/2");
    lcd_gotoxy(1,4);
    lcd_putc("Salir:[F1]");
    lcd_gotoxy(1,2);
    lcd_putc("S1:");
    lcd_gotoxy(9,2);
    lcd_putc("S2:");
    lcd_gotoxy(1,3);
    lcd_putc("S3:");
    lcd_gotoxy(9,3);
    lcd_putc("S4:");
    lcd_gotoxy(6,2);
    lcd_putc("%");
    lcd_gotoxy(14,2);
    lcd_putc("%");
    lcd_gotoxy(6,3);
    lcd_putc("%");
    lcd_gotoxy(14,3);
    lcd_putc("%");
    num_veces=2;
    tecla='Z';
   }
   lcd_gotoxy(4,2);
   lcd_putc("   ");
   lcd_gotoxy(4,2);
   if (valor_humedad_uno[placa]==0)   lcd_putc("OFF"); 
   else 
      {
       printf(lcd_putc,"%2lu",valor_humedad_uno[placa]-1);
       lcd_gotoxy(6,2);
       lcd_putc("%");
      }
      lcd_gotoxy(12,2);
      lcd_putc("   ");
      lcd_gotoxy(12,2);
   if (valor_humedad_dos[placa]==0) lcd_putc("OFF");  
   else 
      {
        printf(lcd_putc,"%2lu",valor_humedad_dos[placa]-1);
        lcd_gotoxy(14,2);
        lcd_putc("%");
      }
   lcd_gotoxy(4,3);
   lcd_putc("   ");
   lcd_gotoxy(4,3);
   if (valor_humedad_tres[placa]==0) lcd_putc("OFF");
   else 
      {
       printf(lcd_putc,"%2lu",valor_humedad_tres[placa]-1);
       lcd_gotoxy(6,3);
       lcd_putc("%");
      }
   lcd_gotoxy(12,3);
   lcd_putc("   ");
   lcd_gotoxy(12,3);
   if (valor_humedad_cuatro[placa]==0) lcd_putc("OFF");
   else
     {
       printf(lcd_putc,"%2lu",valor_humedad_cuatro[placa]-1);
       estado_siguiente_maquina_lcd=STATE_03_G0;
       lcd_gotoxy(14,3);
       lcd_putc("%");
      }
   if (tecla=='A')
   {
     estado_siguiente_maquina_lcd=STATE_03;
     tecla='Z';
     num_veces=0;
   } 
   comando_tx=3;//solicito valores de humedad
 break;
 case STATE_03_H:
   if (num_veces==0)
   {
    lcd_putc('\f');
    lcd_gotoxy(1,1);
    lcd_putc("    *MANUAL* 1/3");
    lcd_gotoxy(1,2);
    lcd_putc("Sector:?");
    lcd_gotoxy(1,3);
    lcd_putc("Valv(0=OFF):?");
    lcd_gotoxy(1,4);
    lcd_putc("Continua:[F1]");
    ind1=0; 
    ind2=0;
    num_veces=1;
    tecla='Z';
    for(i=0;i<=2;i++) dato1[i]='0';
    for(i=0;i<=2;i++) dato2[i]='0';
   }
   if ((tecla!='Z')&&(ind1==0)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
   {
     dato1[ind1]=tecla;
     lcd_gotoxy(8,2);
     lcd_putc(tecla);
     dato1[1]=0;
     ind1=1;
     tecla='Z';
   }
   if ((tecla!='Z')&&(ind2==0)&&(tecla!='A')&&(tecla!='B')&&(tecla!='C')&&(tecla!='D')&&(tecla!='E')&&(tecla!='F'))
   {
     dato2[ind2]=tecla;
     lcd_gotoxy(13,3);
     lcd_putc(tecla);
     dato2[1]=0;
     ind2=1;
     tecla='Z';
   }
   estado_siguiente_maquina_lcd=STATE_03_H;
   if (tecla=='A')
   { 
     tecla='Z';
     resultado1=atoi(dato1);
     resultado2=atoi(dato2);
     if (((resultado1>=1)&&(resultado1<=max_nodo))&&((resultado2>=0)&&(resultado2<=max_valvula))&&
           (((espera_placa_uno!=max_espera)&&(resultado1!=2))||((espera_placa_dos!=max_espera)&&(resultado1!=1))))
     {
      manual_anterior[resultado1]=manual[resultado1];
      if ((((manual[resultado1]==21)&&(resultado2==2))|| ((manual[resultado1]==22)&&(resultado2==1))||
       ((manual[resultado1]==22)&&(resultado2==2))|| ((manual[resultado1]==22)&&(resultado2==1))||
       ((manual[resultado1]==23)&&(resultado2==2))|| ((manual[resultado1]==23)&&(resultado2==1)))||//<-----
       (((manual[resultado1]==11)&&(resultado2==2))|| ((manual[resultado1]==12)&&(resultado2==1))||
       ((manual[resultado1]==12)&&(resultado2==2))|| ((manual[resultado1]==12)&&(resultado2==1))||
       ((manual[resultado1]==13)&&(resultado2==2))|| ((manual[resultado1]==13)&&(resultado2==1))))
      {
       if (resultado1==1) manual[resultado1]=13;
       if (resultado1==2) manual[resultado1]=23;
      }
      else  if (resultado2==0)manual[resultado1]=44;
        else manual[resultado1]=((resultado1*10)+resultado2);
      estado_siguiente_maquina_lcd=STATE_03_H0;
      num_veces=0;
      ind2=0;
      ind1=0;
      comando_tx=4;//envio comando manual
     } else
        {
         lcd_gotoxy(8,2);
         lcd_putc("        ");
         lcd_gotoxy(8,2);
         lcd_putc("error");
         delay_ms(500);
         lcd_gotoxy(8,2);
         lcd_putc("     ");
         lcd_gotoxy(1,2);
         lcd_putc("Sector:?");
         lcd_gotoxy(13,3);
         lcd_putc("        ");
         lcd_gotoxy(12,3);
         lcd_putc("error");
         delay_ms(500);
         lcd_gotoxy(12,3);
         lcd_putc("     ");
         lcd_gotoxy(1,3);
         lcd_putc("Valv(0=OFF):?");
         ind1=0;
         resultado1=5;
         ind2=0;
         resultado2=5;
        } 
  }
 break;
 case STATE_03_H0:
  if (num_veces==0)
   {
     lcd_putc('\f');
     lcd_gotoxy(1,1);
     lcd_putc(" *MANUAL*  2/3  ");
     lcd_gotoxy(1,2);
     lcd_putc("Sector:");
     lcd_gotoxy(1,3);
     lcd_putc("Valv:");
     lcd_gotoxy(1,4);
     lcd_putc("Si[F1],No[F2]");
     num_veces=1;
   }
   ////////PLACA UNO///////
   if ((manual[resultado1]==0)&&(resultado1==1))
   {
    lcd_gotoxy(8,2);
    lcd_putc("UNO->OFF");
    lcd_gotoxy(6,3);
    lcd_putc("TODAS->OFF");
   }
   if (manual[resultado1]==11)
   {
    lcd_gotoxy(8,2);
    lcd_putc("UNO->ON");
    lcd_gotoxy(6,3);
    lcd_putc("UNO->ON");
   }
   if (manual[resultado1]==12)
   {
    lcd_gotoxy(8,2);
    lcd_putc("UNO->ON");
    lcd_gotoxy(6,3);
    lcd_putc("DOS->ON");
   }
   if (manual[resultado1]==13)
   {
    lcd_gotoxy(8,2);
    lcd_putc("UNO->ON");
    lcd_gotoxy(6,3);
    lcd_putc("TODAS->ON");
   }
   if (manual[resultado1]==44)
   {
    lcd_gotoxy(8,2);
    lcd_putc("UNO->OFF");
    lcd_gotoxy(6,3);
    lcd_putc("TODAS->OFF");
   }
   /////////PLACA DOS///////////////
   if ((manual[resultado1]==0)&&(resultado1==2))
   {
    lcd_gotoxy(8,2);
    lcd_putc("DOS->OFF");
    lcd_gotoxy(6,3);
    lcd_putc("TODAS->OFF");
   }
   if (manual[resultado1]==21)
   {
    lcd_gotoxy(8,2);
    lcd_putc("DOS->ON");
    lcd_gotoxy(6,3);
    lcd_putc("UNO->ON");
   }
   if (manual[resultado1]==22)
   {
    lcd_gotoxy(8,2);
    lcd_putc("DOS->ON");
    lcd_gotoxy(6,3);
    lcd_putc("DOS->ON");
   }
   if (manual[resultado1]==23)
   {
    lcd_gotoxy(8,2);
    lcd_putc("DOS->ON");
    lcd_gotoxy(6,3);
    lcd_putc("TODAS->ON");
   }
   if (manual[resultado1]==44)
   {
    lcd_gotoxy(8,2);
    lcd_putc("DOS->OFF");
    lcd_gotoxy(6,3);
    lcd_putc("TODAS->OFF");
   }
   if (tecla=='A')
   {
    estado_siguiente_maquina_lcd=STATE_03_H1;
    tecla='Z';
    num_veces=0;
   }
   if (tecla=='B')
   {
    estado_siguiente_maquina_lcd=STATE_03_H;
    tecla='Z';//manual[resultado1]=11;
    num_veces=0;
   }
 break;
 case STATE_03_H1:       
  if (num_veces==0)
  {
   lcd_putc('\f');
   lcd_gotoxy(1,1);
   lcd_putc(" *MANUAL*  3/3  ");
   lcd_gotoxy(1,2);
   lcd_putc("Continua Manual");
   lcd_gotoxy(1,3);
   lcd_putc("Si:[F1]");
   lcd_gotoxy(1,4);
   lcd_putc("No:[F2]");
   num_veces=1;
  }
  estado_siguiente_maquina_lcd=STATE_03_H1;
  if (tecla=='A')
  {
   estado_siguiente_maquina_lcd=STATE_03_H;
   tecla='Z';
   num_veces=0;
  }
  if (tecla=='B')
  {
   estado_siguiente_maquina_lcd=STATE_03;
   tecla='Z';
   num_veces=0;
   manual[1]=46;
   manual[2]=46;
   comando_tx=4;
  }
 break;
 case STATE_04_A:
  if (num_veces==0)
  {
   lcd_putc('\f');
   lcd_gotoxy(1,1);
   lcd_putc("   *BORRAR*  1/3");
   lcd_gotoxy(1,2);
   lcd_putc("F1:Lim. Humedad");
   lcd_gotoxy(1,4);
   lcd_putc("F2:Lim. Tiempo");
   num_veces=1;
  }
  if (tecla=='A')
  {
   estado_siguiente_maquina_lcd=STATE_04_B;
   num_veces=0;
  }
  if (tecla=='B')
  {
    estado_siguiente_maquina_lcd=STATE_04_C;
    tecla='Z';
    num_veces=0;
  }
break;
case STATE_04_B:
  if (num_veces==0)
  {
   lcd_putc('\f');
   lcd_gotoxy(1,1);
   lcd_putc("   *BORRAR*  2/3"); 
   lcd_gotoxy(1,2);
   lcd_putc("Lim. Humedad:?");
   lcd_gotoxy(1,3);
   lcd_putc("Si:[F3]");
   lcd_gotoxy(1,4);
   lcd_putc("No:[F2]");
   num_veces=1;
  }
  if (tecla=='C')
  {
   minima[1]=0;
   maxima[1]=0;
   minima[2]=0;
   maxima[2]=0;
   write_eeprom(minima_placa_1_e, minima[1]);
   write_eeprom(minima_placa_2_e, minima[2]);
   write_eeprom(maxima_placa_1_e, maxima[1] );
   write_eeprom(maxima_placa_2_e, maxima[2]);
   estado_siguiente_maquina_lcd=STATE_04_D;
   comando_tx=1;
   num_veces=0;
  }
  if (tecla=='B')
  {
   estado_siguiente_maquina_lcd=STATE_04;
   tecla='Z';
   num_veces=0;
  }
  break;   
case STATE_04_C:
  if (num_veces==0)
  {
   lcd_putc('\f');
   lcd_gotoxy(1,1);
   lcd_putc("   *BORRAR*  2/3"); 
   lcd_gotoxy(1,2);
   lcd_putc("Limites Tiempo:?");
   lcd_gotoxy(1,3);
   lcd_putc("Si:[F3]");
   lcd_gotoxy(1,4);
   lcd_putc("No:[F2]");
   num_veces=1;
  }
  if (tecla=='C')
  {
   hora_inicio[1]     =0;
   hora_inicio[2]     =0;
   minutos_inicio[1]  =0;
   minutos_inicio[2]  =0;
   hora_fin[1]        =0;
   hora_fin[2]        =0;
   minutos_fin[1]     =0;
   minutos_fin[2]     =0;
   comando_tx=2;
   estado_siguiente_maquina_lcd=STATE_04_D;
   num_veces=0;
  }
  if (tecla=='B')
  {
   estado_siguiente_maquina_lcd=STATE_04;
   tecla='Z';
   num_veces=0;
  } 
 break;
case STATE_04_D:
  if (num_veces==0)
  {
   lcd_putc('\f');
   lcd_gotoxy(1,1);
   lcd_putc("   *BORRAR*  3/3"); 
   lcd_gotoxy(1,2);
   lcd_putc("El reinicio fue");
   lcd_gotoxy(1,3);
   lcd_putc("exitoso");
   lcd_gotoxy(1,4);
   lcd_putc("Continuar:[F1]");
   num_veces=1;
  }
  estado_siguiente_maquina_lcd=STATE_04_D;
  if (tecla=='A')
  {
   estado_siguiente_maquina_lcd=STATE_04;
   tecla='Z'; 
   num_veces=0;
  } 
 }
}

void maquina_de_estado_teclado_analogico()
{ 
 tarea_teclado_analogico();
 setup_adc_ports(NO_ANALOGS);
}

void reloj_tiempo_real()
{
  delay_ms(15);
  ds1307_get_time(hora,minutos,segundos);
  delay_ms(15); 
  ds1307_get_date(dia,mes,anio,dow1);
  delay_ms(15);
}
void maquina_de_estado_reportes()
{
 switch(estado_siguiente_maquina_reportes){
 case ST_INI:
  if (leer_MEM(0)<1022)
  {  
    guardar_MEM(200,(long int)leer_MEM(0)+1);
    guardar_MEM(200,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    guardar_MEM(201,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    guardar_MEM((byte)dia,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    guardar_MEM((byte)mes,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    guardar_MEM((byte)anio,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    guardar_MEM((byte)hora,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    guardar_MEM((byte)minutos,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    if ((espera_placa_uno!=max_espera))
    {
     guardar_MEM(204,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
     inicializacion_1=1;
    }
    if ((espera_placa_dos!=max_espera))
    {
     guardar_MEM(205,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
     inicializacion_2=1;
    }
    if ((espera_placa_uno==max_espera)&&(espera_placa_dos==max_espera))
    {
     guardar_MEM(206,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);inicializacion_1=1;inicializacion_2=1;
    }
    estado_siguiente_maquina_reportes=ST_LIMITES_HUMEDAD;
    veces_registro=0;
 }
 break;
 case ST_ACTIVO:
  if (((((estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior) ||(estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior))&&
                                                                                             ((tipo_riego_placa_1==2)&&(manual[1]!= manual_anterior[1])))||
        (( (estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)||(estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior))&&
                                                                                               ( (tipo_riego_placa_2==2)&&(manual[2]!= manual_anterior[2]))))&&(leer_MEM(0)<1022))
        estado_siguiente_maquina_reportes=ST_APERTURA_VALVULA_MANUAL;
    
  if (((((estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior) ||(estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior))&&(tipo_riego_placa_1==1))||
        (((estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)||(estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior))&&(tipo_riego_placa_2==1)))&&
             (leer_MEM(0)<1022))//&&(manual[1]!=45)&&(manual[2]!=45)&&(manual[1]!=46)&&(manual[2]!=46)
        estado_siguiente_maquina_reportes=ST_APERTURA_VALVULA_HUMEDAD;
    
  if (((minutos==0)&&(veces_registro==0))&&(leer_MEM(0)<1022))estado_siguiente_maquina_reportes=ST_REGISTRO_VALORES_HUMEDAD;
  if (minutos!=0) veces_registro=0;
  if (((minima[1]!=minima_anterior[1])||(minima[2]!=minima_anterior[2])||(maxima[1]!=maxima_anterior[1])||(maxima[2]!=maxima_anterior[2])) &&(leer_MEM(0)<1022))
        estado_siguiente_maquina_reportes=ST_LIMITES_HUMEDAD;

  if (((((estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior) ||(estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior))&&
                                                                                             ((tipo_riego_placa_1==3)))||
        (( (estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)||(estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior))&&
                                                                                               ( (tipo_riego_placa_2==3))))&&(leer_MEM(0)<1022))
        estado_siguiente_maquina_reportes=ST_APERTURA_VALVULA_TIEMPO;
 break;
 case ST_LIMITES_HUMEDAD:
  guardar_MEM(203,(long int)leer_MEM(0)+1);
  guardar_MEM((long int)leer_MEM(0)+1,0);
  if ((espera_placa_uno!=max_espera))
  {
   guardar_MEM((byte)minima[1],(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   guardar_MEM((byte)maxima[1],(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
  }
  if ((espera_placa_dos!=max_espera))
  {
   guardar_MEM((byte)minima[2],(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   guardar_MEM((byte)maxima[2],(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
  }
  minima_anterior[1]=minima[1];
  minima_anterior[2]=minima[2];
  maxima_anterior[1]=maxima[1];
  maxima_anterior[2]=maxima[2];
  estado_siguiente_maquina_reportes=ST_REGISTRO_VALORES_HUMEDAD;
 break;
 case ST_APERTURA_VALVULA_TIEMPO:
   if((((estado_valvula_1_placa_1==1)&&(estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior))) ||
                    ((estado_valvula_2_placa_1==1)&&(estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior)))
   {
    guardar_MEM(230,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    estado_valvula_1_placa_1_anterior=estado_valvula_1_placa_1;
    estado_valvula_2_placa_1_anterior=estado_valvula_2_placa_1;
   } 
   if(((estado_valvula_1_placa_2==1)&&((estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)))||
                 (( (estado_valvula_2_placa_2==1))&&(estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior)))
   {
    guardar_MEM(233,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    estado_valvula_1_placa_2_anterior=estado_valvula_1_placa_2;
    estado_valvula_2_placa_2_anterior=estado_valvula_2_placa_2;
   }
   if(((estado_valvula_1_placa_1==0)&&(estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior)))
   {
    guardar_MEM(231,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    estado_valvula_1_placa_1_anterior=estado_valvula_1_placa_1;
   }
   if (((estado_valvula_2_placa_1==0))&&(estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior))
   {
    guardar_MEM(232,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    estado_valvula_2_placa_1_anterior=estado_valvula_2_placa_1;
   }
   if((estado_valvula_1_placa_2==0)&&(estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior))
   {
    guardar_MEM(234,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    estado_valvula_1_placa_2_anterior=estado_valvula_1_placa_2;
   }
   if ((estado_valvula_2_placa_2==0)&&(estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior))
   {
    guardar_MEM(235,(long int)leer_MEM(0)+1);
    guardar_MEM((long int)leer_MEM(0)+1,0);
    estado_valvula_2_placa_2_anterior=estado_valvula_2_placa_2;
   }
   guardar_MEM((byte)hora,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   guardar_MEM((byte)minutos,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_siguiente_maquina_reportes=ST_ACTIVO;
 break;
 case ST_REGISTRO_VALORES_HUMEDAD:
  if ((valor_humedad_uno[1]!=0)&&(valor_humedad_dos[1]!=0))  humedad_valvula_uno_placa_uno=((valor_humedad_uno[1]+valor_humedad_dos[1])/2);
        else if(valor_humedad_uno[1]==0) humedad_valvula_uno_placa_uno=valor_humedad_dos[1];
           else if(valor_humedad_dos[1]==0) humedad_valvula_uno_placa_uno=valor_humedad_uno[1];
  
  if ((valor_humedad_tres[1]!=0)&&(valor_humedad_cuatro[1]!=0))  humedad_valvula_dos_placa_uno=((valor_humedad_tres[1]+valor_humedad_cuatro[1])/2);
        else if(valor_humedad_tres[1]==0) humedad_valvula_dos_placa_uno=valor_humedad_cuatro[1];
           else if(valor_humedad_cuatro[1]==0) humedad_valvula_dos_placa_uno=valor_humedad_tres[1];
 
  if ((valor_humedad_uno[2]!=0)&&(valor_humedad_dos[2]!=0))  humedad_valvula_uno_placa_dos=((valor_humedad_uno[2]+valor_humedad_dos[2])/2);
        else if(valor_humedad_uno[2]==0) humedad_valvula_uno_placa_dos=valor_humedad_dos[2];
           else if(valor_humedad_dos[2]==0) humedad_valvula_uno_placa_dos=valor_humedad_uno[2];

  if ((valor_humedad_tres[2]!=0)&&(valor_humedad_cuatro[2]!=0))  humedad_valvula_dos_placa_dos=((valor_humedad_tres[2]+valor_humedad_cuatro[2])/2);
        else if(valor_humedad_tres[2]==0) humedad_valvula_dos_placa_dos=valor_humedad_cuatro[2];
           else if(valor_humedad_cuatro[2]==0) humedad_valvula_dos_placa_dos=valor_humedad_tres[2];
  guardar_MEM(202,(long int)leer_MEM(0)+1);
  guardar_MEM((long int)leer_MEM(0)+1,0);
  if ((espera_placa_uno!=max_espera))
  {
   guardar_MEM((byte)humedad_valvula_uno_placa_uno,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   guardar_MEM((byte)humedad_valvula_dos_placa_uno,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
  }
  if ((espera_placa_dos!=max_espera))
  {
   guardar_MEM((byte) humedad_valvula_uno_placa_dos,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   guardar_MEM((byte)humedad_valvula_dos_placa_dos,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
  }
  estado_siguiente_maquina_reportes=ST_APERTURA_VALVULA_HUMEDAD;
  veces_registro=1;
 break;
 case ST_APERTURA_VALVULA_HUMEDAD:
  if((estado_valvula_1_placa_1==1) &&((estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior)||(inicializacion_1==1)))
  {
   guardar_MEM(220,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_1_placa_1_anterior=estado_valvula_1_placa_1;
  }
  if((estado_valvula_1_placa_1==0) &&((estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior)||(inicializacion_1==1)))
  {
   guardar_MEM(221,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_1_placa_1_anterior=estado_valvula_1_placa_1;
  }
  if((estado_valvula_2_placa_1==1) &&((estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior)||(inicializacion_1==1)))
  {
   guardar_MEM(222,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_2_placa_1_anterior=estado_valvula_2_placa_1;
  }
  if((estado_valvula_2_placa_1==0)&&((estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior)||(inicializacion_1==1)))
  {
   guardar_MEM(223,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_2_placa_1_anterior=estado_valvula_2_placa_1;
  }
  if((estado_valvula_1_placa_2==1)&&((estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)||(inicializacion_2==1)))
  {
   guardar_MEM(224,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_1_placa_2_anterior=estado_valvula_1_placa_2;
  }
  if((estado_valvula_1_placa_2==0)&&((estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)||(inicializacion_2==1)))
  {
   guardar_MEM(225,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_1_placa_2_anterior=estado_valvula_1_placa_2;
  }
  if((estado_valvula_2_placa_2==1)&&((estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior)||(inicializacion_2==1)))
  {
   guardar_MEM(226,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_2_placa_2_anterior=estado_valvula_2_placa_2;
  }
  if((estado_valvula_2_placa_2==0)&&((estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior)||(inicializacion_2==1)))
  {
   guardar_MEM(227,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_valvula_2_placa_2_anterior=estado_valvula_2_placa_2;
  }
  guardar_MEM((byte)hora,(long int)leer_MEM(0)+1);
  guardar_MEM((long int)leer_MEM(0)+1,0);
  guardar_MEM((byte)minutos,(long int)leer_MEM(0)+1);
  guardar_MEM((long int)leer_MEM(0)+1,0);
  inicializacion_1=0;
  inicializacion_2=0;
  estado_siguiente_maquina_reportes=ST_ACTIVO;
 break;
 case ST_APERTURA_VALVULA_MANUAL:
   if ((estado_valvula_1_placa_1!=estado_valvula_1_placa_1_anterior) ||(estado_valvula_2_placa_1!=estado_valvula_2_placa_1_anterior))
   {
    if (manual[1]==11)
    {
     guardar_MEM(210,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if (manual[1]==12)
    {
     guardar_MEM(211,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if((((manual[1]==44)))||((manual[1]==46)))
    {
     guardar_MEM(212,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if ((manual[1]==13)&&(manual_anterior[1]==11)) 
    {
     guardar_MEM(211,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    manual_anterior[1]=manual[1];
     }
   if ((estado_valvula_1_placa_2!=estado_valvula_1_placa_2_anterior)||(estado_valvula_2_placa_2!=estado_valvula_2_placa_2_anterior))
   {
    if ((manual[1]==23)&&(manual_anterior[1]==12))
    {
     guardar_MEM(210,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if (manual[2]==21)
    {
     guardar_MEM(213,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if (manual[2]==22) 
    {
     guardar_MEM(214,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if (((manual[2]==44))||((manual[2]==46)))
    {
     guardar_MEM(215,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if ((manual[2]==23)&&(manual_anterior[2]==21)) 
    {
     guardar_MEM(214,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    if ((manual[2]==23)&&(manual_anterior[2]==22)) 
    {
     guardar_MEM(213,(long int)leer_MEM(0)+1);
     guardar_MEM((long int)leer_MEM(0)+1,0);
    }
    manual_anterior[2]=manual[2];
   }
   guardar_MEM((byte)hora,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   guardar_MEM((byte)minutos,(long int)leer_MEM(0)+1);
   guardar_MEM((long int)leer_MEM(0)+1,0);
   estado_siguiente_maquina_reportes=ST_ACTIVO;
break;
  }
}

void maquina_de_estado_bluetooth()
{
 switch (estado_siguiente_maquina_blue){
    case ST_INI_BLUE:
      estado_siguiente_maquina_blue=ST_BLUE_ACTIVO;
   break;
   case ST_BLUE_ACTIVO:
     if (tx_blue==1) estado_siguiente_maquina_blue= ST_BLUE_TX;
     if (rx_blue==1)estado_siguiente_maquina_blue=ST_BLUE_RX;
   break;
   case ST_BLUE_TX:
     fprintf(PORT2,"Datos guardados:\n\r");
     for(i=0;i<=(long int)leer_MEM(0);i++)    
     {
       delay_us(650);
       itoa((int)leer_MEM(i),10, string);
       fprintf(PORT2, string) ;
       fprintf(PORT2,";") ;   
       if ((int)leer_MEM(i+1)>100) fprintf(PORT2,"\n\r");
     }
     fprintf(PORT2,"\n\r");
     tx_blue=0;
     estado_siguiente_maquina_blue=ST_BLUE_ACTIVO;
   break;
   case ST_BLUE_RX:
     if  (caracter_rx_blue=='E')
     {
      caracter_anterior_rx_blue='E';
      fprintf(PORT2,"Eliminar historial(s/n)?:\n");
     }
     if ((caracter_anterior_rx_blue=='E')&&(caracter_rx_blue=='N'))
     {
      caracter_anterior_rx_blue='k';
      fprintf(PORT2,"El historial no se elimino\n\r");
     }
     if ((caracter_anterior_rx_blue=='E')&&(caracter_rx_blue=='S'))
     {
      guardar_MEM(0,0);
      fprintf(PORT2,"Se elimino el historial\n\r");
      estado_siguiente_maquina_reportes =ST_INI;
     }
     rx_blue=0;
     estado_siguiente_maquina_blue=ST_BLUE_ACTIVO;
 break;
}
}
 

