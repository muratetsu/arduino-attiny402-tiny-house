// ATtiny402 Tiny House Light Control
// LEDをランダムに順次点灯し，一定時間後に順次消灯する
//
// June 17, 2024
// Tetsu Nishimura
//
// ATtiny402のポート番号とIO番号の対応
// PA0 - D5, Reset/UPDI
// PA1 - D2, PWM, LED Control
// PA2 - D3, PWM, LED Control
// PA3 - D4, PWM, LED Control
// PA6 - D0, GPIO, DCDC Control
// PA7 - D1, PWM, LED Control
//
// 書き込み装置として使うArduino Unoにあらかじめjtag2updi.inoを書き込んでおく
//
// 書き込み時の設定
// ボード: "megaTinyCore-ATtiny412/402/212/202"
// ポート: Arduino UNOが接続されているポートを選択
// Chip : "ATtiny402"
// 書き込み装置: jtag2updi
//
// "スケッチ - 書き込み装置を使って書き込み" で書き込み
// 書き込みの時は電源ボタンを押しっぱなしにすること
// 電池は抜いておくこと

// Definitions
#define GPIO_DCDC          0 // DCDC Control
#define GPIO_RND_SEED      2 // ADC Sampling for Random Seed

const char leds[] = {1, 2, 3, 4};
#define NUM_LEDS     sizeof(leds)

typedef enum {
  FADEIN,
  FADEOUT
} fadeMode_t;


// Functions

void fadein(int gpio) {
  for (int n = 0; n < 256; n++) {
    analogWrite(gpio, n);
    delay(6);
  }
}

void fadeout(int gpio) {
  for (int n = 255; n >= 0; n--) {
    analogWrite(gpio, n);
    delay(12);
  }
}

void setRandomSeed(void) {
  unsigned long seed;

  pinMode(GPIO_RND_SEED, INPUT);

  for (int n = 0; n < 4; n++) {
    seed <<= 8;
    seed |= analogRead(GPIO_RND_SEED) & 0xff;
    // Wait a while to get a different value from previous one
    delay(10);
  }

  randomSeed(seed);
}

void selectRandom(fadeMode_t mode) {
  int state = 0;

  while(true) {
    int num = random(NUM_LEDS);
    int bitMask = 1 << num;

    if ((state & bitMask) == 0) {
      if (mode == FADEIN) {
        fadein(leds[num]);
      }
      else {
        fadeout(leds[num]);
      }
      state |= bitMask;

      // If all bits are set
      if (state >= (1 << NUM_LEDS) - 1) {
        break;
      }
    }
    else {
      delay(500);
    }
  }
}

void setup() {
  setRandomSeed();
  pinMode(GPIO_DCDC, OUTPUT);

  for (unsigned int n = 0; n < NUM_LEDS; n++) {
    pinMode(leds[n], OUTPUT);
  }

  digitalWrite(GPIO_DCDC, HIGH);  // Enable DCDC
}

void loop() {
  delay(1000);
  selectRandom(FADEIN);
  delay(8000);
  selectRandom(FADEOUT);

  digitalWrite(GPIO_DCDC, LOW); // Disable DCDC
  while(1);
}
