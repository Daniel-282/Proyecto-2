#include <stdint.h>
#include <stdbool.h>

void emergencia(void);

const int player1_button1 = 4;
const int player1_button3 = 5;
const int player1_button2 = 6;

const int player2_button1 = 8;
const int player2_button2 = 9;
const int player2_button3 = 10;

int estado_boton1 = 0;
int estado_boton2 = 0;
int estado_boton3 = 0;
int estado_boton4 = 0;
int estado_boton5 = 0;
int estado_boton6 = 0;

int bandera0 = 0;
int bandera1 = 0;
int bandera2 = 0;
int bandera3 = 0;
int bandera4 = 0;
int bandera5 = 0;
int bandera6 = 0;

int inByte;
int instruccion;
int bloqueo = 0;

/*
int UART1 = 0;
int UART2 = 0;
int UART3 = 0;
int UART4 = 0;
int UART5 = 0;
int UART6 = 0;

char mystr[6];*/

void setup() {
  Serial.begin(9600);
  
  pinMode(player1_button1, INPUT);
  pinMode(player1_button2, INPUT);
  pinMode(player1_button3, INPUT);
  
  pinMode(player2_button1, INPUT);
  pinMode(player2_button2, INPUT);
  pinMode(player2_button3, INPUT);
}

void loop() 
{ 
  emergencia();
//****************************************************************************      
  estado_boton1 = digitalRead(player1_button1);
  estado_boton2 = digitalRead(player1_button2);
  estado_boton3 = digitalRead(player1_button3);
  estado_boton4 = digitalRead(player2_button1);
  estado_boton5 = digitalRead(player2_button2);
  estado_boton6 = digitalRead(player2_button3);
//****************************************************************************
  if (bloqueo == 0){
    if(estado_boton1 == HIGH & estado_boton2 == LOW & estado_boton3 == LOW)
    {
      bandera1 = 1;
      delay(10);
      inByte = 7;
      Serial.write(inByte);
    }
    else
    {
      if(bandera1 == 1 & estado_boton1 == LOW)
      {
        Serial.println("1");
        inByte = 1;
        Serial.write(inByte);
        /*UART1 = 1;
        UART2 = 0;
        UART3 = 0;*/
        
        bandera1 = 0;
      }
    }
  }
//****************************************************************************
  
  emergencia();
  
  if (bloqueo == 0){
    if(estado_boton2 == HIGH & estado_boton1 == LOW & estado_boton3 == LOW)
    {
      bandera2 = 1;
      delay(10);
      inByte = 8;
      Serial.write(inByte);
    }
    else
    {
      if(bandera2 == 1 & estado_boton2 == LOW)
      {
        Serial.println("2");
        inByte = 2;
        Serial.write(inByte);
        /*UART1 = 0;
        UART2 = 1;
        UART3 = 0;*/
        
        bandera2 = 0;
      }
    }
  }
//****************************************************************************
  
  emergencia();
  
  if (bloqueo == 0){
    if(estado_boton3 == HIGH & estado_boton2 == LOW & estado_boton1 == LOW)
    {
      bandera3 = 1;
    }
    else
    {
      if(bandera3 == 1 & estado_boton3 == LOW)
      {
        Serial.println("3");
        inByte = 3;
        Serial.write(inByte);
        /*UART1 = 0;
        UART2 = 0;
        UART3 = 1;*/
        
        bandera3 = 0;
      }
    }
  }
//****************************************************************************
  
  emergencia();
  
  if (bloqueo == 0){
    if(estado_boton4 == HIGH & estado_boton5 == LOW & estado_boton6 == LOW)
    {
      bandera4 = 1;
    }
    else
    {
      if(bandera4 == 1 & estado_boton4 == LOW)
      {
        Serial.println("4");
        inByte = 4;
        Serial.write(inByte);
        /*UART4 = 1;
        UART5 = 0;
        UART6 = 0;*/
        
        bandera4 = 0;
      }
    }
  }
//****************************************************************************
  
  emergencia();
  
  if (bloqueo == 0){
    if(estado_boton5 == HIGH & estado_boton4 == LOW & estado_boton6 == LOW)
    {
      bandera5 = 1;
    }
    else
    {
      if(bandera5 == 1 & estado_boton5 == LOW)
      {
        Serial.println("5");
        inByte = 5;
        Serial.write(inByte);
        /*UART4 = 0;
        UART5 = 1;
        UART6 = 0;*/
        
        bandera5 = 0;
      }
    }
  }
//****************************************************************************
  
  emergencia();
  
  if (bloqueo == 0){
    if(estado_boton6 == HIGH & estado_boton4 == LOW & estado_boton5 == LOW)
    {
      bandera6 = 1;
    }
    else
    {
      if(bandera6 == 1 & estado_boton6 == LOW)
      {
        Serial.println("6");
        inByte = 6;
        Serial.write(inByte);
        
        /*UART4 = 0;
        UART5 = 0;
        UART6 = 1;*/
        
        bandera6 = 0;
      }
    }
  }
//****************************************************************************
  
  emergencia();
  if (bloqueo == 0){
    if(estado_boton1 == LOW & estado_boton2 == LOW & estado_boton3 == LOW & estado_boton4 == LOW & estado_boton5 == LOW & estado_boton6 == LOW)
    {
      bandera0 = 1;
    }
    else
    {
      if(bandera0 == 1)
      {
        Serial.println("0");
        inByte = 0;
        Serial.write(inByte);
        
        /*UART4 = 0;
        UART5 = 0;
        UART6 = 1;*/
        
        bandera0 = 0;
      }
    }
  }    
}
void emergencia(void)
{
//***************************************************************************
  if (Serial.available())
  {
    instruccion = Serial.read();
  }
  if (instruccion == 1)
  {
    bloqueo = 1;
  }
  if (instruccion == 0)
  {
    bloqueo = 0;
  }
//***************************************************************************  
}
