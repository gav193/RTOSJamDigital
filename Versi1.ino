#include "Wire.h"
#include <LCD_I2C.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define priorityTask1 2
#define priorityTask2 2
#define priorityTask3 2

SemaphoreHandle_t xMutex;

LCD_I2C lcd(0x27, 16, 2);

void update_waktu(void *pvParam);
void tampilkan_waktu(void *pvParam);
void baca_tombol(void *pvParam);

int jam = 0, menit = 0, detik = 0;

#define btn_tambah_jam 2
#define btn_kurang_jam 3
#define btn_tambah_menit 4
#define btn_kurang_menit 5
#define btn_tambah_detik 6
#define btn_kurang_detik 7


void setup() {
  Serial.begin(9600);
  Wire.begin();

  lcd.begin();
  lcd.print("Jam Siap!");
  lcd.backlight();
  xMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(update_waktu, "Task 1", 2048, NULL, priorityTask1, NULL, 0);
  xTaskCreatePinnedToCore(tampilkan_waktu, "Task 2", 2048, NULL, priorityTask2, NULL, 0);
  xTaskCreatePinnedToCore(baca_tombol, "Task 3", 2048, NULL, priorityTask3, NULL 1);

  pinMode(btn_tambah_jam, INPUT_PULLUP);
  pinMode(btn_kurang_jam, INPUT_PULLUP);
  pinMode(btn_tambah_menit, INPUT_PULLUP);
  pinMode(btn_kurang_menit, INPUT_PULLUP);
  pinMode(btn_tambah_detik, INPUT_PULLUP);
  pinMode(btn_kurang_detik, INPUT_PULLUP);
}

void loop(){
  //loop dibiarkan kosong
}

void tampilkan_waktu(void *pvParam) {// wip
  (void) pvParam;
  while (1) {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Waktu: ");
      lcd.print(jam < 10 ? "0" : "");
      lcd.print(jam);
      lcd.print(":");
      lcd.print(menit < 10 ? "0" : "");
      lcd.print(menit);
      lcd.print(":");
      lcd.print(detik < 10 ? "0" : "");
      lcd.print(detik);
    }
    xSemaphoreGive(xMutex);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void update_waktu(void *pvParam) {
  (void) pvParam;
  while(1) {
    xSemaphoreTake(xMutex,portMAX_DELAY);
    {
      detik++;
      if (detik >= 60) {
        detik = 0;
        menit++;
        if (menit >= 60) {
          menit = 0;
          jam = (jam + 1) % 24;
        }
      }
    }
    xSemaphoreGive(xMutex);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void baca_tombol(void *pvParam) {
  if (digitalRead(btn_tambah_jam) == LOW) {
    jam = (jam + 1) % 24;
    delay(200);
  }
  if (digitalRead(btn_tambah_menit) == LOW) {
    menit += 1;
    if (menit >= 60) {
      menit %= 60;
    } 
    delay(200);
  }
  if (digitalRead(btn_tambah_detik) == LOW) {
    detik += 1;
    if (detik >= 60) {
      detik %= 60;
    }
    delay(200);
  }
  if (digitalRead(btn_kurang_jam) == LOW){
    jam = (jam==0) ? 23 : jam - 1;
    delay (200);
  }
  if (digitalRead(btn_kurang_menit) == LOW) {
    menit = (menit == 0) ? 59 : menit - 1;
    delay (200);
  }
  if(digitalRead(btn_kurang_detik) == LOW) {
    detik = (detik ==0) ? 59 : detik - 1;
    delay (200);
  }
}
