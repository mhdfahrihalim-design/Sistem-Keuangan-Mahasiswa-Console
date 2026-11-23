#ifndef MENU_H
#define MENU_H

#define INPUT_HABIS (-2)   /* EOF: input sudah habis (mis. Ctrl+D) */

/* Membaca satu pilihan menu (angka) dengan aman.
Mengembalikan -1 jika input bukan angka, INPUT_HABIS jika EOF. */
int bacaPilihan(void);

/* Menu sebelum login: Registrasi / Login / Keluar */
void menuAwal(void);

/* Menu setelah login (dijalankan sampai pengguna logout) */
void menuUtama(void);

/* Titik masuk aplikasi: muat data, jalankan menu, simpan data */
void jalankanAplikasi(void);

#endif
