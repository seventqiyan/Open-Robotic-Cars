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
