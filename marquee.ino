#include "logger.h"
#include <stdlib.h>
#include <SPI.h>


#define REG_DECODE (0x09)
#define REG_DIGIT(pos) ((pos) + 1)
#define REG_INTENSITY (0x0A)
#define REG_SCANLIMIT (0x0B)
#define REG_SHUTDOWN (0x0C)
#define REG_DISPLAYTEST (0x0F)
#define ON (0x1)
#define OFF (0x0)


#define LETTER_G 0
#define LETTER_A 1
#define LETTER_M 2
#define LETTER_E 3
#define LETTER_O 4
#define LETTER_V 5
#define LETTER_R 6
#define LETTER_SPC 7
#define LETTER_J 8
#define LETTER_S 9
#define LETTER_N 10
#define LETTER_K 11
#define LETTER_T 12
#define LETTER_H 13
#define LETTER_Y 14
#define LETTER_HRT 15


void set_register(byte address, byte value)
{
  digitalWrite(SS, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(SS, HIGH);
}


void init_max()
{
  set_register(REG_DISPLAYTEST, OFF);
  set_register(REG_INTENSITY, 0x8);
  set_register(REG_SHUTDOWN, OFF);
  set_register(REG_SCANLIMIT, 7);
  set_register(REG_DECODE, B00000000);
}


void setup (void) {
  digitalWrite(SS, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  Serial.begin(9600);
  Serial.println("Program Startup!!!");


  init_max();
}


void render(uint8_t *row) {
  set_register(REG_SHUTDOWN, OFF);

  for (uint8_t i = 0; i < 8; i++)
    set_register(REG_DIGIT(i), row[i]);

  set_register(REG_SHUTDOWN, ON);
}


uint8_t letter[][8] = {
  {60,102,96,110,102,102,60,0}, // G
  {24,60,102,126,102,102,102,0}, // A
  {99,119,127,107,99,99,99,0}, // M
  {126,96,96,120,96,96,126,0}, // E
  {60,102,102,102,102,102,60,0}, // O
  {102,102,102,102,102,60,24,0}, // V
  {124,102,102,124,120,108,102,0}, // R
  {0,0,0,0,0,0,0,0}, // space
  {62,12,12,12,12,108,56,0}, //J
  {60,102,96,60,6,102,60,0}, //S
  {102,118,126,126,110,102,102,0}, //N
  {102,108,120,112,120,108,102,0}, //K
  {126,24,24,24,24,24,24,0}, //T
  {102,102,102,126,102,102,102,0}, //H
  {102,102,102,60,24,24,24,0}, //Y
  {54,127,127,127,62,28,8,0}, //HEART
  
};

uint8_t msg[] = {
    //LETTER_G, LETTER_A, LETTER_M, LETTER_E, LETTER_SPC, LETTER_O, LETTER_V, LETTER_E, LETTER_R
    LETTER_J, LETTER_A, LETTER_S, LETTER_O, LETTER_N, LETTER_HRT, LETTER_K, LETTER_A, LETTER_T, LETTER_H, LETTER_Y, LETTER_SPC
  };

uint8_t len_msg = 12;

void loop (void) {
  // inner loop
  for (uint8_t i = 0; i < len_msg * 8; i++) {
    int msgindex = i / 8;
    uint8_t offset = i % 8;
    uint8_t *result = NULL;

    // return the whole character
    if (offset == 0) {
      result = letter[msg[msgindex]];
    }

    // we are spanning two characters
    else {
      if (result != NULL)
        delete result;
        
      result = new uint8_t[8];
      uint8_t *left_dst = new uint8_t[8];
      uint8_t *right_dst = new uint8_t[8];
      uint8_t *left_src = letter[msg[msgindex]];
      uint8_t *right_src = msgindex > (len_msg - 1) ? letter[LETTER_SPC] : letter[msg[msgindex + 1]];
      
      // iterate over array & shift by the offset 
      for (uint8_t j = 0; j < 8; j++) {
        left_dst[j] = left_src[j] << offset;
        right_dst[j] = right_src[j] >> (8 - offset);
        result[j] = left_dst[j] | right_dst[j];        

      }

      delete left_dst;
      delete right_dst;
    }

    //Logger::log("idle");
    
    render(result);
    delay(80);
  }

}
