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
    // Serial.println(com_data);//显示读入的字符串
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
