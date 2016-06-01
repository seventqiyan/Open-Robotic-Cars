/*
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

//更新地址部分(例子)
void update_address()
{
  Serial2.write(0x41);//更新地址指令
  Serial2.write(0x54);//模块地址
  Serial2.write(xxxx);//新地址
}
*/
