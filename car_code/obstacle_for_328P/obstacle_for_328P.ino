/*
  由于资源占用原因，测距模块独立出来，交给328P来做
  串口被用作超声波测距，故采用IIC方式与2560通信
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
#include <Wire.h>//IIC通信

String data = "";//字符串变量，赋空值
int obstacle_number[8] = {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67}; //分别对应超声波1~8
int long iic_data[8] = {0};
int counter = 0;//测距状态定义
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
  Serial.begin(115200);//超声波串口通信波特率
  // wdt_enable(WDTO_1S);//开启看门狗，并设置溢出时间为1s
  pinMode(led, OUTPUT);//指示灯输出引脚定义
  Wire.begin(2);//注册I2C地址，设为2号设备
  Wire.onRequest(obstacle_wire);//收到命令就调用obstacle_wire函数
}
void loop()
{
  for (int i = 1; i < 8; i++)
  {

  }



}
void obstacle_wire()
{
  Wire.write(obstacle_1); //送出超声波1距离数据
  Wire.write(",");
  Wire.write(obstacle_2); //送出超声波2距离数据
  Wire.write(",");
  Wire.write(obstacle_3); //送出超声波3距离数据
  Wire.write(",");
  Wire.write(obstacle_4); //送出超声波4距离数据
  Wire.write(",");
  Wire.write(obstacle_5); //送出超声波5距离数据
  Wire.write(",");
  Wire.write(obstacle_6); //送出超声波6距离数据
  Wire.write(",");
  Wire.write(obstacle_7); //送出超声波7距离数据
}




/*
  测距一次需要60ms，也就是说一秒钟最多16次，
  考虑单片机资源占用，以及实时性要求，会用328P来跑测距，然后用串口通信
  int obstacle_number[8]={0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67};//分别对应超声波1~8
  void serial2()
  {

  }

  int serial2_obstacle()//串口超声波
  {
  unsigned char S_obs[24], i = 0;
  }
  Serial2.write(0x54);//模块地址
  Serial2.write(0x60);//让模块测距
  while (Serial2.available())
  {
  S_obs[i] = (unsigned char)Serial2.read();




  }
*/

/*更新地址部分(例子)
  void update_address()
  {
  Serial2.write(0x41);//更新地址指令
  Serial2.write(0x54);//模块地址
  Serial2.write(xxxx);//新地址
  }
*/
