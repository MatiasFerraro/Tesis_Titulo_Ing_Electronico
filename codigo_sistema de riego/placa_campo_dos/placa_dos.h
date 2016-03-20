#include <16C74.h>
#device adc=8

#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES RC                    	//Resistor/Capacitor Osc with CLKOUT
#FUSES PUT                   	//Power Up Timer
#FUSES NOPROTECT             	//Code not protected from reading

#use delay(clock=20000000)

