void lcd()/* 显示屏函数（位置已经调整好）*/
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
