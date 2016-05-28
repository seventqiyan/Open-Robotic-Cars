/*
  由于出现不可复现问题，此部分暂时停用
*/
void get_gy953()
{
  while (Serial1.available())
  {
    Re_buf[counter] = (unsigned char)Serial1.read();
    if (counter == 0 && Re_buf[0] != 0x5A) return; // 检查帧头
    counter++;
    if (counter == 11)             //接收到数据
    {
      counter = 0;               //重新赋值，准备下一帧数据的接收
    }
  }
  if (Re_buf[0] == 0x5A && Re_buf[2] == 0x15 ) //检查帧头，帧尾数据是否符合（0x15）
  {
    ACC[0] = (Re_buf[8] << 8 | Re_buf[9]) / 100; //合成数据，去掉小数点后2位
    ACC[1] = (Re_buf[6] << 8 | Re_buf[7]) / 100;
    ACC[2] = (Re_buf[4] << 8 | Re_buf[5]) / 100;
    ACC_x = abs(ACC[0]);
    ACC_y = abs(ACC[1]);
    ACC_z = abs(ACC[2]);
  }
}
