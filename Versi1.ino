#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// RTOS Task Priority Macro Definitions
#define priorityTask1 2
#define priorityTask2 2
#define priorityTask3 1

SemaphoreHandle_t xMutex; // init mutex for RTOS
LiquidCrystal_I2C lcd(0x27, 16, 2); // create LCD object
int jam = 0, menit = 0, detik = 0; // init time variables
// function definitions
void update_waktu(void *pvParam);
void tampilkan_waktu(void *pvParam);
void tambah_jam(void *pvParam);
void kurang_jam(void *pvParam);
void tambah_menit(void *pvParam);
void kurang_menit(void *pvParam);
void tambah_detik(void *pvParam);
void kurang_detik(void *pvParam);
// button pin macro definitions
#define btn_tambah_jam 12
#define btn_kurang_jam 13
#define btn_tambah_menit 14
#define btn_kurang_menit 15
#define btn_tambah_detik 16
#define btn_kurang_detik 17

void setup() {
  Serial.begin(9600); // init serial communication
  Wire.begin(); // init i2c connection
  lcd.init(); // start LCD
  lcd.print("Jam Siap!");
  lcd.backlight();
  // setup button pin modes to internal pullup
  pinMode(btn_tambah_jam, INPUT_PULLUP); 
  pinMode(btn_kurang_jam, INPUT_PULLUP);
  pinMode(btn_tambah_menit, INPUT_PULLUP);
  pinMode(btn_kurang_menit, INPUT_PULLUP);
  pinMode(btn_tambah_detik, INPUT_PULLUP);
  pinMode(btn_kurang_detik, INPUT_PULLUP);

  xMutex = xSemaphoreCreateMutex(); // create mutex object
  if (xMutex == NULL) { // mutex debugging printout
    Serial.println("Mutex creation failed!");
    while (1);
  }
  // assign lcd and time increment task to core 0
  xTaskCreatePinnedToCore(update_waktu, "Task 1", 2048, NULL, priorityTask1, NULL,0 );
  xTaskCreatePinnedToCore(tampilkan_waktu, "Task 2", 2048, NULL, priorityTask2, NULL,0);
  // assign button task to core 1
  xTaskCreatePinnedToCore(tambah_jam, "Tambah Jam", 1024, NULL, priorityTask3, NULL,1);
  xTaskCreatePinnedToCore(kurang_jam, "Kurang Jam", 1024, NULL, priorityTask3, NULL,1);
  xTaskCreatePinnedToCore(tambah_menit, "Tambah Menit", 1024, NULL, priorityTask3, NULL,1);
  xTaskCreatePinnedToCore(kurang_menit, "Kurang Menit", 1024, NULL, priorityTask3, NULL,1);
  xTaskCreatePinnedToCore(tambah_detik, "Tambah Detik", 1024, NULL, priorityTask3, NULL,1);
  xTaskCreatePinnedToCore(kurang_detik, "Kurang Detik", 1024, NULL, priorityTask3, NULL,1);
}

void loop() {
  // empty loop (functionality from RTOS)
}

void tampilkan_waktu(void *pvParam) { // function for displaying LCD
  while (1) {
    Serial.println("Updating LCD...");
    xSemaphoreTake(xMutex, portMAX_DELAY); // receive mutex to start task
    { // display time information
      lcd.clear(); 
      lcd.setCursor(0, 0);
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
    xSemaphoreGive(xMutex); // return mutex for next task
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1s (to avoid flickering)
  }
}

void update_waktu(void *pvParam) { // function to increment time
  while (1) {
    xSemaphoreTake(xMutex, portMAX_DELAY); // receive mutex to start task
    {
      detik++; // increment second
      if (detik >= 60) { // check for second overflow
        detik = 0;
        menit++; // if overflow, increment minute
        if (menit >= 60) { // check for minute overflow
          menit = 0;
          jam = (jam + 1) % 24; // if overflow, increment hour
        }
      }
    }
    xSemaphoreGive(xMutex); // return mutex for next task
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1s (to simulate real-time clock)
  }
}

void tambah_jam(void *pvParam) { // function to increase hour var
  while (1) {
    if (digitalRead(btn_tambah_jam) == LOW) { // check button input
      xSemaphoreTake(xMutex, portMAX_DELAY); // ensure safety with mutex (no other tasks are ran)
      jam = (jam + 1) % 24; // increment hour, discard overflow
      xSemaphoreGive(xMutex); // return mutex
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce delay
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // polling delay (if button not read)
  }
}

void kurang_jam(void *pvParam) { // function to decrease hour var
  while (1) {
    if (digitalRead(btn_kurang_jam) == LOW) { // check button input
      xSemaphoreTake(xMutex, portMAX_DELAY); // ensure safety with mutex (no other tasks are ran)
      jam = (jam == 0) ? 23 : jam - 1; // decrement hour, normalize value if negative
      xSemaphoreGive(xMutex); // return mutex
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce delay
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // polling delay (if button not read)
  }
}

void tambah_menit(void *pvParam) { // function to increase minute var
  while (1) {
    if (digitalRead(btn_tambah_menit) == LOW) { // check button input
      xSemaphoreTake(xMutex, portMAX_DELAY); // ensure safety with mutex
      menit = (menit + 1) % 60; // increment minute, discard overflow
      xSemaphoreGive(xMutex); // return mutex
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce delay
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // polling delay (if button not read)
  }
}

void kurang_menit(void *pvParam) { // function to decrease minute var
  while (1) {
    if (digitalRead(btn_kurang_menit) == LOW) { // check button input
      xSemaphoreTake(xMutex, portMAX_DELAY); // ensure safety 
      menit = (menit == 0) ? 59 : menit - 1; // decrement minute, normalize value
      xSemaphoreGive(xMutex); // return mutex
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce delay
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // polling delay (if button not read)
  }
}

void tambah_detik(void *pvParam) { // function to increase seconds var
  while (1) {
    if (digitalRead(btn_tambah_detik) == LOW) { // check button input
      xSemaphoreTake(xMutex, portMAX_DELAY); // ensure safety
      detik = (detik + 1) % 60; // increment seconds, discard overflow
      xSemaphoreGive(xMutex); // return mutex
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce delay
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // polling delay (if button not read)
  }
}

void kurang_detik(void *pvParam) { // function to decrease seconds var
  while (1) {
    if (digitalRead(btn_kurang_detik) == LOW) { // check button input 
      xSemaphoreTake(xMutex, portMAX_DELAY); // ensure safety
      detik = (detik == 0) ? 59 : detik - 1; // decrement seconds, normalize value
      xSemaphoreGive(xMutex); // return mutex
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce delay
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // polling delay (if button not read)
  }
}
