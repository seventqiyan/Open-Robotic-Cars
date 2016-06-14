#define SERIAL_TX_BUFFER_SIZE 128//修改串口缓存区大小
#define SERIAL_RX_BUFFER_SIZE 128//修改串口缓存区大小
#include <stdio.h>
#include <string.h>

int i;   // 保存PWM需要输出的值
String inString = "";   // 输入的字符串
char LED = ' ';   // 用于判断指定LED颜色对应的引脚
boolean stringComplete = false; // 用于判断数据是否读取完成
void setup()
{
  Serial.begin(115200);
}

void loop()
{
  while (Serial.available() > 0)
  {
    char data = Serial.read();
    delay(2);
  }
  if (isDigit(inChar))
  {
inString += inChar;

  }
  //char str[comdata.length()];
  // n = atoi(comdata);
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
