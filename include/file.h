#ifndef FILE_H
#define FILE_H

#define FILE_USER       "data_user.txt"
#define FILE_TRANSAKSI  "data_transaksi.txt"

/* Simpan seluruh data user & transaksi ke file teks.
Ditulis ke file sementara dulu, sehingga file lama tidak rusak jika gagal.
Mengembalikan 1 jika berhasil, 0 jika gagal. */
int simpanData(void);

/* Muat data dari file teks (aman jika file belum ada).
Baris yang rusak/tidak valid dilewati dan dilaporkan.
Mengembalikan 1 jika selesai. */
int muatData(void);

#endif
