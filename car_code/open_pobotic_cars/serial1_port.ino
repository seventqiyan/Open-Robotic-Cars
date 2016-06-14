/*超声波串口接收函数*/
void serial2_port()
{
  String com1_data = "";//字符串变量，赋空值
  int long osb_data[5] = {0}, serial1_tag = 0;
  int long osb_check;//校验值
  int j = 0; //j为拆分后数组的位置计数
  while (Serial1.available() > 0)//不断检查串口是否有数据
  {
    com1_data += char(Serial1.read());//读入的数据给com1_data
    delay(2);//延时，不然丢数据
    serial1_tag = 1;//标记串口是否被读取
  }
  if (serial1_tag = 1);//如果接收到数据则执行com1_data分析操作，否则什么都不做。
  {
    // Serial.println(com_data);//显示读入的字符串
    /**********************这里是重点************************/
    for (int i = 0; i < com1_data.length(); i++)//以串口读取字符串长度循环，
      if (com1_data[i] == ',')
      {
        j++;
      }
      else
      {
        osb_data[j] = osb_data[j] * 10 + (com1_data[i] - '0');
      }
  }
  com1_data = String("");
  obstacle_1 = osb_data[1];
  obstacle_2 = osb_data[2];
  obstacle_3 = osb_data[3];
  obstacle_4 = osb_data[4];
  obstacle_5 = osb_data[5];
  obstacle_6 = osb_data[6];
  obstacle_7 = osb_data[7];
  obstacle_8 = osb_data[8];
  osb_check = osb_data[9];
  serial1_tag = 0;//serial_tag置0
}
