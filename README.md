# PROJEKT
====================

## STM8
Startovací strom zdrojových kódů pro výuku mikroprocesorové techniky s kitem
[NUCLEO-8S208RB](https://www.st.com/en/evaluation-tools/nucleo-8s208rb.html).
Strom je určen pro překladač [SDCC](http://sdcc.sourceforge.net/). Standardní
knihovnu pro práci s periferiemi 
[SPL](https://www.st.com/content/st_com/en/products/embedded-software/mcu-mpu-embedded-software/stm8-embedded-software/stsw-stm8069.html)
je třeba (z licenčních důvodů) stáhnou zvlášť ze stránek výrobce a použít
[patch](https://github.com/gicking/STM8-SPL_SDCC_patch).

## Popis:
Projekt představuje indikaci stavu baterie.
Skládá se z několika periferií: rotační n-kodér, NeoPixel modul s 8 RGB LED WS2812, LCD displej HD44780
Všechny tyto periferie jsou drátově propojeny s STM8 kitem na nepájivém poli.
K napájení nám stačí USB/ micro-USB kabel, který napájí STM8 kit napětím 5V. Všechny ostatní periferie budou napájeny přímo z kitu. 
Kód využívá tlačítka přímo na kitu (USER, RESET), ale je možné použít i externě zapojená tlačítka. 

## Součástky:
* rotační n-kodér - Udává aktuální stav. Při rotaci bude nabývat hodnot 0-100. 
* LED modul WS2812 - Modul obsahuje 8 LED diod. Programujeme je pomocí dátového výstupu MOSI. Pomocí 3 bitů na každé ledce nastavujeme barvu a jas.
   - Modul vizuálně indikuje stav baterie jak barvou, tak počtem rozvícených diod.
* LCD displej HD44780 - K tomuto displeji je přímo knihovna (stm8_hd44780.h, stm8_hd44780.c). Jas displeje je regulovaný pomocí potenciometru 5kΩ, který je připojený na nepájivém poli. 
    # - Zapojení:
        K STM8 připojíme datové piny D4-D7. Piny D0-D3 necháme nezapojené.
        Dále k STM8 připojíme vstupy RS, RW, E.
        Piny zřejmě musíme zapojit přesně tak, jak jsou zapojeny v tomto projektu. Při jiném zapojení mi displej nefungoval.
             ```
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
             ```
        Zapojíme pyny pro nápájení na VDD na +5V a VDD na GND.
        Společnou anodu (A) zapojíme také na +5V a společnou katodu (K) přes ochranný rezistor (použitý 560Ω) na GND.
        Pin V0 slouží pro podsvícení displeje. Tento jas regulujeme pomocí potenciometru, jehož oba konce jsou zapojeny na napájecí svorky a jezdec je připojený na pin V0 (viz. schéma[Contribution guidelines for this project](docs/)).
* Tlačítko USER - je zabudované na kitu s STM8. Při stisku se hodnota stavu baterie skokově sníží o 10% .
* Tlačítko RESET - je zabudované na kitu s STM8. Při stisku se vše vrátí do výchozí pozice, kdy hodnota nabývá 100.

## Popis funkce:
Po startu se výchozí hodnota nastaví na 100. Tato hodnota je ukázána na LCD displeji jako 100%.
Na LED modulu se rozvítí všech 8 diod zelenou barvou.
Na výběr máme 3 možnosti interakce (když nepočítáme RESET tlačítko):
    1. Pomocí rotačního enkodéru můžeme měnit hodnotu 0-100.
    2. Stiskem tlačítka USER, které sníží hodnotu o 10%, maximálně na hodnotu 0.
    3. Při stisku enkodéru se spustí časovač, který každých 10 sekund odebere z hodnoty 10% z maximální hodnoty.
Podle snížené hodnoty se postupně shasínají LED diody  (každých 13% 1 LED dioda), mění se barva (od 50%-15% oranžová, 15%-1% červená).
Hodnota na displeji se mění na danou hodnotu.
Při snížení hodnoty na 0 budou všechny LED diody zhasnuté. 
Při stisku tlačítka USER se rozbliká jedna červená dioda a po chvíli zase zhasne.
Pokud je aktivovaný časovač pomocí n-kodéru, můžeme ho zastavit dalším stisknutím.

## Zhodnocení:

# Dokumenty:
[Contribution guidelines for this project](docs/)


autor: Tomáš Smida