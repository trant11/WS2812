// led ring k miutnák

#include "milis.h"
#include "stm8s.h"
#include "delay.h"


#define L_PATTERN 0b01110000 // 3x125ns (8MHZ SPI)
#define H_PATTERN 0b01111100 // 5x125ns (8MHZ SPI), first and last bit must be zero (to remain
             // MOSI in Low between frames/bits)

void zhasniLED(uint16_t cisloLED);
void rozsvitLED(uint16_t cisloLED, uint8_t r, uint8_t g, uint8_t b);
void init_spi(void);
void test(uint8_t *data, uint16_t delka);
void beziciLED(void);
// void hodiny(void);

uint8_t cisloLED = 0; // cislo aktualni LED
uint8_t minule = 0;

uint8_t red(uint16_t x);
uint8_t green(uint16_t x);
uint8_t blue(uint16_t x);
uint16_t x = 0;
uint8_t colors[72] = { // array do kterého se zapisuje barvy na určité LEDce
    0};

/*
void delay_ms(uint16_t i)
{
        for (i=0; i<n; inc-expression) {
        statements
        }
}
*/
void delay_ms(uint16_t ms) {
  uint16_t i;
  for (i = 0; i < ms; i = i + 1) {
    _delay_us(250);
    _delay_us(250);
    _delay_us(250);
    _delay_us(250);
  }
}

void main(void) {
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // 16MHz from internal RC
  init_milis(); // millis using TIM4 - not necessary
  init_spi();

  while (1) {
    if (GPIO_ReadInputPin(GPIOE, GPIO_PIN_4) == RESET &&
        minule == 1) { // na tlacitko rozsviti nasledujici LED "větší" barvou
                       // podle spektra barev
      minule = 0;
      rozsvitLED(cisloLED, red(x), green(x), blue(x));
      cisloLED++;
      if (x < 254) { // dokud neni x nejvesti barva zvetsuje barvu - na konci
                     // spektra je jen cervena
        x = x + 8;   // pricte barvu
      }
      if (cisloLED >= 24) { // pocet LEDek na ringu
        cisloLED = 0;
      }
    }
    if (GPIO_ReadInputPin(GPIOE, GPIO_PIN_4) != RESET) {
      minule = 1;
    }
  }
}

// takes array of LED_number * 3 bytes (RGB per LED)
void test(uint8_t *data, uint16_t length) {
  uint8_t mask;
  disableInterrupts(); // can be omitted if interrupts do not take more then
                       // about ~25us
  while (length) {     // for all bytes from input array
    length--;
    mask = 0b10000000; // for all bits in byte
    while (mask) {
      while (!(SPI->SR & SPI_SR_TXE))
        ; // wait for empty SPI buffer
      if (mask &
          data[length]) { // send pulse with coresponding length ("L" od "H")
        SPI->DR = H_PATTERN;
      } else {
        SPI->DR = L_PATTERN;
      }
      mask = mask >> 1;
    }
  }
  enableInterrupts();
  while (SPI->SR & SPI_SR_BSY)
    ; // wait until end of transfer - there should come "reset" (>50us in Low)
}

void init_spi(void) {
  // Software slave managment (disable CS/SS input), BiDirectional-Mode release
  // MISO pin to general purpose
  SPI->CR2 |= SPI_CR2_SSM | SPI_CR2_SSI | SPI_CR2_BDM | SPI_CR2_BDOE;
  SPI->CR1 |= SPI_CR1_SPE | SPI_CR1_MSTR; // Enable SPI as master at maximum
                                          // speed (F_MCU/2, there 16/2=8MHz)
}

void rozsvitLED(uint16_t cisloLED, uint8_t r, uint8_t g, uint8_t b) { // 0-23
  uint16_t x = cisloLED * 3; // každá LED ma 3 bajty proto krát3
  colors[x] = b; // do array colors na pozici x připišu barvu, teď modrou která
                 // je v tuto chvíli 0, svítím jen červeně
  x++;
  colors[x] = r;
  x++;
  colors[x] = g;
  test(colors, sizeof(colors)); // pošlu array "colors" do led ringu
  delay_ms(2);
}

void zhasniLED(uint16_t cisloLED) { // 0-23
  uint16_t x = cisloLED * 3;        // každá LED ma 3 bajty proto krát3
  colors[x] = 0; // do array colors na pozici x připišu barvu, teď žádnou
                 // protoože chci zhasnout
  x++;
  colors[x] = 0;
  x++;
  colors[x] = 0;
  test(colors, sizeof(colors)); // pošlu array "colors" do led ringu
  delay_ms(2);
}

/*void zhasniVse(void){
        uint16_t x = 0;
        while (x<255){
        colors[x] = 0; //do array colors na pozici x připišu barvu, teď žádnou
protoože chci zhasnout x++; colors[x] = 0; x++; colors[x] = 0;
        test(colors,sizeof(colors));  // pošlu array "colors" do led ringu
        delay_ms(2);
        }
}*/

void beziciLED(void) { // svítí jen jedna LED bezi pořád dokola
  uint8_t cisloLED = 0;
  if (cisloLED <= 23) {
    rozsvitLED(cisloLED, red(x), green(x), blue(x));
    delay_ms(50);
    zhasniLED(cisloLED);
    cisloLED++;
  } else {
    cisloLED = 0;
  }
}

/*void hodiny(void){
        uint8_t cisloLED = 0;
                rozsvitLED(cisloLED,red(x),green(x),blue(x));
                cisloLED++;
                x = x+2;
                delay_ms(50);
                if(cisloLED>23){
                        cisloLED = 0;
                }
        }*/

uint8_t
green(uint16_t x) { // enkoderem nastavim hodnotu 0-255 vypocita se nalezici
                    // barva, od ktere se bude později odcitat - minutky
  x = x % 256;
  if (x < 42) {
    return x * 255 / 42;
  }
  if (x < 128) {
    return 255;
  }
  if (x < 170) {
    return 255 - ((x - 128) * 255 / 42);
  }
  return 0;
}

uint8_t red(uint16_t x) { return green(x - 170); }

uint8_t blue(uint16_t x) { return green(x - 85); }
