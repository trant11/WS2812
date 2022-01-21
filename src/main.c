#include "stm8s.h"
#include "delay.h"
#include "milis.h"
#include "stm8_hd44780.h"
#include "stdio.h"

#define swt_PORT GPIOB
#define swt_PIN GPIO_PIN_7
#define enc_swt (GPIO_ReadInputPin(swt_PORT, swt_PIN)== RESET)

#define NCODER_CLK_PORT GPIOD
#define NCODER_DATA_PORT GPIOD
#define NCODER_CLK_PIN GPIO_PIN_5
#define NCODER_DATA_PIN GPIO_PIN_6
#define NCODER_GET_CLK (GPIO_ReadInputPin(NCODER_CLK_PORT, NCODER_CLK_PIN)!=RESET)
#define NCODER_GET_DATA (GPIO_ReadInputPin(NCODER_DATA_PORT, NCODER_DATA_PIN)!=RESET)

//LCD DEFINE
#define LCD_RS_PORT GPIOF
#define LCD_RW_PORT GPIOF
#define LCD_E_PORT GPIOF
#define LCD_D4_PORT GPIOG
#define LCD_D5_PORT GPIOG
#define LCD_D6_PORT GPIOG
#define LCD_D7_PORT GPIOG

#define LCD_RS_PIN GPIO_PIN_7
#define LCD_RW_PIN GPIO_PIN_6
#define LCD_E_PIN GPIO_PIN_5
#define LCD_D4_PIN GPIO_PIN_0
#define LCD_D5_PIN GPIO_PIN_1
#define LCD_D6_PIN GPIO_PIN_2
#define LCD_D7_PIN GPIO_PIN_3

void setup(void){
CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //Taktovat MCU 16 MHz

//GPIO SETUP
GPIO_Init(LCD_RS_PORT, LCD_RS_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(LCD_RW_PORT, LCD_RW_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(LCD_E_PORT, LCD_E_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(LCD_D4_PORT, LCD_D4_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(LCD_D5_PORT, LCD_D5_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(LCD_D6_PORT, LCD_D6_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(LCD_D7_PORT, LCD_D7_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

GPIO_Init(swt_PORT,swt_PIN,GPIO_MODE_IN_FL_NO_IT);
GPIO_Init (NCODER_CLK_PORT, NCODER_CLK_PIN, GPIO_MODE_IN_PU_NO_IT );
GPIO_Init (NCODER_DATA_PORT, NCODER_DATA_PIN, GPIO_MODE_IN_PU_NO_IT );
}

//Proměnné
char text[32];
uint16_t enc_value = 0;
uint32_t time_enc = 0;
int minuly = 0;  //nespecifukuje jak velký (kolik Bytů) má datový typ: INT, SHORT, LONG, CHAR jak třeba uint8_t
/*
uint32_t time_enc_swt = 0;
uint32_t time_ye = 0;
uint32_t time_run = 0;
uint32_t time_error = 0;
uint16_t akt = 0;
int16_t max = 0;
_Bool aktualni_stav = 0;
_Bool minuly_stav = 0;
_Bool run = 0;
_Bool porucha = 0;
*/

int8_t check_ncoder(void){ //funkce pro ncoder
if (minuly == 0 && NCODER_GET_CLK == 1){
minuly = 1;
if (NCODER_GET_DATA == 0){
return 1;
}else{
return -1;
}
}else if (minuly == 1 && NCODER_GET_CLK ==0){
minuly = 0;
if (NCODER_GET_DATA ==0){
return -1;
}else{
return 1;
}
}
return 0;
}

void main(void){
setup();
lcd_init();
init_milis();
lcd_clear();

while(1){

    if (milis() - time_enc > 2){
    time_enc = milis();
    enc_value += check_ncoder(); //Kontrola ncoderu
      
    lcd_gotoxy(0,0);
    sprintf(text,"hodnota: %5d", enc_value);
    lcd_puts(text);
    }

    if ( enc_value < 0){ //Ošetření přetečení
     enc_value=99;
    }else if ( enc_value > 99){
     enc_value=0;
    }
    
/*
    if (enc_swt == 0){
    lcd_gotoxy(0,0);
    lcd_puts("Vyberte tlacitko");
    }

    else {
    lcd_gotoxy(0,0);
    sprintf(text, "0%10u");
    lcd_puts(text);
*/


/*
if (milis() - time_ncoder > 7){
if (run==0){
time_ncoder = milis();
max += check_ncoder(); //Kontrola ncoderu
if (max < 0){ //Ošetření přetečení
max=99;
}else if (max > 99){
max=0;
}
}
}
if (milis() - time_enc_swt > 90){
time_enc_swt=milis();
if (enc_swt){ //Kontrola tl. na ncoderu
aktualni_stav=1;
}else{
aktualni_stav=0;
}
if (minuly_stav==1 && aktualni_stav==0){
minuly_stav=0;
if (run==0){ //Spuštení programu
run=1;
}else if (run==1){ //Vypnutí programu
run=0;

}
}
minuly_stav=aktualni_stav;
}
}

if (milis() - time_run > 150){
time_run=milis();
if (run==1 && porucha==0){ // zobrazení displeje při chodu programu
lcd_clear();
sprintf(text,"Max.:%3u",max);
lcd_puts(text);
}else if(run==0){ //zobraní displeje při stop módu
lcd_gotoxy(8,0);
sprintf(text,"Max.:%3u",max);
}
}

if (TL==3){
lcd_clear();
lcd_gotoxy(0,0);
sprintf(Text_TL_A,"f: 20Hz TH: %ums",Time_TL3_H);
lcd_puts(Text_TL_A);
lcd_gotoxy(8,1);
sprintf(Text_TL_B,"TL: %ums" ,Time_TL3_L);
lcd_puts(Text_TL_B);
}


//if (milis() - time_ye > 190){
//time_ye=milis();
//if (run==0 && porucha==0){ //Při stop módu rozsvítit žlutou LED
//}

*/
}
}