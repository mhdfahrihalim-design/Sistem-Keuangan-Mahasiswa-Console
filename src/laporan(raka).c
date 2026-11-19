#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"
#include "transaksi.h"
#include "laporan.h"

#define MAX_BULAN 120

/* Bandingkan dua kunci "MM-YYYY" berdasarkan tahun lalu bulan */
static int bandingBulan(const char *a, const char *b)
{
    int ta = atoi(a + 3), tb = atoi(b + 3);
    int ba = atoi(a),     bb = atoi(b);
    if (ta != tb) return ta - tb;
    return ba - bb;
}

void laporanKeuangan(void)
{
    const char *user = getUsernameAktif();
    int i, jumlah = 0;

    if (user == NULL) {
        printf("Silakan login terlebih dahulu.\n");
        return;
    }

    for (i = 0; i < jumlahTransaksi; i++) {
        if (strcmp(daftarTransaksi[i].username, user) == 0)
            jumlah++;
    }

    printf("\n=== LAPORAN KEUANGAN ===\n");
    printf("Pengguna          : %s\n", user);
    printf("Jumlah transaksi  : %d\n", jumlah);
    printf("Total pemasukan   : Rp %ld\n", hitungTotalMasuk(user));
    printf("Total pengeluaran : Rp %ld\n", hitungTotalKeluar(user));
    printf("Saldo akhir       : Rp %ld\n", hitungSaldo(user));
}

void laporanBulanan(void)
{
    const char *user = getUsernameAktif();
    char kunci[MAX_BULAN][8];
    long masuk[MAX_BULAN], keluar[MAX_BULAN];
    int jumlahBulan = 0;
    int i, j, idx;

    if (user == NULL) {
        printf("Silakan login terlebih dahulu.\n");
        return;
    }

    for (i = 0; i < jumlahTransaksi; i++) {
        const Transaksi *t = &daftarTransaksi[i];
        char key[8];

        if (strcmp(t->username, user) != 0) continue;

        strncpy(key, t->tanggal + 3, 7);   /* ambil MM-YYYY */
        key[7] = '\0';

        idx = -1;
        for (j = 0; j < jumlahBulan; j++) {
            if (strcmp(kunci[j], key) == 0) { idx = j; break; }
        }
        if (idx == -1) {
            if (jumlahBulan >= MAX_BULAN) continue;
            idx = jumlahBulan++;
            strcpy(kunci[idx], key);
            masuk[idx] = 0;
            keluar[idx] = 0;
        }

        if (t->tipe == TRANSAKSI_MASUK) masuk[idx]  += t->jumlah;
        else                            keluar[idx] += t->jumlah;
    }

    printf("\n=== LAPORAN BULANAN ===\n");
    if (jumlahBulan == 0) {
        printf("(belum ada transaksi)\n");
        return;
    }

    /* urutkan dari bulan terlama (bubble sort sederhana) */
    for (i = 0; i < jumlahBulan - 1; i++) {
        for (j = 0; j < jumlahBulan - 1 - i; j++) {
            if (bandingBulan(kunci[j], kunci[j + 1]) > 0) {
                char tmpKey[8];
                long tmp;
                strcpy(tmpKey, kunci[j]);
                strcpy(kunci[j], kunci[j + 1]);
                strcpy(kunci[j + 1], tmpKey);
                tmp = masuk[j];  masuk[j]  = masuk[j + 1];  masuk[j + 1]  = tmp;
                tmp = keluar[j]; keluar[j] = keluar[j + 1]; keluar[j + 1] = tmp;
            }
        }
    }

    printf("%-9s %-14s %-14s %-14s\n", "Bulan", "Masuk(Rp)", "Keluar(Rp)", "Selisih(Rp)");
    printf("---------------------------------------------------\n");
    for (i = 0; i < jumlahBulan; i++) {
        printf("%-9s %-14ld %-14ld %-14ld\n",
               kunci[i], masuk[i], keluar[i], masuk[i] - keluar[i]);
    }
}