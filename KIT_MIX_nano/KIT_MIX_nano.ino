#include <Arduino.h>
#include <TM1637Display.h>
#include <LedControl.h>
// Module connection pins (Digital Pins)
#define CLK 7
#define DIO 6

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

TM1637Display display(CLK, DIO);
// 初始化 LedControl：DATA, CLK, CS, 裝置數量
LedControl lc = LedControl(5, 4, 3, 1);
u-
// 愛心圖案（8x8）
byte heart[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

void setup() {
  lc.shutdown(0, false);   // 啟動 MAX7219
  lc.setIntensity(0, 8);   // 亮度 0~15
  lc.clearDisplay(0);      // 清除顯示
  display.setBrightness(0x0f);
}

void loop() {
  static byte buffer[16];  // 延伸圖像空間供捲動用
  static unsigned long lastUpdate = 0;
  const int delayTime = 150; // 每幀延遲時間（毫秒）

  // 準備動畫資料（heart 上面補空白）
  for (int i = 0; i < 8; i++) buffer[i] = 0x00;
  for (int i = 0; i < 8; i++) buffer[i + 8] = heart[i];

  // 執行由上往下滾動
  for (int shift = 0; shift <= 8; shift++) {
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, buffer[row + shift]);
    }
    delay(delayTime);
  }

//

   for (int i = 10; i > 0; i--) {
      // Hiển thị số hiện tại (i) trên màn hình
      display.showNumberDec(i);
      delay(1000); // Chờ 1 giây trước khi hiển thị số tiếp theo
   }

   // Hiển thị một số cụ thể, ví dụ, 1234
   // Số 1234 sẽ được hiển thị trên màn hình 7 đoạn
   display.showNumberDec(1234);

   // Chờ 2 giây để giữ số được hiển thị
   delay(2000);

   // Xóa màn hình
   // Điều này sẽ tắt tất cả các đoạn của màn hình 7 đoạn
   display.clear();

   // Chờ 1 giây trước khi bắt đầu lại vòng lặp
   delay(1000);

}
