#define SERIAL_TX_BUFFER_SIZE 128//修改串口缓存区大小
#define SERIAL_RX_BUFFER_SIZE 128//修改串口缓存区大小
#include <stdio.h>
#include <string.h>

String comdata = "";
String com = "";
void setup()
{
  Serial.begin(115200);
}

void loop()
{
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
  }
trim(comdata, com);

}








//T`A = 565mm   B = 628mm  D = err
/*
  int strnumbers(const char *str, int strLen, double *valArr, int valCont)
  {
  char number[] = "123456789";
  int currPos = 0;
  int valCount = 0;
  while (currPos < strLen) {
    int startPos = find_first_of(str, strLen, number, currPos);
    currPos = startPos = startPos + 1;
    int endPos = find_fist_not_of(str, strLen, number, currPos);
    valArr[valCount++] = str2number(str, staPos, endPos);
  }
  return valCount;
  }*/
