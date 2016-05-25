/*
  由于使用了看门狗，请用ISP直接烧录！
  定时器与引脚关系
  timer 0 (controls pin 13, 4)
  timer 1 (controls pin 12, 11)
  timer 2 (controls pin 10, 9)//由于使用了定时中断这里不能用作PWM输出了
  timer 3 (controls pin 5, 3, 2)
  timer 4 (controls pin 8, 7, 6)
 ********************* 串口通信协议部分********************
  许可状态+方向+速度+刹车+总和（中间使用"，"分割，电脑赋值check）
  /*******************头文件引用部分****************/
#include <U8glib.h>//显示屏
U8GLIB_ST7920_128X64 u8g(3, 9, 8, U8G_PIN_NONE); //SCK = en = 18, MOSI = rw = 16, CS = di = 17//屏幕接线（已经调试完毕）

//#include <avr/wdt.h>//看门狗

#include<FlexiTimer2.h>//定时中断（2560用这个）

#include <Servo.h>//舵机库

#include <Wire.h>//IIC通信

#include <PID_v1.h>//PID库不懂的看这里：http://playground.arduino.cc/Code/PIDLibrary

/*******************常量定义部分******************/
const int perimeter = 60;//轮胎周长单位/cm
Servo steering;//方向舵机
/*******************引脚定义部分******************/
const int encoder_pin = 2;//编码器中断引脚
const int emergency_switch_pin = 3;//紧急切换引脚

const int accelerator_pwm = 8; //油门PWM值输出引脚
const int direction_pwm = 9; //方向PWM值输出引脚

const int steering_whell_voltage = 0;//方向电压输入引脚
const int accelerator_voltage = 1; //油门电压输入引脚

const int brake_in = 22;//刹车输入
const int brake_out = 23;//刹车输出

/*******************动态变量部分******************/
boolean total_state = false; //总状态
boolean unmanned = false;//驾驶状态,不允许
boolean host_unmanned = false; //电脑是否禁止自动驾驶,不允许
boolean brake_unmanned = false; //刹车状态

int encoder_count = 0; //编码器计数
unsigned int encoder_time0;//临时时间变量
unsigned int encoder_time1; //一圈时间差
int tire_speed = 0, per_hour = 0; //转速r/min，时速km/h

int steering_whell_voltage_adc;//方向电压数模转换变量
int accelerator_voltage_adc;//油门电压数模转换变量

int steering_whell_voltage_out_pwm;//方向电输出变量
int accelerator_voltage_out_pwm;//油门电压输出变量

int obstacle_1,/*超声波1距离*/
    obstacle_2,/*2*/
    obstacle_3,/*3*/
    obstacle_4,/*4*/
    obstacle_5,/*5*/
    obstacle_6,/*6*/
    obstacle_7,/*7*/
    obstacle_8;/*8*/
unsigned int tellurometer_survey;//微波测距

//串口变量
String com_data = "";//字符串变量，赋空值
int long num_data[5] = {0}, serial_tag = 0;
int long check;//校验值
//串口1变量
int ACC[3];
unsigned char Re_buf[11], counter = 0;
unsigned char sign = 0;
int ACC_x, ACC_y, ACC_z;
/*************************************************/
void setup()
{
  // wdt_enable(WDTO_500MS);//开启看门狗，并设置溢出时间为500ms

  attachInterrupt(encoder_pin, encoder_function , RISING);//中断源，函数encoder_function()，上升沿触发
  attachInterrupt(emergency_switch_pin, emergency_switch , CHANGE ); //中断源，函数emergency_switch()，上升沿触发

  pinMode(encoder_pin, INPUT);//编码器中断引脚定义
  pinMode(emergency_switch_pin, INPUT);//紧急切换引脚定义

  pinMode(steering_whell_voltage, INPUT); //反方盘电压输入引脚
  steering.attach(direction_pwm);//舵机库输出引脚定义

  Wire.begin();//不设置地址，当作主机
  Serial.begin(115200);//上位机串口波特率
  //GY953
  Serial1.begin(115200);//GY953传感器串口波特率
  delay(2000);       //等一会~让传感器初始化
  Serial1.write(0XA5);    //命令帧头
  Serial1.write(0X15);    //指令（连续输出加速度）
  Serial1.write(0XBA);    //校验和

  ADCSRA |=  (1 << ADPS2);
  ADCSRA &=  ~(1 << ADPS1);
  ADCSRA &=  ~(1 << ADPS0);

}
/***********************************************/
void loop()
{
  if ((unmanned == false && host_unmanned == true)//(手动不允许，电脑允许)
      || (unmanned == false && host_unmanned == false)//(手动电脑都不允许)
      || (unmanned == true && host_unmanned == false)) //(手动允许电脑允许)
  {
    //手动控制
    total_state = false; //总状态
    steering_whell_voltage_adc = Filter( steering_whell_voltage); //方向滤波赋值
    accelerator_voltage_adc = Filter(accelerator_voltage);//油门滤波赋值

    accelerator_voltage_out_pwm = map(accelerator_voltage_adc, 0, 1024, 0, 255);//缩放赋值（待调试！！！）
    analogWrite(accelerator_voltage, accelerator_voltage_out_pwm); //油门输出
    //方向(测试部分代码)

  }
  else
  {
    //自动控制
    total_state = true; //总状态
    Serial.print("Data");//向电脑要数据
    delay(2);//延时
    serial_port();//读取串口数据
    if ( check == host_unmanned + steering_whell_voltage_out_pwm + accelerator_voltage_out_pwm + brake_unmanned )//校验数据是否正确
    {
      analogWrite(accelerator_voltage, accelerator_voltage_out_pwm); //油门输出
      //方向待写
    }
    else
    {
      Serial.print("Data_Error");//数据不对~
    }
  }
  speed_per_hour();//转速以及时速函数

  u8g.firstPage();//显示必备
  do
  {
    lcd();
  } while ( u8g.nextPage() );

  // wdt_reset();//喂狗
}

/******************函数部分*************************/

void emergency_switch()//紧急切换开关函数
{
  if (digitalRead(emergency_switch_pin) == HIGH)
    unmanned = false;
  else
    unmanned = true;
}
/*
*/
void encoder_function()//测速中断函数
{
  if (encoder_count = 0 ) {
    encoder_time0 = micros();
    encoder_count = 1;
  }
  else {
    encoder_time1 = micros() - encoder_time0;
    encoder_count = 0;
  }
}

void speed_per_hour()//转速，时速函数
{
  tire_speed = 60000 / encoder_time1;//求转速r/min
  per_hour = (tire_speed * perimeter) * 6; //千米每小时km/h
}

void serial_print_out()//串口输出函数
{
  Serial.print("tire_speed=");
  Serial.print(tire_speed);
  Serial.print("per_hour=");
  Serial.print(per_hour);
  Serial.print("unmanned=");
  Serial.print(unmanned);
  Serial.print("host_unmanned=");
  Serial.print(host_unmanned);

  Serial.print("tellurometer_survey=");
  Serial.print(tellurometer_survey);
  Serial.print("obstacle_1=");
  Serial.print(obstacle_1);
  Serial.print("obstacle_2=");
  Serial.print(obstacle_2);
  Serial.print("obstacle_3=");
  Serial.print(obstacle_3);
  Serial.print("obstacle_4=");
  Serial.print(obstacle_4);
  Serial.print("obstacle_5=");
  Serial.print(obstacle_6);
  Serial.print("obstacle_7=");
  Serial.print(obstacle_7);
  Serial.print("obstacle_8=");
  Serial.print(obstacle_8);
  Serial.println();
  delay(2);
}
/*Hi.Bob提供的滤波函数*/
#define DATA_MAX 10
int Filter(int direct)
{
  unsigned int val;
  unsigned int sum = 0;
  unsigned int maxVal = 0;
  unsigned int minVal = 0;
  for (int i = 0; i < DATA_MAX; i++) {
    val = analogRead(direct);
    sum += val;
    /* Get the max */
    if (val > maxVal)
      maxVal = val;
    /* Get the minVal */
    if (val < minVal)
      minVal = val;
  }
  /* Deal the maxVal and the minVal */
  sum -= maxVal;
  sum -= minVal;
  return sum / (DATA_MAX - 2);
}
/* 显示屏函数（位置已经调整好）*/
void lcd()
{
  u8g.setFont(u8g_font_chikitar);//字体设置
  // 转速
  u8g.setPrintPos(64, 29);
  u8g.print("S=");
  u8g.print(tire_speed);
  //时速
  u8g.setPrintPos(64, 35);
  u8g.print("V=");
  u8g.print( per_hour);
  //超声波1
  u8g.setPrintPos(3, 5);
  u8g.print("C_1=");
  u8g.print(obstacle_1);
  //超声波2
  u8g.setPrintPos(0, 11);
  u8g.print("C_2=");
  u8g.print(obstacle_2);
  //超声波3
  u8g.setPrintPos(0, 17);
  u8g.print("C_3=");
  u8g.print(obstacle_3);
  //超声波4
  u8g.setPrintPos(0, 23);
  u8g.print("C_4=");
  u8g.print(obstacle_4);
  //超声波5
  u8g.setPrintPos(0, 29);
  u8g.print("C_5=");
  u8g.print(obstacle_5);
  //超声波6
  u8g.setPrintPos(0, 35);
  u8g.print("C_6=");
  u8g.print(obstacle_6);
  //超声波7
  u8g.setPrintPos(0, 41);
  u8g.print("C_7=");
  u8g.print(obstacle_7);
  //超声波8
  u8g.setPrintPos(0, 47);
  u8g.print("C_8=");
  u8g.print(obstacle_8);
  //状态
  u8g.setPrintPos(0, 64);
  u8g.print("Z=");
  u8g.print(total_state);
  //方向电压数模转换变量
  u8g.setPrintPos(64, 5);
  u8g.print("F_IN=");
  u8g.print(steering_whell_voltage_adc);
  //油门电压数模转换变量
  u8g.setPrintPos(64, 11);
  u8g.print("Y_IN=");
  u8g.print(accelerator_voltage_adc);
  //方向电输出变量
  u8g.setPrintPos(64, 17);
  u8g.print("F_OUT=");
  u8g.print(steering_whell_voltage_out_pwm);
  //油门电压输出变量
  u8g.setPrintPos(64, 23);
  u8g.print("Y_OUT=");
  u8g.print(accelerator_voltage_out_pwm);
}
/*
  串口读取上位机数据部分,参考来源：http://www.geek-workshop.com/thread-260-1-1.html
*//*许可状态+方向+速度+刹车+总和（中间使用"，"分割）*/
void serial_port()
{
  int j = 0; //j为拆分后数组的位置计数
  while (Serial.available() > 0)//不断检查串口是否有数据
  {
    com_data += char(Serial.read());//读入的数据给com_data
    delay(2);//延时，不然丢数据
    serial_tag = 1;//标记串口是否被读取
  }
  if (serial_tag = 1);//如果接收到数据则执行com_data分析操作，否则什么都不做。
  {
    Serial.println(com_data);//显示读入的字符串
    /**********************这里是重点************************/
    for (int i = 0; i < com_data.length(); i++)//以串口读取字符串长度循环，
      if (com_data[i] == ',')
      {
        //逐个分析com_data[i]字符串的文字，如果碰到文字是分隔符（这里选择逗号分割）则将结果数组位置下移一位
        //即比如11,22,33,55开始的11记到num_data[0];碰到逗号就j等于1了，
        //再转换就转换到num_data[1];再碰到逗号就记到num_data[2];以此类推，直到字符串结束
        j++;
      }
      else
      {
        //如果没有逗号的话，就将读到的数字*10加上以前读入的数字，
        //并且(com_data[i] - '0')就是将字符'0'的ASCII码转换成数字0（下面不再叙述此问题，直接视作数字0）
        //比如输入数字是12345，有5次没有碰到逗号的机会，就会执行5次此语句。
        //因为左边的数字先获取到，并且num_data[0]等于0，
        //所以第一次循环是num_data[0] = 0*10+1 = 1
        //第二次num_data[0]等于1，循环是num_data[0] = 1*10+2 = 12
        //第三次是num_data[0]等于12，循环是num_data[0] = 12*10+3 = 123
        //第四次是num_data[0]等于123，循环是num_data[0] = 123*10+4 = 1234
        //如此类推，字符串将被变成数字0。
        num_data[j] = num_data[j] * 10 + (com_data[i] - '0');//com_data的字符串全部转换到num_data
      }
  }
  com_data = String("");//清空comdata以便下一次使用
  host_unmanned = num_data[1];//电脑许可状态赋值
  steering_whell_voltage_out_pwm = num_data[2];//方向赋值
  accelerator_voltage_out_pwm = num_data[3];//油门赋值
  brake_unmanned = num_data[4];//刹车状态
  check = num_data[5];//校验值赋值
  serial_tag = 0;//serial_tag置0
}
void get_gy953()//加速度模块
{
  if (sign)
  {
    sign = 0;
    if (Re_buf[0] == 0x5A && Re_buf[1] == 0x5A ) //检查帧头，帧尾
    {
      ACC[0] = (Re_buf[8] << 8 | Re_buf[9]) / 100; //合成数据，去掉小数点后2位
      ACC[1] = (Re_buf[6] << 8 | Re_buf[7]) / 100;
      ACC[2] = (Re_buf[4] << 8 | Re_buf[5]) / 100;
      ACC_x = (ACC[0] / 131);
      ACC_y = (ACC[1] / 131);
      ACC_z = (ACC[2] / 131);
      //   delay(10);
    }
  }
}
void serialEvent1() //串口1中断程序
{
  while (Serial1.available())
  {
    Re_buf[counter] = (unsigned char)Serial1.read();
    if (counter == 0 && Re_buf[0] != 0x5A) return; // 检查帧头
    counter++;
    if (counter == 11)             //接收到数据
    {
      counter = 0;               //重新赋值，准备下一帧数据的接收
      sign = 1;
    }
  }
}
