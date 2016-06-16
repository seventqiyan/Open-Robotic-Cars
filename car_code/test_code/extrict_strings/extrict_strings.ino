//#define SERIAL_TX_BUFFER_SIZE 128//修改串口缓存区大小
//#define SERIAL_RX_BUFFER_SIZE 128//修改串口缓存区大小
//#include <stdio.h>
//#include <string.h>
int obs1, obs2;
String comdata = "";   // 输入的字符串
void setup()
{
  Serial.begin(115200);
}

void loop()
{
  int i, j;
  while (Serial.available() > 0)
  {
    comdata += Serial.read();
    delay(2);
  }
  char data[comdata.length()];
  for ( i = 0; i < comdata.length(); i++)
  {
    data[i] = comdata[i];
  }
  data[i] = '\0';
  for (j = 0; j < comdata.length(); j++)
  {
    if (isDigit(data[j]))
    {

    }
  }
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
