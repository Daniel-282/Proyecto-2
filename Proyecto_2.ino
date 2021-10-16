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

int bandera = 0;
int bandera0 = 0;
int bandera1 = 0;
int banderamenu = 0;
const int boton1 = PUSH1;  //PUSH1
const int boton2 = PF_0;  //PUSH2
//***************************************************************************************************************************************
int bandera_musica1 = 1;  //Variables settings
int bandera_musica2 = 0;
int modo_clasico = 1;
int modo_secundario = 0;
//***************************************************************************************************************************************
File myFile;

//***************************************************************************************************************************************
// Initialization
//***************************************************************************************************************************************
void setup() {
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Start");

//***************************************************************************************************************************************
// LCD
//***************************************************************************************************************************************  
  LCD_Init();
  LCD_Clear(0x00);

  //FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c)
  //FillRect(80, 60, 160, 120, 0x0400);

  //LCD_Print(String text, int x, int y, int fontSize, int color, int background)
  //String text1 = "IE3027";
  //LCD_Print(text1, 110, 110, 2, 0xffff, 0x0000);
  
  //delay(1000);
    
  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  //LCD_Bitmap(0, 0, 320, 240, uvg);
  
  /*for(int x = 0; x <319; x++){
    LCD_Bitmap(x, 52, 16, 16, tile2);
    LCD_Bitmap(x, 68, 16, 16, tile);
    LCD_Bitmap(x, 207, 16, 16, tile);
    LCD_Bitmap(x, 223, 16, 16, tile);
    x += 15;
 }*/
//***************************************************************************************************************************************
 Serial.begin(9600);
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
  
//********************************************************************************************************  
}
//***************************************************************************************************************************************
// Loop
//***************************************************************************************************************************************
void loop() {
    
  if(banderamenu == 0)
  {
    //delay(50);
    myFile = SD.open("menu1.txt");
    MapSD();
    Serial.println("Imagen leida por completo");
    banderamenu = 1;
  }
//*****************************************************************************************************************************
  while (banderamenu == 1){
  
    botones();
    
    if((contador == 0 | contador == 1) & bandera == 1)
    {    
      FillRect(110, 126, 8, 50, 0x0000);
      flechita(110,119,0xffff);
      Serial.println(contador);
      bandera = 0;
    }  
    
    delay(10);
    
    if(contador == 2 & bandera == 1)
    {
      FillRect(110, 119, 8, 25, 0x0000);
      FillRect(110, 142, 8, 25, 0x0000);
      flechita(110,135,0xffffff);
      
      Serial.println(contador);
      bandera = 0;
    }
    
    delay(10);
    
    if(contador == 3 & bandera == 1)
    {
      FillRect(110, 119, 8, 50, 0x0000);
      flechita(110,151,0xffff);
      
      Serial.println(contador);
      bandera = 0;
    }
  
    delay(10);
    
    if(contador == 4)
    {
      contador = 0;
    }
    if(contador == 255)
    {
      contador = 4;
    }

    delay(10);
    
    if(contador == 3 & aceptar == 1)
    {
      LCD_Clear(0x00);
      FillRect(110, 126, 8, 50, 0x0000);
      myFile = SD.open("settings.txt");
      MapSD();
      Serial.println("Imagen leida por completo");
      contador = 0;
      aceptar = 0;
      
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
      
      Serial.println(contador);
      bandera = 0;
    }
    
    delay(10);
    
    if(contador == 2 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(180,93,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0x00);
      flechita(20,120,0xffff);
      
      Serial.println(contador);
      bandera = 0;
    }
    
    delay(10);

    if(contador == 3 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(20,120,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0x00);
      flechita(180,93,0xffff);
      
      Serial.println(contador);
      bandera = 0;
    }
    
    delay(10);
    
    if(contador == 4 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(180,93,0x00);
      flechita(20,120,0x00);
      flechita(180,12,0x00);
      flechita(180,120,0xffff);
      
      Serial.println(contador);
      bandera = 0;
    }
    
    delay(10);
    
    if(contador == 5 & bandera == 1)
    {
      flechita(20,93,0x00);
      flechita(180,93,0x00);
      flechita(20,120,0x00);
      flechita(180,120,0x00);
      flechita(180,12,0xD0A2);
      
      Serial.println(contador);
      bandera = 0;
    }
    
    delay(10);
    
    if(contador == 6)
    {
      contador = 0;
    }
    if(contador == 255)
    {
      contador = 6;
    }

    delay(10);
    
    if(contador == 1 & aceptar == 1)
    {
      //activa bandera musica 1
      Rect(32, 82, 96, 27, 0xFFC0);
      Rect(33, 83, 94, 25, 0xFFC0);
      
      //limpia bandera musica 2
      Rect(192, 82, 96, 27, 0x0000);
      Rect(193, 83, 94, 25, 0x0000);
      
      bandera_musica1 = 1;
      bandera_musica2 = 0;
      aceptar = 0;
    }
    
    delay(10);
    
    if(contador == 2 & aceptar == 1)
    {
      //activa bandera modo clasico
      Rect(32, 115, 96, 27, 0xFFC0);
      Rect(33, 116, 94, 25, 0xFFC0);
      
      //limpia bandera modo secundario
      Rect(192, 115, 96, 27, 0x0000);
      Rect(193, 116, 94, 25, 0x0000);
      
      modo_secundario = 0;
      modo_clasico = 1;
      aceptar = 0;
    }
    
    delay(10);
    
    if(contador == 3 & aceptar == 1)
    {
      //activa bandera musica 2
      Rect(192, 82, 96, 27, 0xFFC0);
      Rect(193, 83, 94, 25, 0xFFC0);
      
      //limpia bandera musica 1
      Rect(32, 82, 96, 27, 0x0000);
      Rect(33, 83, 94, 25, 0x0000);

      bandera_musica2 = 1;
      bandera_musica1 = 0;
      aceptar = 0;
    }
    
    delay(10);
    
    if(contador == 4 & aceptar == 1)
    {
      //activa bandera modo secundario
      Rect(192, 115, 96, 27, 0xFFC0);
      Rect(193, 116, 94, 25, 0xFFC0);
      
      //limpia bandera modo clasico
      Rect(32, 115, 96, 27, 0x0000);
      Rect(33, 116, 94, 25, 0x0000);

      modo_secundario = 1;
      modo_clasico = 0;
      aceptar = 0;
    }
    
    delay(10);
    
    if(contador == 5 & aceptar == 1)
    {
      contador = 0;
      aceptar = 0;
      banderamenu = 0;
    }
    
    delay(10);
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
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
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
  estado_boton1 = digitalRead(boton1);
  estado_boton2 = digitalRead(boton2);
  
  if(estado_boton1 == HIGH)
  {
    bandera0 = 1;
  }
  else
  {
    if(bandera0 == 1 & estado_boton1 == LOW)
    {
      contador++;
      bandera0 = 0;
      bandera = 1;
    }
  }
  if(estado_boton2 == HIGH)
  {
    bandera1 = 1;
  }
  else
  {
    if(bandera1 == 1 & estado_boton2 == LOW)
    {
      aceptar = 1;
      bandera1 = 0;
      //bandera = 1;
    }
  }
}