
//#include <SoftwareSerial.h>
//SoftwareSerial Serial1(2, 3); //用2，3作为虚拟串口（RX，TX）

String comdata = "";//字符串变量，赋空值
//int  obstacle_number = 60; //分别对应超声波1~8
int obstacle_1;/*超声波1距离*/
void setup()
{
  delay(200);//延时一会等供电稳定
  Serial.begin(115200);//串口通信波特率
  Serial1.begin(115200);//虚拟串口波特率，不稳定则往下调整
}
void loop()
{
  delay(100);
  Serial1.write(0x54);//模块测距命令
  Serial1.write(0x60);//模块地址
 
  while (Serial1.available() > 0)
  {
    comdata += char(Serial1.read());
    delay(2);
 
  Serial.print("obstacle_1: ");
  Serial.println(comdata);
  delay(500);
  comdata = String("");
}
