#include "stm8s.h"
#include "milis.h"
#include "delay.h"

#define _ISOC99_SOURCE
#define _GNU_SOURCE


void init_spi(void)
{
    // Software slave managment (disable CS/SS input), BiDirectional-Mode release MISO pin to general purpose
    SPI->CR2 |= SPI_CR2_SSM | SPI_CR2_SSI | SPI_CR2_BDM | SPI_CR2_BDOE;
    // Enable SPI as master at maximum speed (F_MCU/2, there 16/2=8MHz)
    SPI->CR1 |= SPI_CR1_SPE | SPI_CR1_MSTR;
}


#define L_PATTERN 0b01110000    // 3x125ns (8MHZ SPI)
#define H_PATTERN 0b01111100    // 5x125ns (8MHZ SPI), first and last bit must be zero (to remain MOSI in Low between frames/bits)
// takes array of LED_number * 3 bytes (RGB per LED)

void neopixel(uint8_t * data, uint16_t length)
{
    uint8_t mask;
    disableInterrupts();        // can be omitted if interrupts do not take more then about ~25us
    while(length--) {            // for all bytes from input array
        mask = 0b10000000;     // for all bits in byte
        while (mask) {
            while (!(SPI->SR & SPI_SR_TXE));    // wait for empty SPI buffer
            if (mask & data[length]) {  // send pulse with coresponding length ("L" od "H")
                SPI->DR = H_PATTERN;
            } else {
                SPI->DR = L_PATTERN;
            }
            mask = mask >> 1;
        }
    }
    enableInterrupts();
    while (SPI->SR & SPI_SR_BSY); // wait until end of transfer - there should come "reset" (>50us in Low)
}


// BRG
uint8_t colors[24] = {
    0, 0, 0,     
    0, 0, 0,    
    0, 50, 50,   
    0, 0, 100,    
    0, 0, 10,   
    0, 0, 80, 
    0, 10, 0,         
    10, 0, 0
};

void delay_ms(uint16_t ms) {
    uint16_t  i;
    for (i=0; i<ms; i = i+1){
        _delay_us(250);
        _delay_us(248);
        _delay_us(250);
        _delay_us(250);
    }
}

int main(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    init_spi();

        neopixel(colors, sizeof(colors));
        delay_ms(2);

    while (1) {
        colors[4]=colors[4]+20;
        if   (colors[4]>=235){
            colors[4]=0;
        }
        delay_ms(100);
        neopixel(colors, sizeof(colors));

        if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_6)==0){
            colors[15]=50;
            neopixel(colors, sizeof(colors));
	}
    }
 }

