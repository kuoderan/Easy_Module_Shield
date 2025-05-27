#include <TM1637Display.h>   // 導入 TM1637 七段顯示器函式庫
#include <LedControl.h>      // 導入 MAX7219 8x8 LED 陣列函式庫
#include <Adafruit_NeoPixel.h> // 導入 WS2812 LED 函式庫

// --- TM1637 七段顯示器設定 ---
#define CLK_TM1637 2 // CLK 連接到 Arduino 的 D2
#define DIO_TM1637 3 // DIO 連接到 Arduino 的 D3
TM1637Display display(CLK_TM1637, DIO_TM1637); // 創建 TM1637Display 物件
int count = 99; // 設定倒數的起始數字

// --- MAX7219 8x8 LED 陣列設定 ---
// 定義 MAX7219 引腳:
// DIN (Data In): D12
// CLK (Clock): D11
// CS (Chip Select): D10
// 串聯的 MAX7219 模組數量: 1
LedControl lc = LedControl(12, 11, 10, 1);

// 愛心圖案的位元組陣列 (8x8 矩陣)
byte heart[8] = {
  0b00000000, // 第0行 (最上面)
  0b01100110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000  // 第7行 (最下面)
};

// 愛心垂直捲動的偏移量
// 為了從底部開始往上移動，我們需要讓 scrollOffset 從負值開始
// 初始值為 -8 (或任何小於 0 的值)，這樣愛心會完全在顯示器下方
int scrollOffset = -8; // 愛心從顯示器外下方開始向上移動的偏移量

// --- WS2812 LED 設定 ---
#define LED_PIN 6        // WS2812 的 DIN 引腳連接到 Arduino 的 D6
#define NUM_LEDS 6       // 你有 6 顆 WS2812 LED
Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); // 創建 NeoPixel 物件

// 記錄 WS2812 LED 當前狀態，避免重複設置
int prevCountDigit = -1; // 用於記錄上次的個位數

// --- 可變電阻設定 ---
#define POT_PIN A0       // 可變電阻連接到 Arduino 的 A0

void setup() {
  Serial.begin(9600); // 啟用序列埠通訊，用於除錯 (可選)

  // --- TM1637 設定 ---
  display.setBrightness(0x0a); // 設定顯示器亮度
  display.showNumberDec(count); // 顯示初始數字 99

  // --- MAX7219 設定 ---
  lc.shutdown(0, false); // 喚醒裝置 (從省電模式)
  lc.setIntensity(0, 8); // 設定亮度 (0-15)
  lc.clearDisplay(0);    // 清除顯示器

  // --- WS2812 設定 ---
  pixels.begin(); // 初始化 NeoPixel 庫
  pixels.clear(); // 清除所有 LED，確保初始狀態為熄滅
  pixels.show();  // 將清除的狀態發送到 LED
}

void loop() {
  // --- TM1637 七段顯示器倒數邏輯 ---
  static unsigned long lastCountTime = 0;
  const long interval = 1000;

  if (millis() - lastCountTime >= interval) {
    lastCountTime = millis();
    count--;

    if (count < 0) {
      count = 99;
      display.clear();
      delay(500);
    }
    display.showNumberDec(count);
    Serial.print("Count: ");
    Serial.println(count);

    // --- WS2812 LED 亮度調整邏輯 ---
    int potValue = analogRead(POT_PIN);
    int brightness = map(potValue, 0, 1023, 64, 255);
    pixels.setBrightness(brightness);
    Serial.print("Pot Value: ");
    Serial.print(potValue);
    Serial.print(", Brightness: ");
    Serial.println(brightness);

    // --- WS2812 LED 顯示邏輯 (根據計數器個位數) ---
    int currentCountDigit = count % 10;

    if (currentCountDigit == 0) {
      rainbowCycle(20);
    } else if (currentCountDigit == 5) {
      pixels.clear();
      pixels.show();
    } else {
      pixels.clear();
      pixels.show();
    }
    prevCountDigit = currentCountDigit;
  }

  // --- MAX7219 8x8 LED 陣列愛心動畫邏輯 (從底部向上移動) ---
  lc.clearDisplay(0); // 每次更新前先清空顯示，避免殘影

  for (int row = 0; row < 8; row++) {
    // 這裡的邏輯是將愛心圖案的每一行，根據 scrollOffset 向上偏移
    // heart[row] 是愛心圖案的原始行資料
    // 把它顯示在顯示器的 (row + scrollOffset) 行
    int displayRow = row + scrollOffset; // 計算原始愛心圖案的 `row` 應該顯示在顯示器的哪一行

    if (displayRow >= 0 && displayRow < 8) { // 檢查 `displayRow` 是否在顯示器有效範圍內
      lc.setRow(0, displayRow, heart[row]); // 將 `heart[row]` 顯示在顯示器的 `displayRow` 上
    }
  }

  scrollOffset++; // 增加偏移量，使愛心向上移動 (從負值逐漸變為正值)

  // 當 scrollOffset 達到 8 時，表示愛心已經完全移出頂部，重置到 -8 讓它從底部重新開始
  if (scrollOffset >= 8) {
    scrollOffset = -8; // 重置偏移量，讓愛心從顯示器外下方重新開始
  }

  // 調整愛心動畫的速度
  delay(100);
}

// --- WS2812 函式庫需要的彩虹效果函式 ---
// HUE 輪 (0-65535) 轉換為 RGB 顏色
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// WS2812 彩虹循環效果函式
void rainbowCycle(uint8_t wait) {
  for(uint16_t j=0; j<256; j++) {
    for(uint16_t i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
  }
}