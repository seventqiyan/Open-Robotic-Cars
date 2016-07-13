
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); //用2，3作为虚拟串口（RX，TX）

String data = "";//字符串变量，赋空值
int  obstacle_number = 0x60; //分别对应超声波1~8
int long uart_data[8] = {0};

int serial_tag = 0;//标记串口是否被读取

int obstacle_1;/*超声波1距离*/
unsigned char Re_buf[11], counter = 0;
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

  while (mySerial.available())
  {
    Re_buf[counter] = (unsigned char)mySerial.read();
    if (counter == 0 ) return; // 检查帧头
    counter++;
    if (counter == 11)             //接收到数据
    {
      counter = 0;               //重新赋值，准备下一帧数据的接收
    }
  }
  if (Re_buf[0] == 0x5A && Re_buf[2] == 0x15 ) //检查帧头，帧尾数据是否符合（0x15）
  {

  }
}
