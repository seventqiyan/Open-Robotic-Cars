
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //用2，3作为虚拟串口（RX，TX）

String comdata = "";//字符串变量，赋空值
int  obstacle_number = 60; //分别对应超声波1~8
int serial_tag = 0;//标记串口是否被读取
int obstacle_1;/*超声波1距离*/
void setup()
{
  delay(200);//延时一会等供电稳定
  Serial.begin(115200);//串口通信波特率
  mySerial.begin(115200);//虚拟串口波特率，不稳定则往下调整
}
void loop()
{
  mySerial.print("T");//模块测距命令
  mySerial.write(obstacle_number);//模块地址

  int j = 0;
  while (mySerial.available() > 0)
  {
    comdata += char(mySerial.read());
    delay(2);
    serial_tag = 1;
  }
  if (serial_tag = 1)
  {
    obstacle_1 = comdata[1] * 1000 + comdata[2] * 100 + comdata[3] * 10 + comdata[4];
  }
  comdata = String("");
  serial_tag = 0;
  Serial.println("obstacle_1");
  Serial.println(obstacle_1);
}
