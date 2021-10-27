//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * Modificaciones y adaptación: Diego Morales
 * IE3027: Electrónica Digital 2 - 2021
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include <avr/pgmspace.h>


// include the SD library:
#include <SPI.h>
#include <SD.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
bool Collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);


const int chipSelect = PA_3; //cs PIN
void MapSD(void);
int ASCII_Hex(int a);
void botones(void);
void flechita(int x, int y, int c);

//***************************************************************************************************************************************
Sd2Card card;     //Variables SD
SdVolume volume;
SdFile root;
uint8_t maps[1000];
//***************************************************************************************************************************************
int estado_boton1 = 0;    //Variables botones
int estado_boton2 = 0;

uint8_t contador = 0;
uint8_t aceptar = 0;
uint8_t aceptar2 = 0;

bool izquierda = false;
bool derecha = false;
bool izquierda2 = false;
bool derecha2 = false;

int recibido = 0;
int Ganador = 0;

int inByte;
int boton;
int bandera = 0;
int bandera2 = 0;
int banderamenu = 0;
//***************************************************************************************************************************************
const int buzzerPin = 40; //Variables musica
int musica = 0;
//***************************************************************************************************************************************
int bandera_musica1 = 1;  //Variables settings
int bandera_musica2 = 0;
int modo_clasico = 1;
int modo_secundario = 0;
//***************************************************************************************************************************************
extern uint8_t NaveGalaga1[];   //Variables para imagenes
extern uint8_t NaveGalaga2[];
extern uint8_t NaveGalaga3[];
extern uint8_t NaveGalaga4[];
extern uint8_t DisparoNave[];
extern uint8_t enemigo1[];   
extern uint8_t enemigo2[];
extern uint8_t enemigo3[];
extern uint8_t explosion1[];

bool collision = false;
bool collision2 = false;

int NaveGalaga1_x = 145; // posicion x
int NaveGalaga1_y = 198; // posicion y
int NaveGalaga1_width = 32; // ancho de bitmap
int NaveGalaga1_height = 32; // altura de bitmap

int NaveGalaga2_x = 225; // posicion x
int NaveGalaga2_y = 198; // posicion y
int NaveGalaga2_width = 32; // ancho de bitmap
int NaveGalaga2_height = 32; // altura de bitmap
//***************************************************************************************************************************************
//PRIMER CONJUNTO DE ENEMIGOS
//***************************************************************************************************************************************
int enemigo_x[15] = {55, 104, 153, 201, 250, 56, 105, 154, 202, 251, 56, 105, 154, 202, 251};
int enemigo_y[15] = {35, 35, 35, 35, 35, 71, 71, 71, 71, 71, 104, 104, 104, 104, 104};
int enemigo_width[15] = {17, 17, 17, 17, 17, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
int enemigo_height[15] = {16, 16, 16, 16, 16, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int enemigo_index[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int enemigo_hit[5] = {0, 0, 0, 0, 0};

int enemigo2_x[15] = {55, 104, 153, 201, 250, 56, 105, 154, 202, 251, 56, 105, 154, 202, 251};
int enemigo2_y[15] = {35, 35, 35, 35, 35, 71, 71, 71, 71, 71, 104, 104, 104, 104, 104};
int enemigo2_width[15] = {17, 17, 17, 17, 17, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};
int enemigo2_height[15] = {16, 16, 16, 16, 16, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int enemigo2_index[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int enemigo2_hit[5] = {0, 0, 0, 0, 0};
//***************************************************************************************************************************************
//SEGUNDO CONJUNTO DE ENEMIGOS
//***************************************************************************************************************************************
int enemigo_x1[6] = {49, 98, 50, 99, 50, 99};
int enemigo_y1[6] = {35, 35, 71, 71, 104, 104};
int enemigo_width1[6] = {17, 17, 15, 15, 15, 15};
int enemigo_height1[6] = {16, 16, 10, 10, 10, 10};
int enemigo_index1[6] = {0, 0, 0, 0, 0, 0};
int enemigo_hit1[2] = {0, 0};

int enemigo2_x1[6] = {49, 98, 50, 99, 50, 99};
int enemigo2_y1[6] = {35, 35, 71, 71, 104, 104};
int enemigo2_width1[6] = {17, 17, 15, 15, 15, 15};
int enemigo2_height1[6] = {16, 16, 10, 10, 10, 10};
int enemigo2_index1[6] = {0, 0, 0, 0, 0, 0};
int enemigo2_hit1[2] = {0, 0};
//***************************************************************************************************************************************
int enemigo_x2[6] = {209, 258, 210, 259, 210, 259};
int enemigo_y2[6] = {35, 35, 71, 71, 104, 104};
int enemigo_width2[6] = {17, 17, 15, 15, 15, 15};
int enemigo_height2[6] = {16, 16, 10, 10, 10, 10};
int enemigo_index2[6] = {0, 0, 0, 0, 0, 0};
int enemigo_hit2[2] = {0, 0};

int enemigo2_x2[6] = {209, 258, 210, 259, 210, 259};
int enemigo2_y2[6] = {35, 35, 71, 71, 104, 104};
int enemigo2_width2[6] = {17, 17, 15, 15, 15, 15};
int enemigo2_height2[6] = {16, 16, 10, 10, 10, 10};
int enemigo2_index2[6] = {0, 0, 0, 0, 0, 0};
int enemigo2_hit2[2] = {0, 0};
//***************************************************************************************************************************************
//VARIABLES PRIMER JUGADOR
//***************************************************************************************************************************************
int contador_enemigos;
bool enemiesMove1 = false;
bool enemiesMove2 = false;
bool enemiesMove3 = false;

bool LimiteNaveIzquierda = false;
bool LimiteNaveDerecha = false;

int bandera_disparo = 0;
int municion = 40;
int municion1 = 15;
String texto_municion;
int disparo_y = 0;
int disparo_x = 145;
bool disparo_completado = true;

int Victoria = 0;

unsigned long previousMillis = 0;
const long interval = 42;
//***************************************************************************************************************************************
//VARIABLES SEGUNDO JUGADOR
//***************************************************************************************************************************************
int contador_enemigos2;
bool enemiesMove1_2 = false;
bool enemiesMove2_2 = false;
bool enemiesMove3_2 = false;

bool LimiteNaveIzquierda_2 = false;
bool LimiteNaveDerecha_2 = false;

int bandera_disparo2 = 0;
int municion2 = 15;
String texto_municion2;
int disparo_y2 = 0;
int disparo_x2 = 145;
bool disparo_completado2 = true;

int Victoria2 = 0;

unsigned long previousMillis2 = 0;
const long interval2 = 42;
//***************************************************************************************************************************************
File myFile;

//***************************************************************************************************************************************
// Initialization
//***************************************************************************************************************************************
void setup() {
  
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Start");

//***************************************************************************************************************************************
// LCD
//***************************************************************************************************************************************  
  LCD_Init();
  LCD_Clear(0x00);
//***************************************************************************************************************************************
// SERIAL PORTS
//***************************************************************************************************************************************
 Serial.begin(9600);
 Serial2.begin(9600);
 Serial5.begin(9600);
//***************************************************************************************************************************************
// GPIO
//***************************************************************************************************************************************
  pinMode(buzzerPin, OUTPUT);
//***************************************************************************************************************************************
// SD
//***************************************************************************************************************************************
  SPI.setModule(0);
  Serial.print("\nInitializing SD card...");
  pinMode(PA_3, OUTPUT);     // change this to 53 on a mega
//********************************************************************************************************
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  Serial.println("Archivos Existentes en MicroSD");
  root.ls(LS_R | LS_DATE | LS_SIZE);
  
  if (!SD.begin(PA_3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}
//***************************************************************************************************************************************
// Loop
//***************************************************************************************************************************************
void loop() {
  if(banderamenu == 0)
  {
    if (bandera_musica1 == 1){
      //Envia 3 a musica
      musica = 3;
      Serial2.write(musica);
    }
    if (bandera_musica2 == 1){
      //Envia 5 a musica
      musica = 5;
      Serial2.write(musica);
    }

    recibido = 1;
    Serial5.write(recibido);
    LCD_Clear(0x00);
    myFile = SD.open("menu1.txt");
    MapSD();
    Serial.println("Imagen leida por completo");
    flechita(110,119,0xffff);
    recibido = 0;
    Serial5.write(recibido);
    
    contador = 1;
    aceptar = 0;
    aceptar2 = 0;
    bandera = 0;
     
    banderamenu = 1;
  }
//*****************************************************************************************************************************
  while (banderamenu == 1){
  
    botones();
     
    if(contador == 1 & bandera == 1)
    {    
      FillRect(110, 126, 8, 50, 0x0000);
      flechita(110,119,0xffff);
      tone(buzzerPin, 523, 190);

      bandera = 0;
    }  
    
    botones();
    
    if(contador == 2 & bandera == 1)
    {
      FillRect(110, 119, 8, 25, 0x0000);
      FillRect(110, 142, 8, 25, 0x0000);
      flechita(110,135,0xffffff);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
    
    botones();
    
    if(contador == 3 & bandera == 1)
    {
      FillRect(110, 119, 8, 50, 0x0000);
      flechita(110,151,0xffff);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
  
    botones();
    
    if(contador == 4)
    {
      contador = 1;
    }
    if(contador < 1)
    {
      contador = 3;
    }

    botones();
    
    if(contador == 1 & aceptar == 1)
    {
      tone(buzzerPin, 659, 190);
      if (bandera_musica1 == 1){
        //Envia 4 a musica
        musica = 4;
        Serial2.write(musica);
      }
      if (bandera_musica2 == 1){
        //Envia 6 a musica
        musica = 6;
        Serial2.write(musica);
      }
      
      recibido = 1;
      Serial5.write(recibido);
      LCD_Clear(0x00);
      FillRect(110, 126, 8, 50, 0x0000);
      myFile = SD.open("BACKGR~1.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      
      String text1 = "MUNICION = 40/40";
      LCD_Print(text1, 170, 10, 1, 0xD0A2, 0x0000);
      
      String text2 = "READY";
      LCD_Print(text2, 124, 112, 2, 0xD0A2, 0x0000);     
      delay(3000);
      FillRect(124, 112, 81, 16, 0x0000); 
      
      for (int i = 0; i<15; i++){
        enemigo_x[i] = enemigo2_x[i];
        enemigo_y[i] = enemigo2_y[i];
        enemigo_width[i] = enemigo2_width[i];
        enemigo_height[i] = enemigo2_height[i];
        enemigo_index[i] = enemigo2_index[i];
      }
      
      for (int i = 0; i<5; i++){
        enemigo_hit[i] = enemigo2_hit[i];
      }
      municion = 40;
      NaveGalaga1_x = 145; // posicion x
      NaveGalaga1_y = 198; // posicion y
      
      LimiteNaveDerecha = false;
      LimiteNaveIzquierda = false;
      
      recibido = 0;
      Serial5.write(recibido);
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0;
      
      banderamenu = 3;
    }
    
    botones();
    
    if(contador == 2 & aceptar == 1)
    {
      tone(buzzerPin, 659, 190);
      if (bandera_musica1 == 1){
        //Envia 4 a musica
        musica = 4;
        Serial2.write(musica);
      }
      if (bandera_musica2 == 1){
        //Envia 6 a musica
        musica = 6;
        Serial2.write(musica);
      }
      
      recibido = 1;
      Serial5.write(recibido);
      LCD_Clear(0x00);
      FillRect(110, 126, 8, 50, 0x0000);
      myFile = SD.open("BACKGR~1.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      
      String text1 = "MUNICION = 15/15";
      LCD_Print(text1, 170, 10, 1, 0xD0A2, 0x0000);
      
      String text2 = "MUNICION = 15/15";
      LCD_Print(text2, 20, 10, 1, 0xD0A2, 0x0000);
      
      String text3 = "READY";
      LCD_Print(text3, 124, 112, 2, 0xD0A2, 0x0000);     
      delay(3000);
      FillRect(124, 112, 81, 16, 0x0000); 
      
      for (int i = 0; i<6; i++){
        enemigo_x1[i] = enemigo2_x1[i];
        enemigo_y1[i] = enemigo2_y1[i];
        enemigo_width1[i] = enemigo2_width1[i];
        enemigo_height1[i] = enemigo2_height1[i];
        enemigo_index1[i] = enemigo2_index1[i];
        
        enemigo_x2[i] = enemigo2_x2[i];
        enemigo_y2[i] = enemigo2_y2[i];
        enemigo_width2[i] = enemigo2_width2[i];
        enemigo_height2[i] = enemigo2_height2[i];
        enemigo_index2[i] = enemigo2_index2[i];
      }
      
      for (int i = 0; i<2; i++){
        enemigo_hit1[i] = enemigo2_hit1[i];
        enemigo_hit2[i] = enemigo2_hit2[i];
      }
      municion1 = 15;
      NaveGalaga1_x = 65; // posicion x
      NaveGalaga1_y = 198; // posicion y
      
      municion2 = 15;
      NaveGalaga2_x = 225; // posicion x
      NaveGalaga2_y = 198; // posicion y
      
      LimiteNaveDerecha = false;
      LimiteNaveIzquierda = false;
      
      LimiteNaveDerecha_2 = false;
      LimiteNaveIzquierda_2 = false;
      
      recibido = 0;
      Serial5.write(recibido);
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0;
      
      banderamenu = 4;
    }
    
    botones();
    
    if(contador == 3 & aceptar == 1)
    {
      tone(buzzerPin, 659, 190);
      //Envia 0 a musica
      musica = 0;
      Serial2.write(musica);
      //********************
      
      recibido = 1;
      Serial5.write(recibido);
      LCD_Clear(0x00);
      FillRect(110, 126, 8, 50, 0x0000);
      myFile = SD.open("settings.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      flechita(20,93,0xffff);
      recibido = 0;
      Serial5.write(recibido);
      
      if (bandera_musica1 == 1){
        //activa bandera musica 1
        Rect(32, 82, 96, 27, 0xFFC0);
        Rect(33, 83, 94, 25, 0xFFC0);
        
        //limpia bandera musica 2
        Rect(192, 82, 96, 27, 0x0000);
        Rect(193, 83, 94, 25, 0x0000);
      }
      if (bandera_musica2 == 1){
        //activa bandera musica 2
        Rect(192, 82, 96, 27, 0xFFC0);
        Rect(193, 83, 94, 25, 0xFFC0);
        
        //limpia bandera musica 1
        Rect(32, 82, 96, 27, 0x0000);
        Rect(33, 83, 94, 25, 0x0000);
      }
      if (modo_clasico == 1){
        //activa bandera modo clasico
        Rect(32, 115, 96, 27, 0xFFC0);
        Rect(33, 116, 94, 25, 0xFFC0);
        
        //limpia bandera modo secundario
        Rect(192, 115, 96, 27, 0x0000);
        Rect(193, 116, 94, 25, 0x0000);
      }
      if (modo_secundario== 1){
        //activa bandera modo secundario
        Rect(192, 115, 96, 27, 0xFFC0);
        Rect(193, 116, 94, 25, 0xFFC0);
        
        //limpia bandera modo clasico
        Rect(32, 115, 96, 27, 0x0000);
        Rect(33, 116, 94, 25, 0x0000);
      }      

      contador = 1;
      aceptar = 0;
      aceptar2 = 0;
      
      banderamenu = 2;
    }
    
    delay(10);
  }
//*****************************************************************************************************************************
  while (banderamenu == 2){
    botones();
      
    if(contador == 1 & bandera == 1)
    {
      flechita(180,93,0x00);
      flechita(20,120,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0x00);
      flechita(20,93,0xffff);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
    
    botones();
    
    if(contador == 2 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(180,93,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0x00);
      flechita(20,120,0xffff);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
    
    botones();

    if(contador == 3 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(20,120,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0x00);
      flechita(180,93,0xffff);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
    
    botones();
    
    if(contador == 4 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(180,93,0x00);
      flechita(20,120,0x00);
      flechita(180,12,0x00);
      flechita(180,120,0xffff);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
    
    botones();
    
    if(contador == 5 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(180,93,0x00);
      flechita(20,120,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0xD0A2);
      tone(buzzerPin, 523, 190);
      
      bandera = 0;
    }
    
    botones();
    
    if(contador == 6)
    {
      contador = 1;
    }
    if(contador < 1)
    {
      contador = 5;
    }

    botones();
    
    if(contador == 1 & aceptar == 1)
    {
      //activa bandera musica 1
      Rect(32, 82, 96, 27, 0xFFC0);
      Rect(33, 83, 94, 25, 0xFFC0);
      
      //limpia bandera musica 2
      Rect(192, 82, 96, 27, 0x0000);
      Rect(193, 83, 94, 25, 0x0000);
      tone(buzzerPin, 659, 190);
      
      bandera_musica1 = 1;
      bandera_musica2 = 0;
      aceptar = 0;
      aceptar2 = 0;
    }
    
    botones();
    
    if(contador == 2 & aceptar == 1)
    {
      //activa bandera modo clasico
      Rect(32, 115, 96, 27, 0xFFC0);
      Rect(33, 116, 94, 25, 0xFFC0);
      
      //limpia bandera modo secundario
      Rect(192, 115, 96, 27, 0x0000);
      Rect(193, 116, 94, 25, 0x0000);
      tone(buzzerPin, 659, 190);
      
      modo_secundario = 0;
      modo_clasico = 1;
      aceptar = 0;
      aceptar2 = 0;
    }
    
    botones();
    
    if(contador == 3 & aceptar == 1)
    {
      //activa bandera musica 2
      Rect(192, 82, 96, 27, 0xFFC0);
      Rect(193, 83, 94, 25, 0xFFC0);
      
      //limpia bandera musica 1
      Rect(32, 82, 96, 27, 0x0000);
      Rect(33, 83, 94, 25, 0x0000);
      tone(buzzerPin, 659, 190);

      bandera_musica2 = 1;
      bandera_musica1 = 0;
      aceptar = 0;
      aceptar2 = 0;
    }
    
    botones();
    
    if(contador == 4 & aceptar == 1)
    {
      //activa bandera modo secundario
      Rect(192, 115, 96, 27, 0xFFC0);
      Rect(193, 116, 94, 25, 0xFFC0);
      
      //limpia bandera modo clasico
      Rect(32, 115, 96, 27, 0x0000);
      Rect(33, 116, 94, 25, 0x0000);
      tone(buzzerPin, 659, 190);

      modo_secundario = 1;
      modo_clasico = 0;
      aceptar = 0;
      aceptar2 = 0;
    }
    
    botones();
    
    if(contador == 5 & aceptar == 1)
    {
      tone(buzzerPin, 659, 190);
      contador = 0;
      aceptar = 0;
      aceptar2 = 0;
      banderamenu = 0;
    }
    
    delay(10);
  }
//*****************************************************************************************************************************  
//*****************************************************************************************************************************
// CORAZON DEL JUEGO
//*****************************************************************************************************************************
//*****************************************************************************************************************************
  while (banderamenu == 3){
  unsigned long currentMillis = millis();
  

  String text1 = "MUNICION =   /40";
  LCD_Print(text1, 170, 10, 1, 0xD0A2, 0x0000);
  texto_municion = String(municion);
  LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);
      
  // actualización de frame cada 42ms = 24fps
  //if (currentMillis - previousMillis >= interval) {
    //previousMillis = currentMillis;
      
    botones();
    
    texto_municion = String(municion);
    LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);
    
    if(izquierda == true & bandera == 1)
    {
      LimiteNaveDerecha = false;
      if (LimiteNaveIzquierda == false) { // movimiento de rectángul
        NaveGalaga1_x-= 4;
        if (NaveGalaga1_x <= 5) {
          LimiteNaveIzquierda = true;
        }
      }    
      FillRect(NaveGalaga1_x + NaveGalaga1_width, NaveGalaga1_y, 4, NaveGalaga1_height, 0x0000);      
      bandera = 0;
      izquierda = false;
    }  
    
    botones();
    
    texto_municion = String(municion);
    LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);
    
    if(derecha == true & bandera == 1)
    {
      LimiteNaveIzquierda = false;
      if (LimiteNaveDerecha == false) {
        NaveGalaga1_x+= 4;
        if (NaveGalaga1_x >= 280) {
          LimiteNaveDerecha = true;
        }
      }
      FillRect(NaveGalaga1_x - 4, NaveGalaga1_y, 4, NaveGalaga1_height, 0x0000);
      bandera = 0;
      derecha = false;
    }
    
    botones();
    
    texto_municion = String(municion);
    LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);
    
    texto_municion = String(municion);
    LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);
    
    if (modo_clasico == 1){
        LCD_Bitmap(NaveGalaga1_x, NaveGalaga1_y, NaveGalaga1_width, NaveGalaga1_height, NaveGalaga1);
      }
    if (modo_secundario== 1){
        LCD_Bitmap(NaveGalaga1_x, NaveGalaga1_y, NaveGalaga1_width, NaveGalaga1_height, NaveGalaga3);
      }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 0; i < 5; i++){
      LCD_Sprite(enemigo_x[i], enemigo_y[i], enemigo_width[i], enemigo_height[i], enemigo1, 5, enemigo_index[i], 0, 0);
    }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 5; i < 10; i++){
      LCD_Sprite(enemigo_x[i], enemigo_y[i], enemigo_width[i], enemigo_height[i], enemigo2, 3, enemigo_index[i], 0, 0);
    }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 10; i < 15; i++){
      LCD_Sprite(enemigo_x[i], enemigo_y[i], enemigo_width[i], enemigo_height[i], enemigo3, 3, enemigo_index[i], 0, 0);
    }
//***********************************************************************************************************************    
    if (enemiesMove1) { // movimiento de enemigos
      for (int i = 0; i < 5; i++){
        FillRect(enemigo_x[i] + enemigo_width[i], enemigo_y[i], 2, enemigo_height[i], 0x0000);
        enemigo_x[i]-= 2;
      }
      if (enemigo_x[0] <= 5) {
        enemiesMove1 = false;
      }
    }  
    else {
      for (int i = 0; i < 5; i++){
        FillRect(enemigo_x[i] - 2, enemigo_y[i], 2, enemigo_height[i], 0x0000);
        enemigo_x[i]+= 2;
      }
      if (enemigo_x[4] >= 288) {
        enemiesMove1 = true;
      }
    }
//------------------------------------------------------------------------------------------------    
    if (enemiesMove2) { // movimiento de enemigos
      for (int i = 5; i < 10; i++){
        FillRect(enemigo_x[i] - 8, enemigo_y[i], 8, enemigo_height[i], 0x0000);
        enemigo_x[i]+= 8;
      }
      if (enemigo_x[9] >= 288) {
        enemiesMove2 = false;
      }
    }  
    else {
      for (int i = 5; i < 10; i++){
        FillRect(enemigo_x[i] + enemigo_width[i], enemigo_y[i], 8, enemigo_height[i], 0x0000);
        enemigo_x[i]-= 8;
      }
      if (enemigo_x[5] <= 5) {
        enemiesMove2 = true;
      }
    }
//------------------------------------------------------------------------------------------------    
    if (enemiesMove3) { // movimiento de enemigos
      for (int i = 10; i < 15; i++){
        FillRect(enemigo_x[i] + enemigo_width[i], enemigo_y[i], 5, enemigo_height[i], 0x0000);
        enemigo_x[i]-= 5;
      }
      if (enemigo_x[10] <= 5) {
        enemiesMove3 = false;
      }
    }  
    else {
      for (int i = 10; i < 15; i++){
        FillRect(enemigo_x[i] - 5, enemigo_y[i], 5, enemigo_height[i], 0x0000);
        enemigo_x[i]+= 5;
      }
      if (enemigo_x[14] >= 288) {
        enemiesMove3 = true;
      }
    }
//***********************************************************************************************************************     
    contador_enemigos+=1;
    
    if (contador_enemigos == 10)
    {
      for (int i = 5; i < 15; i++){
        if (enemigo_index[i] != 2){
          enemigo_index[i]+=1;
          if (enemigo_index[i] > 1){
            enemigo_index[i] = 0;
          }
        }
      }
      for (int i = 0; i < 5; i++){
        if (enemigo_index[i] != 4){
          if (enemigo_hit[i]==0){
            enemigo_index[i] = enemigo_index[i] + 1;
            if (enemigo_index[i] > 1){
              enemigo_index[i] = 0;
            }
          }
          if (enemigo_hit[i]==1){
            enemigo_index[i]+=1;
            if (enemigo_index[i] > 3){
              enemigo_index[i] = 2;
            }
          }
        }
      }
      contador_enemigos = 0;
    }
//***********************************************************************************************************************
// DISPARO
//***********************************************************************************************************************           
    botones();
    
    texto_municion = String(municion);
    LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);

    if(aceptar == 1){
      bandera_disparo = 1;
      aceptar = 0;
    }
    if (bandera_disparo == 1){ 
      if (disparo_completado == true){
        tone(buzzerPin, 659, 90);
        disparo_x = NaveGalaga1_x;
        disparo_completado = false;
      }
      LCD_Bitmap(disparo_x + 14, 190 - disparo_y, 3, 8, DisparoNave);
      FillRect(disparo_x + 14, 190 - disparo_y + 8, 3, 8, 0x0000);
      disparo_y+=8;
      
      if (disparo_y >= 163){
        municion-=1;
        FillRect(disparo_x + 14, 27, 3, 16, 0x0000);
        disparo_y = 8;
        disparo_completado = true;
        bandera_disparo = 0;
      }
    }
//***********************************************************************************************************************
// CONDICION DE COLISION
//***********************************************************************************************************************
    botones();
    
    texto_municion = String(municion);
    LCD_Print(texto_municion, 255, 10, 1, 0xD0A2, 0x0000);
    
    for (int i = 5; i<15; i++){
      if (enemigo_index[i] != 2){
        collision = Collision((disparo_x + 14), (190 - disparo_y), 3, 8, enemigo_x[i], enemigo_y[i], enemigo_width[i], enemigo_height[i]); // detección de colisión
        if (collision) { // se reemplaza el color al colisionar
          enemigo_index[i] = 2;
          
          Victoria+=1;
          municion-=1;
        
          FillRect(disparo_x + 14, (190 - disparo_y), 3, 16, 0x0000);
          LCD_Sprite(enemigo_x[i] - 10, enemigo_y[i] - 11, 32, 32, explosion1, 6, 0, 0, 0);
          LCD_Sprite(enemigo_x[i] - 10, enemigo_y[i] - 11, 32, 32, explosion1, 6, 1, 0, 0);
          LCD_Sprite(enemigo_x[i] - 10, enemigo_y[i] - 11, 32, 32, explosion1, 6, 2, 0, 0);
          LCD_Sprite(enemigo_x[i] - 10, enemigo_y[i] - 11, 32, 32, explosion1, 6, 3, 0, 0);
          LCD_Sprite(enemigo_x[i] - 10, enemigo_y[i] - 11, 32, 32, explosion1, 6, 4, 0, 0);
          LCD_Sprite(enemigo_x[i] - 10, enemigo_y[i] - 11, 32, 32, explosion1, 6, 5, 0, 0);
          
          disparo_y = 8;
          disparo_completado = true;
          bandera_disparo = 0;
        }
      }
    }
    for (int i = 0; i<5; i++){
      if (enemigo_hit[i] == 0){
        if (enemigo_index[i] != 4){
          collision = Collision((disparo_x + 14), (190 - disparo_y), 3, 8, enemigo_x[i], enemigo_y[i], enemigo_width[i], enemigo_height[i]); // detección de colisión
          if (collision) { // se reemplaza el color al colisionar
            enemigo_hit[i] = 1;
            enemigo_index[i] = 2;
            
            municion-=1;
        
            FillRect(disparo_x + 14, (190 - disparo_y), 3, 16, 0x0000);
            disparo_y = 8;
            disparo_completado = true;
            bandera_disparo = 0;
          }
        }
      }
      if (enemigo_hit[i] == 1){
        if (enemigo_index[i] != 4){
          collision = Collision((disparo_x + 14), (190 - disparo_y), 3, 8, enemigo_x[i], enemigo_y[i], enemigo_width[i], enemigo_height[i]); // detección de colisión
          if (collision) { // se reemplaza el color al colisionar
            enemigo_index[i] = 4;
            
            Victoria+=1;            
            municion-=1;
            
            FillRect(disparo_x + 14, (190 - disparo_y), 3, 16, 0x0000);
            LCD_Sprite(enemigo_x[i] - 9, enemigo_y[i] - 8, 32, 32, explosion1, 6, 0, 0, 0);
            LCD_Sprite(enemigo_x[i] - 9, enemigo_y[i] - 8, 32, 32, explosion1, 6, 1, 0, 0);
            LCD_Sprite(enemigo_x[i] - 9, enemigo_y[i] - 8, 32, 32, explosion1, 6, 2, 0, 0);
            LCD_Sprite(enemigo_x[i] - 9, enemigo_y[i] - 8, 32, 32, explosion1, 6, 3, 0, 0);
            LCD_Sprite(enemigo_x[i] - 9, enemigo_y[i] - 8, 32, 32, explosion1, 6, 4, 0, 0);
            LCD_Sprite(enemigo_x[i] - 9, enemigo_y[i] - 8, 32, 32, explosion1, 6, 5, 0, 0);

            
            disparo_y = 8;
            disparo_completado = true;
            bandera_disparo = 0;
          }
        }
      }
    }

//***********************************************************************************************************************
// CONDICION DE DERROTA
//***********************************************************************************************************************
    botones();
    
    if(municion == 0 && Victoria < 15)
    {
      //Envia 1 a musica
      musica = 1;
      Serial2.write(musica);
      //********************
      
      recibido = 1;
      Serial5.write(recibido); 
      LCD_Clear(0x00);
      myFile = SD.open("GAMEOVER.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      recibido = 0;
      Serial5.write(recibido);

      Victoria = 0;
      Ganador = 1;  
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0; 
      
      banderamenu = 5;
    }
    
//***********************************************************************************************************************
// CONDICION DE VICTORIA
//***********************************************************************************************************************    
    botones();
    
    Serial.println(Victoria);
    
    if(Victoria == 15)
    {
      //Envia 2 a musica
      musica = 2;
      Serial2.write(musica);  
      //********************
      
      recibido = 1;
      Serial5.write(recibido);
      LCD_Clear(0x00);
      myFile = SD.open("PLAYER1.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      recibido = 0;
      Serial5.write(recibido);

      Victoria = 0;
      Ganador = 3;
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0; 
      
      banderamenu = 5;
    }
    
    botones();
    
    delay(10);
  //}
}
//*****************************************************************************************************************************  
//*****************************************************************************************************************************
// JUGADOR 1
//*****************************************************************************************************************************
//*****************************************************************************************************************************
while (banderamenu == 4){
  unsigned long currentMillis = millis();
  

  String text1 = "MUNICION =   /15";
  LCD_Print(text1, 20, 10, 1, 0xD0A2, 0x0000);
  texto_municion = String(municion1);
  LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);
      
  // actualización de frame cada 42ms = 24fps
  //if (currentMillis - previousMillis >= interval) {
    //previousMillis = currentMillis;
      
    botones();
    
    texto_municion = String(municion1);
    LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);
    
    if(izquierda == true & bandera == 1)
    {
      LimiteNaveDerecha = false;
      if (LimiteNaveIzquierda == false) { // movimiento de rectángul
        NaveGalaga1_x-= 4;
        if (NaveGalaga1_x <= 5) {
          LimiteNaveIzquierda = true;
        }
      }    
      FillRect(NaveGalaga1_x + NaveGalaga1_width, NaveGalaga1_y, 4, NaveGalaga1_height, 0x0000);      
      bandera = 0;
      izquierda = false;
    }  
    
    botones();
    
    texto_municion = String(municion1);
    LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);
    
    if(derecha == true & bandera == 1)
    {
      LimiteNaveIzquierda = false;
      if (LimiteNaveDerecha == false) {
        NaveGalaga1_x+= 4;
        if (NaveGalaga1_x >= 144) {
          LimiteNaveDerecha = true;
        }
      }
      FillRect(NaveGalaga1_x - 4, NaveGalaga1_y, 4, NaveGalaga1_height, 0x0000);
      bandera = 0;
      derecha = false;
    }
    
    botones();
    
    texto_municion = String(municion1);
    LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);
    
    texto_municion = String(municion1);
    LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);
    
    if (modo_clasico == 1){
        LCD_Bitmap(NaveGalaga1_x, NaveGalaga1_y, NaveGalaga1_width, NaveGalaga1_height, NaveGalaga1);
      }
    if (modo_secundario== 1){
        LCD_Bitmap(NaveGalaga1_x, NaveGalaga1_y, NaveGalaga1_width, NaveGalaga1_height, NaveGalaga3);
      }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 0; i < 2; i++){
      LCD_Sprite(enemigo_x1[i], enemigo_y1[i], enemigo_width1[i], enemigo_height1[i], enemigo1, 5, enemigo_index1[i], 0, 0);
    }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 2; i < 4; i++){
      LCD_Sprite(enemigo_x1[i], enemigo_y1[i], enemigo_width1[i], enemigo_height1[i], enemigo2, 3, enemigo_index1[i], 0, 0);
    }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 4; i < 6; i++){
      LCD_Sprite(enemigo_x1[i], enemigo_y1[i], enemigo_width1[i], enemigo_height1[i], enemigo3, 3, enemigo_index1[i], 0, 0);
    }
//***********************************************************************************************************************    
    if (enemiesMove1) { // movimiento de enemigos
      for (int i = 0; i < 2; i++){
        FillRect(enemigo_x1[i] + enemigo_width1[i], enemigo_y1[i], 2, enemigo_height1[i], 0x0000);
        enemigo_x1[i]-= 2;
      }
      if (enemigo_x1[0] <= 5) {
        enemiesMove1 = false;
      }
    }  
    else {
      for (int i = 0; i < 2; i++){
        FillRect(enemigo_x1[i] - 2, enemigo_y1[i], 2, enemigo_height1[i], 0x0000);
        enemigo_x1[i]+= 2;
      }
      if (enemigo_x1[1] >= 144) {
        enemiesMove1 = true;
      }
    }
//------------------------------------------------------------------------------------------------    
    if (enemiesMove2) { // movimiento de enemigos
      for (int i = 2; i < 4; i++){
        FillRect(enemigo_x1[i] - 8, enemigo_y1[i], 8, enemigo_height1[i], 0x0000);
        enemigo_x1[i]+= 8;
      }
      if (enemigo_x1[3] >= 144) {
        enemiesMove2 = false;
      }
    }  
    else {
      for (int i = 2; i < 4; i++){
        FillRect(enemigo_x1[i] + enemigo_width1[i], enemigo_y1[i], 8, enemigo_height1[i], 0x0000);
        enemigo_x1[i]-= 8;
      }
      if (enemigo_x1[2] <= 5) {
        enemiesMove2 = true;
      }
    }
//------------------------------------------------------------------------------------------------    
    if (enemiesMove3) { // movimiento de enemigos
      for (int i = 4; i < 6; i++){
        FillRect(enemigo_x1[i] + enemigo_width1[i], enemigo_y1[i], 5, enemigo_height1[i], 0x0000);
        enemigo_x1[i]-= 5;
      }
      if (enemigo_x1[4] <= 5) {
        enemiesMove3 = false;
      }
    }  
    else {
      for (int i = 4; i < 6; i++){
        FillRect(enemigo_x1[i] - 5, enemigo_y1[i], 5, enemigo_height1[i], 0x0000);
        enemigo_x1[i]+= 5;
      }
      if (enemigo_x1[5] >= 144) {
        enemiesMove3 = true;
      }
    }
//***********************************************************************************************************************     
    contador_enemigos+=1;
    
    if (contador_enemigos == 10)
    {
      for (int i = 2; i < 6; i++){
        if (enemigo_index1[i] != 2){
          enemigo_index1[i]+=1;
          if (enemigo_index1[i] > 1){
            enemigo_index1[i] = 0;
          }
        }
      }
      for (int i = 0; i < 2; i++){
        if (enemigo_index1[i] != 4){
          if (enemigo_hit1[i]==0){
            enemigo_index1[i] = enemigo_index1[i] + 1;
            if (enemigo_index1[i] > 1){
              enemigo_index1[i] = 0;
            }
          }
          if (enemigo_hit1[i]==1){
            enemigo_index1[i]+=1;
            if (enemigo_index1[i] > 3){
              enemigo_index1[i] = 2;
            }
          }
        }
      }
      contador_enemigos = 0;
    }
//***********************************************************************************************************************
// DISPARO
//***********************************************************************************************************************           
    botones();
    
    texto_municion = String(municion1);
    LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);

    if(aceptar == 1){
      bandera_disparo = 1;
      aceptar = 0;
    }
    if (bandera_disparo == 1){ 
      if (disparo_completado == true){
        tone(buzzerPin, 659, 90);
        disparo_x = NaveGalaga1_x;
        disparo_completado = false;
      }
      LCD_Bitmap(disparo_x + 14, 190 - disparo_y, 3, 8, DisparoNave);
      FillRect(disparo_x + 14, 190 - disparo_y + 8, 3, 8, 0x0000);
      disparo_y+=8;
      
      if (disparo_y >= 163){
        municion1-=1;
        FillRect(disparo_x + 14, 27, 3, 16, 0x0000);
        disparo_y = 8;
        disparo_completado = true;
        bandera_disparo = 0;
      }
    }
//***********************************************************************************************************************
// CONDICION DE COLISION
//***********************************************************************************************************************
    botones();
    
    texto_municion = String(municion1);
    LCD_Print(texto_municion, 105, 10, 1, 0xD0A2, 0x0000);
    
    for (int i = 2; i<6; i++){
      if (enemigo_index1[i] != 2){
        collision = Collision((disparo_x + 14), (190 - disparo_y), 3, 8, enemigo_x1[i], enemigo_y1[i], enemigo_width1[i], enemigo_height1[i]); // detección de colisión
        if (collision) { // se reemplaza el color al colisionar
          enemigo_index1[i] = 2;
          
          Victoria+=1;
          municion1-=1;
        
          FillRect(disparo_x + 14, (190 - disparo_y), 3, 16, 0x0000);
          LCD_Sprite(enemigo_x1[i] - 10, enemigo_y1[i] - 11, 32, 32, explosion1, 6, 0, 0, 0);
          LCD_Sprite(enemigo_x1[i] - 10, enemigo_y1[i] - 11, 32, 32, explosion1, 6, 1, 0, 0);
          LCD_Sprite(enemigo_x1[i] - 10, enemigo_y1[i] - 11, 32, 32, explosion1, 6, 2, 0, 0);
          LCD_Sprite(enemigo_x1[i] - 10, enemigo_y1[i] - 11, 32, 32, explosion1, 6, 3, 0, 0);
          LCD_Sprite(enemigo_x1[i] - 10, enemigo_y1[i] - 11, 32, 32, explosion1, 6, 4, 0, 0);
          LCD_Sprite(enemigo_x1[i] - 10, enemigo_y1[i] - 11, 32, 32, explosion1, 6, 5, 0, 0);
          
          disparo_y = 8;
          disparo_completado = true;
          bandera_disparo = 0;
        }
      }
    }
    for (int i = 0; i<2; i++){
      if (enemigo_hit1[i] == 0){
        if (enemigo_index1[i] != 4){
          collision = Collision((disparo_x + 14), (190 - disparo_y), 3, 8, enemigo_x1[i], enemigo_y1[i], enemigo_width1[i], enemigo_height1[i]); // detección de colisión
          if (collision) { // se reemplaza el color al colisionar
            enemigo_hit1[i] = 1;
            enemigo_index1[i] = 2;
            
            municion1-=1;
        
            FillRect(disparo_x + 14, (190 - disparo_y), 3, 16, 0x0000);
            disparo_y = 8;
            disparo_completado = true;
            bandera_disparo = 0;
          }
        }
      }
      if (enemigo_hit1[i] == 1){
        if (enemigo_index1[i] != 4){
          collision = Collision((disparo_x + 14), (190 - disparo_y), 3, 8, enemigo_x1[i], enemigo_y1[i], enemigo_width1[i], enemigo_height1[i]); // detección de colisión
          if (collision) { // se reemplaza el color al colisionar
            enemigo_index1[i] = 4;

            Victoria+=1;            
            municion1-=1;
            
            FillRect(disparo_x + 14, (190 - disparo_y), 3, 16, 0x0000);
            LCD_Sprite(enemigo_x1[i] - 9, enemigo_y1[i] - 8, 32, 32, explosion1, 6, 0, 0, 0);
            LCD_Sprite(enemigo_x1[i] - 9, enemigo_y1[i] - 8, 32, 32, explosion1, 6, 1, 0, 0);
            LCD_Sprite(enemigo_x1[i] - 9, enemigo_y1[i] - 8, 32, 32, explosion1, 6, 2, 0, 0);
            LCD_Sprite(enemigo_x1[i] - 9, enemigo_y1[i] - 8, 32, 32, explosion1, 6, 3, 0, 0);
            LCD_Sprite(enemigo_x1[i] - 9, enemigo_y1[i] - 8, 32, 32, explosion1, 6, 4, 0, 0);
            LCD_Sprite(enemigo_x1[i] - 9, enemigo_y1[i] - 8, 32, 32, explosion1, 6, 5, 0, 0);

            
            disparo_y = 8;
            disparo_completado = true;
            bandera_disparo = 0;
          }
        }
      }
    }

//***********************************************************************************************************************
//***********************************************************************************************************************
// JUGADOR 2
//***********************************************************************************************************************
//***********************************************************************************************************************
    String text2 = "MUNICION =   /15";
    LCD_Print(text2, 170, 10, 1, 0xD0A2, 0x0000);
    texto_municion2 = String(municion2);
    LCD_Print(texto_municion2, 255, 10, 1, 0xD0A2, 0x0000);
      
    botones();
    
    texto_municion2 = String(municion2);
    LCD_Print(texto_municion2, 255, 10, 1, 0xD0A2, 0x0000);
    
    if(izquierda2 == true & bandera2 == 1)
    {
      LimiteNaveDerecha_2 = false;
      if (LimiteNaveIzquierda_2 == false) { // movimiento de rectángul
        NaveGalaga2_x-= 4;
        if (NaveGalaga2_x <= 176) {
          LimiteNaveIzquierda_2 = true;
        }
      }    
      FillRect(NaveGalaga2_x + NaveGalaga2_width, NaveGalaga2_y, 4, NaveGalaga2_height, 0x0000);      
      bandera2 = 0;
      izquierda2 = false;
    }  
    
    botones();
    
    texto_municion2 = String(municion2);
    LCD_Print(texto_municion2, 255, 10, 1, 0xD0A2, 0x0000);

    if(derecha2 == true & bandera2 == 1)
    {
      LimiteNaveIzquierda_2 = false;
      if (LimiteNaveDerecha_2 == false) {
        NaveGalaga2_x+= 4;
        if (NaveGalaga2_x >= 280) {
          LimiteNaveDerecha_2 = true;
        }
      }
      FillRect(NaveGalaga2_x - 4, NaveGalaga2_y, 4, NaveGalaga2_height, 0x0000);
      bandera2 = 0;
      derecha2 = false;
    }
    
    botones();
    
    texto_municion2 = String(municion2);
    LCD_Print(texto_municion2, 255, 10, 1, 0xD0A2, 0x0000);
    
    if (modo_clasico == 1){
        LCD_Bitmap(NaveGalaga2_x, NaveGalaga2_y, NaveGalaga2_width, NaveGalaga2_height, NaveGalaga2);
      }
    if (modo_secundario== 1){
        LCD_Bitmap(NaveGalaga2_x, NaveGalaga2_y, NaveGalaga2_width, NaveGalaga2_height, NaveGalaga4);
      }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 0; i < 2; i++){
      LCD_Sprite(enemigo_x2[i], enemigo_y2[i], enemigo_width2[i], enemigo_height2[i], enemigo1, 5, enemigo_index2[i], 0, 0);
    }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 2; i < 4; i++){
      LCD_Sprite(enemigo_x2[i], enemigo_y2[i], enemigo_width2[i], enemigo_height2[i], enemigo2, 3, enemigo_index2[i], 0, 0);
    }
//**********************************************PRIMER SET DE ENEMIGOS***************************************************
    for (int i = 4; i < 6; i++){
      LCD_Sprite(enemigo_x2[i], enemigo_y2[i], enemigo_width2[i], enemigo_height2[i], enemigo3, 3, enemigo_index2[i], 0, 0);
    }
//***********************************************************************************************************************    
    if (enemiesMove1_2) { // movimiento de enemigos
      for (int i = 0; i < 2; i++){
        FillRect(enemigo_x2[i] + enemigo_width2[i], enemigo_y2[i], 2, enemigo_height2[i], 0x0000);
        enemigo_x2[i]-= 2;
      }
      if (enemigo_x2[0] <= 176) {
        enemiesMove1_2 = false;
      }
    }  
    else {
      for (int i = 0; i < 2; i++){
        FillRect(enemigo_x2[i] - 2, enemigo_y2[i], 2, enemigo_height2[i], 0x0000);
        enemigo_x2[i]+= 2;
      }
      if (enemigo_x2[1] >= 288) {
        enemiesMove1_2 = true;
      }
    }
//------------------------------------------------------------------------------------------------    
    if (enemiesMove2_2) { // movimiento de enemigos
      for (int i = 2; i < 4; i++){
        FillRect(enemigo_x2[i] - 8, enemigo_y2[i], 8, enemigo_height2[i], 0x0000);
        enemigo_x2[i]+= 8;
      }
      if (enemigo_x2[3] >= 288) {
        enemiesMove2_2 = false;
      }
    }  
    else {
      for (int i = 2; i < 4; i++){
        FillRect(enemigo_x2[i] + enemigo_width2[i], enemigo_y2[i], 8, enemigo_height2[i], 0x0000);
        enemigo_x2[i]-= 8;
      }
      if (enemigo_x2[2] <= 176) {
        enemiesMove2_2 = true;
      }
    }
//------------------------------------------------------------------------------------------------    
    if (enemiesMove3_2) { // movimiento de enemigos
      for (int i = 4; i < 6; i++){
        FillRect(enemigo_x2[i] + enemigo_width2[i], enemigo_y2[i], 5, enemigo_height2[i], 0x0000);
        enemigo_x2[i]-= 5;
      }
      if (enemigo_x2[4] <= 176) {
        enemiesMove3_2 = false;
      }
    }  
    else {
      for (int i = 4; i < 6; i++){
        FillRect(enemigo_x2[i] - 5, enemigo_y2[i], 5, enemigo_height2[i], 0x0000);
        enemigo_x2[i]+= 5;
      }
      if (enemigo_x2[5] >= 288) {
        enemiesMove3_2 = true;
      }
    }
//***********************************************************************************************************************     
    contador_enemigos2+=1;
    
    if (contador_enemigos2 == 10)
    {
      for (int i = 2; i < 6; i++){
        if (enemigo_index2[i] != 2){
          enemigo_index2[i]+=1;
          if (enemigo_index2[i] > 1){
            enemigo_index2[i] = 0;
          }
        }
      }
      for (int i = 0; i < 2; i++){
        if (enemigo_index2[i] != 4){
          if (enemigo_hit2[i]==0){
            enemigo_index2[i] = enemigo_index2[i] + 1;
            if (enemigo_index2[i] > 1){
              enemigo_index2[i] = 0;
            }
          }
          if (enemigo_hit2[i]==1){
            enemigo_index2[i]+=1;
            if (enemigo_index2[i] > 3){
              enemigo_index2[i] = 2;
            }
          }
        }
      }
      contador_enemigos2 = 0;
    }
//***********************************************************************************************************************
// DISPARO
//***********************************************************************************************************************           
    botones();
    
    texto_municion2 = String(municion2);
    LCD_Print(texto_municion2, 255, 10, 1, 0xD0A2, 0x0000);

    if(aceptar2 == 1){
      bandera_disparo2 = 1;
      aceptar2 = 0;
    }
    if (bandera_disparo2 == 1){ 
      if (disparo_completado2 == true){
        tone(buzzerPin, 659, 90);
        disparo_x2 = NaveGalaga2_x;
        disparo_completado2 = false;
      }
      LCD_Bitmap(disparo_x2 + 14, 190 - disparo_y2, 3, 8, DisparoNave);
      FillRect(disparo_x2 + 14, 190 - disparo_y2 + 8, 3, 8, 0x0000);
      disparo_y2+=8;
      
      if (disparo_y2 >= 163){
        municion2-=1;
        FillRect(disparo_x2 + 14, 27, 3, 16, 0x0000);
        disparo_y2 = 8;
        disparo_completado2 = true;
        bandera_disparo2 = 0;
      }
    }
//***********************************************************************************************************************
// CONDICION DE COLISION
//***********************************************************************************************************************
    botones();
    
    texto_municion2 = String(municion2);
    LCD_Print(texto_municion2, 255, 10, 1, 0xD0A2, 0x0000);
    
    for (int i = 2; i<6; i++){
      if (enemigo_index2[i] != 2){
        collision2 = Collision((disparo_x2 + 14), (190 - disparo_y2), 3, 8, enemigo_x2[i], enemigo_y2[i], enemigo_width2[i], enemigo_height2[i]); // detección de colisión
        if (collision2) { // se reemplaza el color al colisionar
          enemigo_index2[i] = 2;
          
          Victoria2+=1;
          municion2-=1;
        
          FillRect(disparo_x2 + 14, (190 - disparo_y2), 3, 16, 0x0000);
          LCD_Sprite(enemigo_x2[i] - 10, enemigo_y2[i] - 11, 32, 32, explosion1, 6, 0, 0, 0);
          LCD_Sprite(enemigo_x2[i] - 10, enemigo_y2[i] - 11, 32, 32, explosion1, 6, 1, 0, 0);
          LCD_Sprite(enemigo_x2[i] - 10, enemigo_y2[i] - 11, 32, 32, explosion1, 6, 2, 0, 0);
          LCD_Sprite(enemigo_x2[i] - 10, enemigo_y2[i] - 11, 32, 32, explosion1, 6, 3, 0, 0);
          LCD_Sprite(enemigo_x2[i] - 10, enemigo_y2[i] - 11, 32, 32, explosion1, 6, 4, 0, 0);
          LCD_Sprite(enemigo_x2[i] - 10, enemigo_y2[i] - 11, 32, 32, explosion1, 6, 5, 0, 0);
          
          disparo_y2 = 8;
          disparo_completado2 = true;
          bandera_disparo2 = 0;
        }
      }
    }
    for (int i = 0; i<2; i++){
      if (enemigo_hit2[i] == 0){
        if (enemigo_index2[i] != 4){
          collision2 = Collision((disparo_x2 + 14), (190 - disparo_y2), 3, 8, enemigo_x2[i], enemigo_y2[i], enemigo_width2[i], enemigo_height2[i]); // detección de colisión
          if (collision2) { // se reemplaza el color al colisionar
            enemigo_hit2[i] = 1;
            enemigo_index2[i] = 2;
            
            municion2-=1;
        
            FillRect(disparo_x2 + 14, (190 - disparo_y2), 3, 16, 0x0000);
            disparo_y2 = 8;
            disparo_completado2 = true;
            bandera_disparo2 = 0;
          }
        }
      }
      if (enemigo_hit2[i] == 1){
        if (enemigo_index2[i] != 4){
          collision2 = Collision((disparo_x2 + 14), (190 - disparo_y2), 3, 8, enemigo_x2[i], enemigo_y2[i], enemigo_width2[i], enemigo_height2[i]); // detección de colisión
          if (collision2) { // se reemplaza el color al colisionar
            enemigo_index2[i] = 4;

            Victoria2+=1;            
            municion2-=1;
            
            FillRect(disparo_x2 + 14, (190 - disparo_y2), 3, 16, 0x0000);
            LCD_Sprite(enemigo_x2[i] - 9, enemigo_y2[i] - 8, 32, 32, explosion1, 6, 0, 0, 0);
            LCD_Sprite(enemigo_x2[i] - 9, enemigo_y2[i] - 8, 32, 32, explosion1, 6, 1, 0, 0);
            LCD_Sprite(enemigo_x2[i] - 9, enemigo_y2[i] - 8, 32, 32, explosion1, 6, 2, 0, 0);
            LCD_Sprite(enemigo_x2[i] - 9, enemigo_y2[i] - 8, 32, 32, explosion1, 6, 3, 0, 0);
            LCD_Sprite(enemigo_x2[i] - 9, enemigo_y2[i] - 8, 32, 32, explosion1, 6, 4, 0, 0);
            LCD_Sprite(enemigo_x2[i] - 9, enemigo_y2[i] - 8, 32, 32, explosion1, 6, 5, 0, 0);

            
            disparo_y2 = 8;
            disparo_completado2 = true;
            bandera_disparo2 = 0;
          }
        }
      }
    }

//***********************************************************************************************************************
// CONDICION DE DERROTA
//***********************************************************************************************************************
    botones();
    
    if (municion1 < 1){
      municion1 = 0;
      recibido = 3;
      Serial5.write(recibido); 
    }
    
    if (municion2 < 1){
      municion2 = 0;
      recibido = 2;
      Serial5.write(recibido); 
    }
    
    if((municion1 == 0 && Victoria < 6)&&(municion2 == 0 && Victoria2 < 6))
    {
      //Envia 1 a musica
      musica = 1;
      Serial2.write(musica);
      //********************
      
      recibido = 1;
      Serial5.write(recibido); 
      LCD_Clear(0x00);
      myFile = SD.open("GAMEOVER.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      recibido = 0;
      Serial5.write(recibido);

      Victoria = 0;
      Victoria2 = 0;
      
      Ganador = 0;
      
      recibido = 5;
      Serial5.write(recibido); 

      delay(10);
      
      recibido = 4;
      Serial5.write(recibido); 
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0;  
      
      banderamenu = 5;
    }

//***********************************************************************************************************************
// CONDICION DE VICTORIA 1
//***********************************************************************************************************************    
    botones();
    
    if(Victoria == 6)
    {
      //Envia 2 a musica
      musica = 2;
      Serial2.write(musica);  
      //********************
      
      recibido = 1;
      Serial5.write(recibido);
      LCD_Clear(0x00);
      myFile = SD.open("PLAYER1.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      recibido = 0;
      Serial5.write(recibido);

      Victoria = 0;
      Victoria2 = 0;
      
      recibido = 4;
      Serial5.write(recibido);
      
      recibido = 5;
      Serial5.write(recibido);
      Ganador = 1; 
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0;
      
      banderamenu = 5;
    }
    
    botones();
    
    delay(10);
        
//***********************************************************************************************************************
// CONDICION DE VICTORIA 2
//***********************************************************************************************************************    
    botones();
    
    if(Victoria2 == 6)
    {
      //Envia 2 a musica
      musica = 2;
      Serial2.write(musica);  
      //********************
      
      recibido = 1;
      Serial5.write(recibido);
      LCD_Clear(0x00);
      myFile = SD.open("PLAYER2.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      recibido = 0;
      Serial5.write(recibido);

      Victoria = 0;
      Victoria2 = 0;
      
      recibido = 4;
      Serial5.write(recibido);
      
      recibido = 5;
      Serial5.write(recibido);
       
      Ganador = 2;
      
      contador = 1;
      aceptar = 0;
      aceptar2 = 0;
      
      banderamenu = 5;
    }
    
    botones();
    
    delay(10);
  //}
 }
//*****************************************************************************************************************************
//*****************************************************************************************************************************
//*****************************************************************************************************************************
while (banderamenu == 5){
    
    botones();
    
    if((aceptar == 1 | aceptar2 == 1) & Ganador == 0)
    {
      Ganador = 0;
      aceptar = 0;
      aceptar2 = 0;
      banderamenu = 0;
    }
    
    botones();
  
    if(aceptar == 1 & Ganador == 1)
    {
      Ganador = 0;
      aceptar = 0;
      aceptar2 = 0;
      banderamenu = 0;
    }
    
    botones();
    
    if(aceptar2 == 1 & Ganador == 2)
    {
      Ganador = 0;
      aceptar = 0;
      aceptar2 = 0;
      banderamenu = 0;
    }
    
    botones();
    
    if(aceptar == 1 & Ganador == 3)
    {
      Ganador = 0;
      aceptar = 0;
      aceptar2 = 0;
      banderamenu = 0;
    }
    
  }
//*****************************************************************************************************************************
}
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c); 
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
    for (int j = 0; j < height; j++){
        k = (j*(ancho) + index*width -1 - offset)*2;
        k = k+width*2;
       for (int i = 0; i < width; i++){
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k+1]);
        k = k - 2;
       } 
    }
  }
  else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para detectar choque de objetos
//***************************************************************************************************************************************
bool Collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
  return (x1 < x2 + w2) && (x1+ w1 > x2) && (y1 < y2 + h2) && (y1 + h1 > y2);
}
//***************************************************************************************************************************************
// Función para mapear archivos de la SD y dibujarlos linea por linea
//***************************************************************************************************************************************
void MapSD(void){
  int hex1 = 0;
  int val1 = 0; 
  int val2 = 0;
  int mapar = 0;
  int vert = 0;
  if(myFile)
  {
    Serial.println("Leyendo el archivo...");
      while(myFile.available())
      {
        mapar=0;
        while(mapar<640)
        {
          hex1 = myFile.read();
          if(hex1 == 120)
          {
            val1 = myFile.read();
            val2 = myFile.read();
            val1 = ASCII_Hex(val1);
            val2 = ASCII_Hex(val2);
            maps[mapar] = val1*16+val2;
            mapar++;
          }
        }
        LCD_Bitmap(0, vert, 320, 1, maps);
        vert++;
      }
      myFile.close();
  }
  else{
    Serial.println("No se pudo leer el archivo");
    myFile.close();
  }
}
//***************************************************************************************************************************************
// Función para convertir un ascii a su valor en hex
//***************************************************************************************************************************************
int ASCII_Hex(int a){
  switch(a){
    case 48:
      return 0;
    case 49:
      return 1;
    case 50:
      return 2;
    case 51:
      return 3;
    case 52:
      return 4;
    case 53:
      return 5;
    case 54:
      return 6;
    case 55:
      return 7;
    case 56:
      return 8;
    case 57:
      return 9;
    case 97:
      return 10;
    case 98:
      return 11;
    case 99:
      return 12;
    case 100:
      return 13;
    case 101:
      return 14;
    case 102:
      return 15;
  }
}
//***************************************************************************************************************************************
// Función para colocar una flechita en la posicion indicada
//***************************************************************************************************************************************
void flechita(int x, int y, int c)
{
      delay(5);
      V_line(x, y, 7, c);
      delay(5);
      V_line(x+1, y, 7, c);
      delay(5);
      V_line(x+2, y+1, 5, c);
      delay(5);
      V_line(x+3, y+1, 5, c);
      delay(5);
      V_line(x+4, y+2, 3, c);
      delay(5);
      V_line(x+5, y+2, 3, c);
      delay(5);
      V_line(x+6, y+3, 1, c);
      delay(5);
      V_line(x+7, y+3, 1, c);
      delay(5);
}
//***************************************************************************************************************************************
// Función para manejar botones PUSH1 y PUSH2 de la TIVAC
//***************************************************************************************************************************************
void botones(void)
{
  
  recibido = 0;
  Serial5.write(recibido);
  
  if (Serial5.available())
  {
    inByte = Serial5.read();
  }

  if (inByte == 0)
  {
    boton = 0;
  }
  
  if (inByte == 1)
  {
    boton = 1;
  }
  
  if(boton == 1)
  {
    contador++;
    boton = 0;
    bandera = 1;
  }
  
  if (inByte == 2)
  {
    boton = 2;
  }
  
  if(boton == 2)
  {
    contador--;
    boton = 0;
    bandera = 1;
  }

  if (inByte == 3)
  {
    boton = 3;
  }

  if(boton == 3)
  {
    aceptar = 1;
    boton = 0;
  }
  
  if (inByte == 4)
  {
    boton = 4;
  }
  
  if(boton == 4)
  {
    boton = 0;
    bandera2 = 1;
  }
  
  if(inByte == 21)
  {
    boton = 21;
  }
  
  if(boton == 5)
  {
    boton = 0;
    bandera2 = 1;
  }

  if(inByte == 6)
  {
    boton = 6;
  }

  if(boton == 6)
  {
    aceptar2 = 1;
    boton = 0;
  }

  if(inByte == 7 & (banderamenu == 3 | banderamenu == 4))
  {
    boton = 7;
  }

  if(boton == 7 & (banderamenu == 3 | banderamenu == 4))
  {
    izquierda = true;
    bandera = 1;
    boton = 0;
  }

  if(inByte == 8 & (banderamenu == 3 | banderamenu == 4))
  {
    boton = 8;
  }

  if(boton == 8 & (banderamenu == 3 | banderamenu == 4))
  {
    derecha = true;
    bandera = 1;
    boton = 0;
  }

  if(inByte == 9 & banderamenu == 4)
  {
    boton = 9;
  }

  if(boton == 9 & banderamenu == 4)
  {
    izquierda2 = true;
    bandera2 = 1; 
    boton = 0;
  }

  if(inByte == 5 & banderamenu == 4)
  {
    boton = 5;
  }
  
  if(boton == 5 & banderamenu == 4)
  {
    derecha2 = true;
    bandera2 = 1; 
    boton = 0;
  }
  
  recibido = 1;
  inByte = 0;
  Serial5.write(recibido);
}
