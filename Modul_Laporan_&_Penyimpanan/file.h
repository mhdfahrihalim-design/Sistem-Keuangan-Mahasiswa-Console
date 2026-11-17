#ifndef FILE_H
#define FILE_H

#define FILE_USER       "data_user.txt"
#define FILE_TRANSAKSI  "data_transaksi.txt"

/* Simpan seluruh data user & transaksi ke file teks */
int simpanData(void);

/* Muat data dari file teks (aman jika file belum ada) */
int muatData(void);

#endif