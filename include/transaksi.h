#ifndef TRANSAKSI_H
#define TRANSAKSI_H

#include "user.h"

#define MAX_TRANSAKSI       500
#define PANJANG_KATEGORI    30
#define PANJANG_KETERANGAN  60
#define PANJANG_TANGGAL     11      /* DD-MM-YYYY + '\0' */

#define TRANSAKSI_MASUK     0
#define TRANSAKSI_KELUAR    1

typedef struct {
    int    id;
    char   username[PANJANG_NAMA];  /* pemilik transaksi */
    int    tipe;                    /* TRANSAKSI_MASUK / TRANSAKSI_KELUAR */
    long   jumlah;                  /* dalam rupiah */
    char   kategori[PANJANG_KATEGORI];
    char   keterangan[PANJANG_KETERANGAN];
    char   tanggal[PANJANG_TANGGAL]; /* DD-MM-YYYY */
} Transaksi;

/* Data global (dipakai juga oleh file.c dan laporan.c) */
extern Transaksi daftarTransaksi[MAX_TRANSAKSI];
extern int       jumlahTransaksi;

/* Semua fungsi di bawah bekerja untuk pengguna yang sedang login */
void tambahUangMasuk(void);
void tambahPengeluaran(void);
void riwayatTransaksi(void);
void ubahTransaksi(void);

/* lihatSaldo() dan perhitungan saldo ada di laporan.h (modul Laporan & Penyimpanan) */

#endif
