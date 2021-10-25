#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdbool.h>

extern char notes0[];
extern uint8_t beats0[];

extern char notes1[];
extern uint8_t beats1[];

extern char notes2[];
extern uint8_t beats2[];

extern char notes3[];
extern uint8_t beats3[];

extern char notes4[];
extern uint8_t beats4[];

extern char notes5[];
extern uint8_t beats5[];

extern char notes6[];
extern uint8_t beats6[];

const int buzzerPin = 9;

int inByte;
int bandera;

int bloqueo1 = 0;
int bloqueo3 = 0;

void botones(void);
int frequency(char note);

  
void setup()
{
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  botones();
  //**********************************************PAUSE MUSIC***************************************************
  if (bandera == 0)
  {
    bloqueo1 = 0;
    bloqueo3 = 0;
    const int songLength = 3;//(sizeof(notes) - 1);
    int tempo = 95;

    int i, duration;

    for (i = 0; i < songLength; i++) // step through the song arrays
    {
      botones();
      char nota = pgm_read_byte_near(notes0 + i);
      int compas = pgm_read_byte_near(beats0 + i);
      
      if (bandera != 0) {
        i = 0;
        break;
      }
      else
      {
        duration = compas * tempo;  // length of note/rest in ms

        if (nota == ' ')          // is this a rest?
        {
          delay(duration);            // then pause for a moment
        }
        else                          // otherwise, play the note
        {
          tone(buzzerPin, frequency(nota), duration);
          delay(duration);            // wait for tone to finish
        }
        delay(tempo / 10);            // brief pause between notes
      }
    }
  }
  //************************************************GAME OVER***************************************************
  if (bandera == 1)
  {
    bloqueo3 = 0;
    
    const int songLength = 14;//(sizeof(notes) - 1);
    int tempo = 95;

    int i, duration;
    
    if (bloqueo1 != 1)
    {
    for (i = 0; i < songLength; i++) // step through the song arrays
    {
      botones();
      char nota = pgm_read_byte_near(notes1 + i);
      int compas = pgm_read_byte_near(beats1 + i);
      
      if (bandera != 1) {
        i = 0;
        break;
      }
      else
      {
        duration = compas * tempo;  // length of note/rest in ms

        if (nota == ' ')          // is this a rest?
        {
          delay(duration);            // then pause for a moment
        }
        else                          // otherwise, play the note
        {
          tone(buzzerPin, frequency(nota), duration);
          delay(duration);            // wait for tone to finish
        }
        delay(tempo / 10);            // brief pause between notes
      }
    }
      bloqueo1 = 1;
    }
  }
  //*********************************************VICTORY FANFARE************************************************
  if (bandera == 2)
  {
    bloqueo1 = 0;
    bloqueo3 = 0;
    
    const int songLength = 330;
    int tempo = 115;

    int i, duration;

    for (i = 0; i < songLength; i++) // step through the song arrays
    {
      botones();
      char nota2 = pgm_read_byte_near(notes2 + i);
      int compas2 = pgm_read_byte_near(beats2 + i);
      
      if (bandera != 2) {
        i = 0;
        break;
      }
      else
      {
        duration = compas2 * tempo;  // length of note/rest in ms
        
        if (nota2 == ' ')          // is this a rest?
        {
          delay(duration);            // then pause for a moment
        }
        else                          // otherwise, play the note
        {
          tone(buzzerPin, frequency(nota2), duration);
          delay(duration);            // wait for tone to finish
        }
        delay(tempo / 10);            // brief pause between notes
      }
    }
  }
  //***********************************************GALAGA INTRO*************************************************
  if (bandera == 3)
  {
    bloqueo1 = 0;

    const int songLength = 34;
    int tempo = 85;
    
    int i, duration;
    
    if (bloqueo3 != 1)
    {
    for (i = 0; i < songLength; i++) // step through the song arrays
    {
        botones();
        char nota3 = pgm_read_byte_near(notes3 + i);
        int compas3 = pgm_read_byte_near(beats3 + i);
        
        if (bandera != 3) {
          i = 0;
          break;
        }
        else
        {
          duration = compas3 * tempo;  // length of note/rest in ms
          
          if (nota3 == ' ')          // is this a rest?
          {
            delay(duration);            // then pause for a moment
          }
          else                          // otherwise, play the note
          {
            tone(buzzerPin, frequency(nota3), duration);
            delay(duration);            // wait for tone to finish
          }
          delay(tempo / 10);            // brief pause between notes
        }
      }
      bloqueo3 = 1;
    }
    }  
  //********************************************MARIO GROUND THEME**********************************************
  if (bandera == 4)
  {
    bloqueo1 = 0;
    bloqueo3 = 0;
    
    const int songLength = 454;
    int tempo = 95;
  
    int i, duration;

    for (i = 0; i < songLength; i++) // step through the song arrays
    {

      botones();
      char nota4 = pgm_read_byte_near(notes4 + i);
      int compas4 = pgm_read_byte_near(beats4 + i);
      
      if (bandera != 4) {
        i = 0;
        break;
      }
      else
      {
        duration = compas4 * tempo;  // length of note/rest in ms
        
        if (nota4 == ' ')          // is this a rest?
        {
          delay(duration);            // then pause for a moment
        }
        else                          // otherwise, play the note
        {
          tone(buzzerPin, frequency(nota4), duration);
          delay(duration);            // wait for tone to finish
        }
        delay(tempo / 10);            // brief pause between notes
      }
    }
  }
  //**********************************************POKEMON INTRO*************************************************
  if (bandera == 5)
  {
    bloqueo1 = 0;
    bloqueo3 = 0;
    
    const int songLength = 161;
    int tempo = 115;
  
    int i, duration;

    for (i = 0; i < songLength; i++) // step through the song arrays
    {

      botones();
      char nota5 = pgm_read_byte_near(notes5 + i);
      int compas5 = pgm_read_byte_near(beats5 + i);
      
      if (bandera != 5) {
        i = 0;
        break;
      }
      else
      {
        duration = compas5 * tempo;  // length of note/rest in ms
        
        if (nota5 == ' ')          // is this a rest?
        {
          delay(duration);            // then pause for a moment
        }
        else                          // otherwise, play the note
        {
          tone(buzzerPin, frequency(nota5), duration);
          delay(duration);            // wait for tone to finish
        }
        delay(tempo / 10);            // brief pause between notes
      }
    }
  }
  //***********************************************PALLET TOWN**************************************************
  if (bandera == 6)
  {
    bloqueo1 = 0;
    bloqueo3 = 0;
    
    const int songLength = 84;
    int tempo = 121;
  
    int i, duration;

    for (i = 0; i < songLength; i++) // step through the song arrays
    {

      botones();
      char nota6 = pgm_read_byte_near(notes6 + i);
      int compas6 = pgm_read_byte_near(beats6 + i);
      
      if (bandera != 6) {
        i = 0;
        break;
      }
      else
      {
        duration = compas6 * tempo;  // length of note/rest in ms
        
        if (nota6 == ' ')          // is this a rest?
        {
          delay(duration);            // then pause for a moment
        }
        else                          // otherwise, play the note
        {
          tone(buzzerPin, frequency(nota6), duration);
          delay(duration);            // wait for tone to finish
        }
        delay(tempo / 10);            // brief pause between notes
      }
    }
  }
}

//*****************************************************************************************************************************************

int frequency(char note)
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.

  int i;

  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'r', 't', 'T', 'o', 'l', 's', 'c', '1', 'd', '2', 'e', 'f', '3', 'g', '4', 'a', '5', 'b', 'C', '6', 'D', '7', 'E', 'F', '8', 'G', '9', 'A', '0', 'B', 'v', 'V', 'n', 'm', 'ñ', 'p' };
  int frequencies[] = {147, 175, 185, 196, 220, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1319, 1397, 1568};

  const int numNotes = sizeof(names);  // number of notes we're storing

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.

  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return (frequencies[i]);    // Yes! Return the frequency
    }
  }
  return (0); // We looked through everything and didn't find it,
  // but we still need to return a value, so return 0.
}
//***************************************************************************************************************************************
// Función para manejar botones PUSH1 y PUSH2 de la TIVAC
//***************************************************************************************************************************************
void botones(void)
{
  if (Serial.available())
  {
    inByte = Serial.read();
  }

  if (inByte == 0)
  {
    bandera = 0;
  }
  
  if (inByte == 1)
  {
    bandera = 1;
  }

  if (inByte == 2)
  {
    bandera = 2;
  }

  if (inByte == 3)
  {
    bandera = 3;
  }

  if (inByte == 4)
  {
    bandera = 4;
  }
  
  if(inByte == 5)
  {
  bandera = 5;
  }

  if(inByte == 6)
  {
  bandera = 6;
  }
}
