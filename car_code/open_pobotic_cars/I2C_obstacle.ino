void I2C_obstacle()
{
  Wire.requestFrom(2, 6);    // 要求 2號僕人 透過 Wire 送 6 char 過來!

  while (Wire.available())   // 如果Wire上還有 char 等我讀取
  {
    char c = Wire.read(); // 從 Wire 讀取一個 char
    Serial.print(c);      // 送到串口監視器查看
  }


}
