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
#define SERIAL_TX_BUFFER_SIZE 128//修改串口缓存区大小
#define SERIAL_RX_BUFFER_SIZE 128//修改串口缓存区大小

#include <U8glib.h>//显示屏
U8GLIB_ST7920_128X64 u8g(2, 3, 4, U8G_PIN_NONE); //SCK = en = 18, MOSI = rw = 16, CS = di = 17//屏幕接线（已经调试完毕原值：3,9,8）

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

const int led_brake = 24;//刹车灯
const int led_left = 25;//左转灯
const int led_right = 26;//右转灯
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

//串口变量(上位机)
String com_data = "";//字符串变量，赋空值
int long num_data[5] = {0}, serial_tag = 0;
int long check;//校验值

//串口1变量（加速度传感器）
int ACC[3];
unsigned char Re_buf[11], counter = 0;
int ACC_x, ACC_y, ACC_z;
/*************************************************/
void setup()
{

  Serial.begin(115200);//上位机串口波特率

  Serial1.begin(115200);//GY953传感器串口波特率
  delay(2000);   //等一会~让传感器初始化
  Serial1.write(0XA5);    //命令帧头
  Serial1.write(0X15);    //指令（连续输出加速度）
  Serial1.write(0XBA);    //校验和

  Serial2.begin(115200);//超声波串口波特率

  // wdt_enable(WDTO_500MS);//开启看门狗，并设置溢出时间为500ms

  attachInterrupt(encoder_pin, encoder_function , RISING);//中断源，函数encoder_function()，上升沿触发
  attachInterrupt(emergency_switch_pin, emergency_switch , CHANGE ); //中断源，函数emergency_switch()，上升沿触发

  pinMode(encoder_pin, INPUT);//编码器中断引脚定义
  pinMode(emergency_switch_pin, INPUT);//紧急切换引脚定义

  pinMode(steering_whell_voltage, INPUT); //方向盘电压输入引脚
  steering.attach(direction_pwm);//舵机库输出引脚定义

  pinMode(led_brake, OUTPUT);//刹车灯输出引脚定义
  pinMode(led_left, OUTPUT);//左转灯输出引脚定义
  pinMode(led_right, OUTPUT);//右转灯输出引脚定义

  Wire.begin();//不设置地址，当作主机

  ADCSRA |=  (1 << ADPS2);
  ADCSRA &=  ~(1 << ADPS1);
  ADCSRA &=  ~(1 << ADPS0);

}
/***********************************************/
void loop()
{

  if ((unmanned == false && host_unmanned == true)//(手动不允许，电脑允许)
      || (unmanned == false && host_unmanned == false)//(手动电脑都不允许)
      || (unmanned == true && host_unmanned == false)) //(手动允许电脑不允许)
  {
    //手动控制
    total_state = false; //总状态
    steering_whell_voltage_adc = Filter( steering_whell_voltage); //方向滤波赋值
    accelerator_voltage_adc = Filter(accelerator_voltage);//油门滤波赋值
    if (digitalRead(brake_in) == HIGH)
    {
      brake();//刹车
    }
    accelerator_voltage_out_pwm = map(accelerator_voltage_adc, 0, 1024, 0, 255);//缩放赋值（待调试！！！）
    analogWrite(accelerator_voltage, accelerator_voltage_out_pwm); //油门输出
    //方向(测试部分代码)
    steering_whell_voltage_out_pwm = map(steering_whell_voltage_adc, 0, 1024, 1000, 2000);//方向缩放赋值（待调试！！！）
    if (1450 <= steering_whell_voltage_out_pwm <= 1550) {
      steering_whell_voltage_out_pwm = 1500; //消除回中偏差造成的抖动
    }
    steering.writeMicroseconds(steering_whell_voltage_out_pwm); //方向输出
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
      if (brake_unmanned = true||digitalRead(brake_in) == HIGH)
      {
        brake();
      }
      analogWrite(accelerator_voltage, accelerator_voltage_out_pwm); //油门输出
      //方向待写
      steering.writeMicroseconds(steering_whell_voltage_out_pwm); //方向输出
    }
    else
    {
      Serial.print("Data_Error");//数据不对~
    }
  }
  speed_per_hour();//转速以及时速函数

  u8g.firstPage();//显示必备，将来肯定要去掉的
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
void brake()//刹车函数ABS防抱死TAT
{ do {
    analogWrite(accelerator_voltage, 0); //油门输出0
    digitalWrite(led_brake, HIGH);//亮个刹车灯
    digitalWrite(brake_out, HIGH);
    delay(500); 
    digitalWrite(brake_out, LOW);
    delay(500);  
    digitalWrite(brake_out, HIGH);
    delay(100);
    digitalWrite(brake_out, LOW);
    delay(100);
    digitalWrite(brake_out, HIGH);
  }
  while (digitalRead(brake_in) == LOW);
      digitalWrite(led_brake, LOW);
  }
