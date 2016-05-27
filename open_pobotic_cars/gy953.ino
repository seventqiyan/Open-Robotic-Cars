
void get_gy953()//加速度模块
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
  if (Re_buf[0] == 0x5A && Re_buf[1] == 0x5A ) //检查帧头，帧尾
  {
    ACC[0] = (Re_buf[8] << 8 | Re_buf[9]) / 100; //合成数据，去掉小数点后2位
    ACC[1] = (Re_buf[6] << 8 | Re_buf[7]) / 100;
    ACC[2] = (Re_buf[4] << 8 | Re_buf[5]) / 100;
    ACC_x = ACC[0]; //除以16383获得加速度值
    ACC_y = ACC[1];
    ACC_z = ACC[2];
    // delay(10);
  }
}

/*void serialEvent111() //串口1中断程序
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
  }*/
