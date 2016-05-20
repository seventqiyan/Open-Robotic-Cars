#include <U8glib.h>//显示屏

U8GLIB_ST7920_128X64_1X u8g(3, 9, 8); //SCK = en = 18, MOSI = rw = 16, CS = di = 17//屏幕接线

void setup()
{
    Serial.begin(115200);//串口波特率
  
  }

  void loop()
  {
     u8g.firstPage();//显示必备
  do
  {
    lcd();
  } while ( u8g.nextPage() );
    }
    void lcd()
{

  u8g.setFont(u8g_font_timB08);
  // 转速
  u8g.setPrintPos(1, 26);
  u8g.print("tire_speed");
 
}
