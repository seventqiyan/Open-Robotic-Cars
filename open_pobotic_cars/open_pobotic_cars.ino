/*
  由于使用了看门狗，请用ISP直接烧录！
  定时器与引脚关系
  timer 0 (controls pin 13, 4)
  timer 1 (controls pin 12, 11)
  timer 2 (controls pin 10, 9)//由于使用了定时中断这里不能用作PWM输出了
  timer 3 (controls pin 5, 3, 2)
  timer 4 (controls pin 8, 7, 6)
*/
/*******************头文件引用部分****************/
#include <U8glib.h>//显示屏
U8GLIB_ST7920_128X64_1X u8g(3, 9, 8); //SCK = en = 18, MOSI = rw = 16, CS = di = 17//屏幕接线
#include <avr/wdt.h>//看门狗
#include<FlexiTimer2.h>//定时中断（2560用这个）
#include <Servo.h>//舵机库
#include <Wire.h>//IIC通信
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

/*******************动态变量部分******************/
boolean total_state = false; //总状态
boolean unmanned = false;//驾驶状态,不允许
boolean host_unmanned = false; //电脑是否禁止自动驾驶,不允许
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
/*************************************************/
void setup()
{
  wdt_enable(WDTO_500MS);//开启看门狗，并设置溢出时间为500ms
  attachInterrupt(encoder_pin, encoder_function , RISING);//中断源，函数encoder_function()，上升沿触发
  attachInterrupt(emergency_switch_pin, emergency_switch , CHANGE ); //中断源，函数emergency_switch()，上升沿触发
  pinMode(encoder_pin, INPUT);//编码器中断引脚定义
  pinMode(emergency_switch_pin, INPUT);//紧急切换引脚定义
  pinMode(steering_whell_voltage, INPUT); //反方盘电压输入引脚
  steering.attach(direction_pwm);//舵机库输出引脚定义
  Wire.begin();//不设置地址，当作主机
  Serial.begin(115200);//串口波特率
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
  { //手动控制
    total_state = false; //总状态
    steering_whell_voltage_adc = Filter( steering_whell_voltage); //方向滤波赋值
    accelerator_voltage_adc = Filter(accelerator_voltage);//油门滤波赋值
    accelerator_voltage_out_pwm = map(accelerator_voltage_adc, 0, 1024, 0, 255);//缩放赋值（待调试！！！）
    analogWrite(accelerator_voltage, accelerator_voltage_out_pwm); //油门输出
    //方向暂时不写（待器件测试）

  }
  else
  { //自动控制
    total_state = true; //总状态

  }
  wdt_reset();//喂狗
  speed_per_hour();//转速以及时速函数

  u8g.firstPage();
  do
  {
    LCD();
  } while ( u8g.nextPage() );

  wdt_reset();
}

/*******
***********函数部分******************
*******/

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
/*
*/
void speed_per_hour()//转速，时速函数
{
  tire_speed = 60000 / encoder_time1;//求转速r/min
  per_hour = (tire_speed * perimeter) * 6; //千米每小时km/h
}
/*
*/
void serial_print()//串口输出函数
{
  Serial.print("tire_speed=");
  Serial.print(tire_speed);
  Serial.print("per_hour=");
  Serial.print(per_hour);
  Serial.print("unmanned=");
  Serial.print(unmanned);
  Serial.print("host_unmanned=");
  Serial.print(host_unmanned);
  /************************************/
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
/*
  显示屏函数(！！！所有位置参数待修改！！！)
*/
void LCD()
{

  u8g.setFont(u8g_font_timB08);
  u8g.drawStr( 0, 20, "Open Robotic Cars");
  // 转速
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //时速
  u8g.setPrintPos(2, 26);
  u8g.print(tire_speed);
  //超声波1
  u8g.setPrintPos(3, 26);
  u8g.print(tire_speed);
  //超声波2
  u8g.setPrintPos(4, 26);
  u8g.print(tire_speed);
  //超声波3
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //超声波4
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //超声波5
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //超声波6
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //超声波6
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //超声波7
  u8g.setPrintPos(1, 26);
  u8g.print(tire_speed);
  //状态
  u8g.setPrintPos(1, 26);
  u8g.print(total_state);
}
