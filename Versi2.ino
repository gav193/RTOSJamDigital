#include "Wire.h"
#include <LCD_I2C.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <TimerOne.h>

// declare priority of tasks for RTOS
#define priorityTask1 2
#define priorityTask2 2
// declare mutex for task starting/stopping
SemaphoreHandle_t xMutex;
// create LCD object
LCD_I2C lcd(0x27, 16, 2);
// RTOS function declarations
void update_waktu(void *pvParam);
void tampilkan_waktu(void *pvParam);
// declare time variables
int jam = 0, menit = 0, detik = 0;
// declare button pin macros
#define btn_tambah_jam 2
#define btn_kurang_jam 3
#define btn_tambah_menit 4
#define btn_kurang_menit 5
#define btn_tambah_detik 6
#define btn_kurang_detik 7

void setup() {
  Serial.begin(9600); // serial comm init
  Wire.begin(); // i2c connection init

  lcd.begin(); // startup lcd
  lcd.print("Jam Siap!"); 
  lcd.backlight();
  
  xMutex = xSemaphoreCreateMutex(); // mutex init
  xTaskCreatePinnedToCore(update_waktu, "Task 1", 2048, NULL, priorityTask1, NULL, 0); // assign update_waktu to CPU 0 with priorityTask1
  xTaskCreatePinnedToCore(tampilkan_waktu, "Task 2", 2048, NULL, priorityTask2, NULL, 1); // assign tampilkan_waktu to CPU 1 with priorityTask2
  // button assignments
  pinMode(btn_tambah_jam, INPUT_PULLUP);
  pinMode(btn_kurang_jam, INPUT_PULLUP);
  pinMode(btn_tambah_menit, INPUT_PULLUP);
  pinMode(btn_kurang_menit, INPUT_PULLUP);
  pinMode(btn_tambah_detik, INPUT_PULLUP);
  pinMode(btn_kurang_detik, INPUT_PULLUP);
  // button intetrrupt init
  attachInterrupt(digitalPinToInterrupt(btn_tambah_jam), tambah_jam, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn_kurang_jam), kurang_jam, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn_tambah_menit), tambah_menit, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn_kurang_menit), kurang_menit, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn_tambah_detik), tambah_detik, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn_kurang_detik), kurang_detik, FALLING);
}

void loop() {
  // empty loop (process runs with RTOS and interrupt)
}

void tampilkan_waktu(void *pvParam) { // function for display
  (void) pvParam;
  while (1) {
    xSemaphoreTake(xMutex, portMAX_DELAY); // take xMutex when available (other tasks not ran)
    {
      lcd.clear(); // display time info
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
    xSemaphoreGive(xMutex); // return xMutex to run other tasks
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay for 1s 
  }
}

void update_waktu(void *pvParam) { // function for time increment
  (void) pvParam;
  while(1) {
    xSemaphoreTake(xMutex,portMAX_DELAY); // take xMutex when available (other tasks not ran)
    {
      detik++; // increment second
      if (detik >= 60) { // check for second overflow
        detik = 0;
        menit++; // if overflow, reset second -> increment minute
        if (menit >= 60) { // check for minute overflow
          menit = 0;
          jam = (jam + 1) % 24; // if overflow, reset minute -> increment hour
        }
      }
    }
    xSemaphoreGive(xMutex); // return xMutex to run other tasks
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay for 1s
  }
}
// interrupt functions to change time info
void tambah_jam() {
  jam = (jam + 1) % 24;
}

void tambah_menit() {
    menit += 1;
    if (menit >= 60) {
      menit %= 60;
    } 
}

void tambah_detik() {
  detik += 1;
  if (detik >= 60) {
    detik %= 60;
  }
}

void kurang_jam() {
  jam = (jam==0) ? 23 : jam - 1;
}

void kurang_menit() {
  menit = (menit == 0) ? 59 : menit - 1;
}

void kurang_detik() {  
  detik = (detik ==0) ? 59 : detik - 1;
}
