#ifndef LAPORAN_H
#define LAPORAN_H

/* Perhitungan saldo (per username) */
long hitungTotalMasuk(const char *username);
long hitungTotalKeluar(const char *username);
long hitungSaldo(const char *username);

/* Menampilkan saldo pengguna yang sedang login */
void lihatSaldo(void);

/* Ringkasan keuangan + rincian pengeluaran per kategori */
void laporanKeuangan(void);

/* Rekap pemasukan, pengeluaran, dan saldo per bulan */
void laporanBulanan(void);

#endif
