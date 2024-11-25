# RTOSJamDigital

# Tugas Sistem Mikroprosesor #3: RTOS

Kode pada tugas ini dibuat untuk compiler Arduino IDE menggunakan bahasa C-Lite.

## 1. Jam Digital Dengan RTOS
**Deskripsi Masalah:**
Sebuah jam digital yang menampilkan jam : menit : detik dengan display LCD (I2C) dirancang 
dengan tambahan input tombol untuk mengubah jam, menit, dan detiknya. Update waktu dan pembacaan
input tombol pada program dilakukan dengan menggunakan RTOS.

**Spesifikasi Kode:**
- Pembagian Core RTOS
Kode menggunakan pembagian core pada RTOS menjadi core 0 dan core 1. Pada Core 1, dilakukan task 
update_waktu dan tampilkan_waktu yang secara berturut melakukan inkrementasi waktu serta menjalankan tampilan pada LCD I2C. Selain itu, diberikan prioritas '2' pada core 0 dan prioritas '1' pada core 1 sehingga task pada core 1 lebih diutamakan (pembacaan input).

- Mutex Assignment
Program yang dirancang menggunakan mutex untuk memastikan tidak ada task yang tabrakan. Oleh karena itu, perubahan nilai variabel yang saling digunakan antar-fungsi akan selalu diawali menggunakan pemeriksaan availability mutex.
--------------

Sistem wiring tombol menggunakan mode **INPUT_PULLUP**, sehingga tombol dihubungkan ke ground, dan ketika tombol ditekan, sinyal berubah menjadi LOW.
