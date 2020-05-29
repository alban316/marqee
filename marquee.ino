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


#define CHR_G 0
#define CHR_A 1
#define CHR_M 2
#define CHR_E 3
#define CHR_O 4
#define CHR_V 5
#define CHR_R 6
#define CHR_SPC 7
#define CHR_J 8
#define CHR_S 9
#define CHR_N 10
#define CHR_K 11
#define CHR_T 12
#define CHR_H 13
#define CHR_Y 14
#define CHR_HRT 15
#define CHR_D 16
#define CHR_I 17
#define CHR_C 18
#define CHR_SML 19


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
  {120,108,102,102,102,108,120,0}, //D
  {60,24,24,24,24,24,60,0}, //I
  {60,102,96,96,96,102,60,0}, //C
  {60,66,165,129,165,153,66,60} //SML
  
};

uint8_t msg[] = {
    CHR_H, CHR_O, CHR_O, CHR_K, CHR_E, CHR_D, CHR_SPC, CHR_O, CHR_N, CHR_SPC, CHR_T, CHR_R, CHR_O, CHR_N, CHR_I, CHR_C, CHR_S, CHR_SPC, CHR_SML, CHR_SPC
  };

uint8_t len_msg = 20;
int msgindex;
uint8_t offset;
uint8_t left_dst[8];
uint8_t right_dst[8];
uint8_t temp_result[8];
uint8_t *result;
uint8_t *left_src;
uint8_t *right_src;

void loop (void) {
  // inner loop
  for (uint8_t i = 0; i < len_msg * 8; i++) {
    msgindex = i / 8;
    offset = i % 8;

    // return the whole character
    if (offset == 0) {
      result = letter[msg[msgindex]];
    }

    // we are spanning two characters
    else {
      left_src = letter[msg[msgindex]];
      right_src = msgindex > (len_msg - 1) ? letter[CHR_SPC] : letter[msg[msgindex + 1]];
      
      // iterate over array & shift by the offset 
      for (uint8_t j = 0; j < 8; j++) {
        left_dst[j] = left_src[j] << offset;
        right_dst[j] = right_src[j] >> (8 - offset);
        temp_result[j] = left_dst[j] | right_dst[j];        
      }

      result = temp_result;
    }

    render(result);
    delay(60);
  }

}
