# Prototype Suction Pump Otomatis

## Deskripsi Proyek
Proyek ini adalah rancang bangun purwarupa (prototype) *Suction Pump* medis pintar berbasis mikrokontroler. Sistem ini dirancang untuk meningkatkan higienitas pengoperasian dan keamanan pasien melalui fitur sakelar tanpa sentuh (*touchless control*) dan sistem pemutusan otomatis (*auto-cutoff*) terintegrasi saat tabung penampung penuh.

Program ini ditulis dengan arsitektur **State Machine** (FSM) dan metode **Non-Blocking Delay** menggunakan `millis()`, memastikan pembacaan sensor, aktuasi *relay*, dan alarm peringatan dapat berjalan secara bersamaan dan responsif.

## Fitur Utama
*   **Touchless Control:** Menyalakan dan mematikan pompa hisap cukup dengan mendekatkan tangan pada jarak < 20 cm menggunakan sensor Ultrasonik, menjaga higienitas operator (tenaga medis).
*   **Auto-Cutoff & Overflow Protection:** Pompa akan otomatis mati dan terkunci (tidak bisa dinyalakan) jika cairan mencapai batas maksimal (dideteksi melalui perubahan intensitas cahaya oleh sensor LDR pada ambang batas >= 850).
*   **Dual Audio Feedback:** Dilengkapi dua *buzzer* terpisah. Buzzer 1 memberikan bunyi *bip* pendek saat pompa dihidupkan/dimatikan, sedangkan Buzzer 2 memberikan bunyi alarm berulang saat tabung penuh.
*   **Serial Debugging:** Memudahkan pemantauan nilai jarak, tingkat cahaya, status relay, dan state sistem secara *real-time* melalui Serial Monitor.

## Pemetaan Pin (Pin Mapping)
Berikut adalah daftar pin yang digunakan pada mikrokontroler:

| Komponen                 | Pin Mikrokontroler | Keterangan                              |
| **Ultrasonic Trigger**   | Pin 2              | Output untuk memancarkan sinyal suara   |
| **Ultrasonic Echo**      | Pin 3              | Input untuk menerima pantulan suara     |
| **Relay Pompa**          | Pin 4              | Aktuator pompa vakum (*Active Low*)     |
| **Buzzer 1 (Indikator)** | Pin 9              | Bunyi *bip* tunggal saat relay *toggle* |
| **Buzzer 2 (Alarm)**     | Pin 10             | Bunyi alarm berulang saat *overflow*    |
| **Sensor LDR**           | Pin A0             | Input analog pendeteksi batas cairan    |

## Arsitektur State Machine
Sistem beroperasi dalam dua *state* utama:
1.  **SIAGA (Standby/Normal):** 
    Sistem membaca jarak dari sensor ultrasonik. Jika terdeteksi objek (tangan) pada jarak ≤ 20 cm, mikrokontroler akan membalik status (*toggle*) relay untuk menghidupkan atau mematikan pompa.
2.  **ALARM_PENUH (Overflow):** 
    Jika nilai LDR mencapai ≥ 850 (mengindikasikan cairan menutupi cahaya), sistem langsung berpindah ke mode ini. Relay dipaksa mati secara otomatis, dan Buzzer 2 akan berbunyi dengan interval 300ms. Sistem akan kembali ke state `SIAGA` hanya jika nilai LDR kembali normal (cairan telah dikosongkan).

## Konfigurasi Kustom
Kamu dapat menyesuaikan parameter sensitivitas perangkat dengan mengubah nilai variabel berikut pada kode sumber:
```cpp
const int BATAS_JARAK_AKTIVASI = 20;   // Jarak deteksi tangan (cm)
const int AMBANG_BATAS_CAHAYA = 850;   // Batas nilai LDR untuk memicu alarm
