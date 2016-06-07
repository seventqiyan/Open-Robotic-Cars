/*
  T`A = 565mm   B = 628mm  D = err
  由于资源占用原因，测距模块独立出来，交给328P来做
  串口被用作超声波测距，故采用串口方式与2560通信
  测距一次需要60ms，也就是说一秒钟最多16次

  1-----2-----3
  |           |
  8     &     4
  |           |
  7-----6-----5

  超声波结构位置图图
*/
#define SERIAL_TX_BUFFER_SIZE 128//修改串口缓存区大小
#define SERIAL_RX_BUFFER_SIZE 128//修改串口缓存区大小


#include <avr/wdt.h>//看门狗
//#include <Wire.h>//IIC通信

#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //用2，3作为虚拟串口（RX，TX）

String data = "";//字符串变量，赋空值
int  obstacle_number[8] = {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67}; //分别对应超声波1~8
int long uart_data[8] = {0};

int counter = 0;//测距状态定义
int serial_tag = 0;//标记串口是否被读取

int led = 13; //定义指示灯引脚为13
int obstacle_1,/*超声波1距离*/
    obstacle_2,/*2*/
    obstacle_3,/*3*/
    obstacle_4,/*4*/
    obstacle_5,/*5*/
    obstacle_6,/*6*/
    obstacle_7,/*7*/
    obstacle_8;/*8*/

void setup()
{
  delay(200);//延时一会等供电稳定
  pinMode(led, OUTPUT);//指示灯输出引脚定义
  Serial.begin(115200);//超声波串口通信波特率
  mySerial.begin(115200);//虚拟串口波特率，不稳定则往下调整
  // wdt_enable(WDTO_1S);//开启看门狗，并设置溢出时间为1s
}
void loop()
{
  for (int i = 1; i < 8; i++)
  {
    Serial.write(0x54);//模块测距命令
    Serial.write(obstacle_number[i]);//模块地址

    while (Serial2.available() > 0)  //等到串口有数据
    {
      data += char(Serial2.read()); //把串口数据给data字符串
      delay(2);  //等一会，不然会丢数据（只大不小）
    }



  }



}

void serial_read()//读取串口赋值
{


}
void obstacle_serial()//输出给2560
{
  mySerial.print(obstacle_1);//
  mySerial.print(",");
  mySerial.print(obstacle_2);//
  mySerial.print(",");
  mySerial.print(obstacle_3);//
  mySerial.print(",");
  mySerial.print(obstacle_4);//
  mySerial.print(",");
  mySerial.print(obstacle_5);//
  mySerial.print(",");
  mySerial.print(obstacle_6);//
  mySerial.print(",");
  mySerial.print(obstacle_7);//
  mySerial.print(",");
  mySerial.print(obstacle_8);//
}
/*更新地址部分(例子)
  void update_address()
  {
  Serial2.write(0x41);//更新地址指令
  Serial2.write(0x54);//模块地址
  Serial2.write(xxxx);//新地址
  }
*/
