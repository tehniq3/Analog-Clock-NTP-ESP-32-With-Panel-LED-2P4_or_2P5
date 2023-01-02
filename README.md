# Analog-Clock-NTP-ESP-32-With-Panel-LED-2P4_or_2P5
replica of Analog Clock for P3 (64x64) led display for 2xP4 or 2xP5 (64x32) led display

original Analog Clock project used https://github.com/3tawi/RGBmatrixPanelGP library and https://github.com/bmzstore/Analog-Clock-NTP-ESP-32-With-Panel-LED-P3 sketch


now it used library: https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA

#define R1_PIN 25

#define G1_PIN 26

#define B1_PIN 27

#define R2_PIN 14

#define G2_PIN 12

#define B2_PIN 13

#define A_PIN 23

#define B_PIN 19

#define C_PIN 5

#define D_PIN 17

#define E_PIN -1 // required for 1/32 scan panels, like 64x64. Any available pin would do, i.e. IO32

#define LAT_PIN 4

#define OE_PIN 15

#define CLK_PIN 16

![Analog Clock NTP](https://github.com/tehniq3/Analog-Clock-NTP-ESP-32-With-Panel-LED-2P4_or_2P5/blob/main/photo/montaj_AdySos_1m.jpg)
![backside](https://github.com/tehniq3/Analog-Clock-NTP-ESP-32-With-Panel-LED-2P4_or_2P5/blob/main/photo/montaj_AdySos_2m.jpg)
