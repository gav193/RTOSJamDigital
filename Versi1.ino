#include "Wire.h"
#include <LCD_I2C.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// task priority definition
#define priorityTask1 2
#define priorityTask2 2
#define priorityTask3 2
// mutex init for RTOS
SemaphoreHandle_t xMutex;
// LCD object init
LCD_I2C lcd(0x27, 16, 2);
// RTOS function init
void update_waktu(void *pvParam);
void tampilkan_waktu(void *pvParam);
void baca_tombol(void *pvParam);
// variable for time 
int jam = 0, menit = 0, detik = 0;
// button pin macro definitions
#define btn_tambah_jam 2
#define btn_kurang_jam 3
#define btn_tambah_menit 4
#define btn_kurang_menit 5
#define btn_tambah_detik 6
#define btn_kurang_detik 7

void setup() {
  Serial.begin(9600); // serial comm init
  Wire.begin(); // i2c connection init

  lcd.begin(); // lcd startup
  lcd.print("Jam Siap!");
  lcd.backlight();
  
  xMutex = xSemaphoreCreateMutex(); // mutex init
  // RTOS function init
  xTaskCreatePinnedToCore(update_waktu, "Task 1", 2048, NULL, priorityTask1, NULL, 0);
  xTaskCreatePinnedToCore(tampilkan_waktu, "Task 2", 2048, NULL, priorityTask2, NULL, 0);
  xTaskCreatePinnedToCore(baca_tombol, "Task 3", 2048, NULL, priorityTask3, NULL 1);
  // button pin init
  pinMode(btn_tambah_jam, INPUT_PULLUP);
  pinMode(btn_kurang_jam, INPUT_PULLUP);
  pinMode(btn_tambah_menit, INPUT_PULLUP);
  pinMode(btn_kurang_menit, INPUT_PULLUP);
  pinMode(btn_tambah_detik, INPUT_PULLUP);
  pinMode(btn_kurang_detik, INPUT_PULLUP);
}

void loop(){
  //loop dibiarkan kosong (fungsionalitas program pada RTOS)
}

void tampilkan_waktu(void *pvParam) { // fungsi untuk membaca tombol
  (void) pvParam;
  while (1) {
    xSemaphoreTake(xMutex, portMAX_DELAY); // take xMutex untuk memulai task
    {
      lcd.clear(); // display informasi waktu pada lcd
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
    xSemaphoreGive(xMutex); // return mutex untuk menjalankan task lain
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1s
  }
}

void update_waktu(void *pvParam) { // fungsi untuk increment waktu
  (void) pvParam;
  while(1) { 
    xSemaphoreTake(xMutex,portMAX_DELAY); // take xMutex untuk memulai task 
    {
      detik++; // increment second
      if (detik >= 60) { // check seconds for overflow
        detik = 0;
        menit++; // if overflow, increment minute
        if (menit >= 60) { // check minute for overflow
          menit = 0;
          jam = (jam + 1) % 24; // if overflow, increment hour
        }
      }
    }
    xSemaphoreGive(xMutex); // return xMutex unutk menjalankan task lain 
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1s
  }
}

void baca_tombol(void *pvParam) {  // fungsi untuk membaca tombol
  (void) pvParam;
  while (1) { 
    xSemaphoreTake(xMutex, portMAX_DELAY); // take xMutex untuk memulai task
    { // check each button input
      if (digitalRead(btn_tambah_jam) == LOW) { 
        jam = (jam + 1) % 24; 
      }
      if (digitalRead(btn_tambah_menit) == LOW) {
        menit += 1;
        if (menit >= 60) {
          menit %= 60;
        }
      }
      if (digitalRead(btn_tambah_detik) == LOW) {
        detik += 1;
        if (detik >= 60) {
          detik %= 60;
        }
      }
      if (digitalRead(btn_kurang_jam) == LOW){
        jam = (jam==0) ? 23 : jam - 1;
      }
      if (digitalRead(btn_kurang_menit) == LOW) {
        menit = (menit == 0) ? 59 : menit - 1;
      }
      if(digitalRead(btn_kurang_detik) == LOW) {
        detik = (detik ==0) ? 59 : detik - 1;
      }
    }
    xSemaphoreGive(xMutex); // return xMutex untuk menjalankan task lain
    vTaskDelay(pdMS_TO_TICKS(200)); // delay 200ms
  }
}
