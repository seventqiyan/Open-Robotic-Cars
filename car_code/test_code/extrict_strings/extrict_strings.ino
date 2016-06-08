#define SERIAL_TX_BUFFER_SIZE 128//修改串口缓存区大小
#define SERIAL_RX_BUFFER_SIZE 128//修改串口缓存区大小
#include <stdio.h>
#include <string.h>

String comdata = "";
char* b[3]; int j = 0;
int a, c;
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
  int k = comdata.length();
  char data[k + 1];
  for (int i = 0; i < k; i++)
  {
    data[i] = comdata[i];
  }
  char *token = strtok(data, "mm");
  while (token != NULL)
  {
    j++;
    b[j] = token = strtok(NULL, "mm");
  }
  a = atoi(b[1]);
  c = atoi(b[2]);
  Serial.print(a);
  Serial.println(c);
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
