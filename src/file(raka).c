#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/user.h"
#include "../include/transaksi.h"
#include "../include/file.h"

#define FILE_USER_TMP       FILE_USER ".tmp"
#define FILE_TRANSAKSI_TMP  FILE_TRANSAKSI ".tmp"

/* Ganti karakter pemisah '|' agar format file tidak rusak */
static void bersihkan(char *s)
{
    for (; *s; s++) {
        if (*s == '|') *s = '/';
    }
}

/* Ganti file tujuan dengan file sementara (aman di Windows & Linux) */
static int gantiFile(const char *tmp, const char *tujuan)
{
    remove(tujuan);
    return rename(tmp, tujuan) == 0;
}

int simpanData(void)
{
    FILE *fu = fopen(FILE_USER_TMP, "w");
    FILE *ft = fopen(FILE_TRANSAKSI_TMP, "w");
    int i, ok;

    if (fu == NULL || ft == NULL) {
        printf("Gagal menyimpan data ke file.\n");
        if (fu) { fclose(fu); remove(FILE_USER_TMP); }
        if (ft) { fclose(ft); remove(FILE_TRANSAKSI_TMP); }
        return 0;
    }

    for (i = 0; i < jumlahUser; i++)
        fprintf(fu, "%s %s\n", daftarUser[i].username, daftarUser[i].password);

    for (i = 0; i < jumlahTransaksi; i++) {
        Transaksi t = daftarTransaksi[i];
        bersihkan(t.kategori);
        bersihkan(t.keterangan);
        fprintf(ft, "%d|%s|%d|%ld|%s|%s|%s\n",
                t.id, t.username, t.tipe, t.jumlah,
                t.kategori, t.keterangan, t.tanggal);
    }

    ok = (fclose(fu) == 0);
    ok = (fclose(ft) == 0) && ok;

    if (!ok || !gantiFile(FILE_USER_TMP, FILE_USER) ||
        !gantiFile(FILE_TRANSAKSI_TMP, FILE_TRANSAKSI)) {
        printf("Gagal menyimpan data ke file.\n");
        return 0;
    }
    return 1;
}

int muatData(void)
{
    FILE *fu = fopen(FILE_USER, "r");
    FILE *ft = fopen(FILE_TRANSAKSI, "r");
    char baris[256];
    int dilewati = 0;

    jumlahUser = 0;
    jumlahTransaksi = 0;

    if (fu != NULL) {
        while (jumlahUser < MAX_USER && fgets(baris, sizeof baris, fu)) {
            char u[PANJANG_NAMA], p[PANJANG_NAMA];
            if (sscanf(baris, "%29s %29s", u, p) == 2) {
                strcpy(daftarUser[jumlahUser].username, u);
                strcpy(daftarUser[jumlahUser].password, p);
                jumlahUser++;
            } else if (baris[strspn(baris, " \t\r\n")] != '\0') {
                dilewati++;
            }
        }
        fclose(fu);
    }

    if (ft != NULL) {
        while (jumlahTransaksi < MAX_TRANSAKSI && fgets(baris, sizeof baris, ft)) {
            Transaksi t;
            char *f[7];
            char *p = baris;
            int n = 0;

            baris[strcspn(baris, "\r\n")] = '\0';
            if (baris[0] == '\0') continue;

            /* pecah berdasarkan '|' (field kosong tetap dihitung) */
            f[n++] = p;
            while (*p && n < 7) {
                if (*p == '|') { *p = '\0'; f[n++] = p + 1; }
                p++;
            }
            if (n != 7) { dilewati++; continue; }

            memset(&t, 0, sizeof t);
            t.id     = atoi(f[0]);
            strncpy(t.username,   f[1], sizeof t.username - 1);
            t.tipe   = atoi(f[2]);
            t.jumlah = atol(f[3]);
            strncpy(t.kategori,   f[4], sizeof t.kategori - 1);
            strncpy(t.keterangan, f[5], sizeof t.keterangan - 1);
            strncpy(t.tanggal,    f[6], sizeof t.tanggal - 1);

            /* validasi isi: tipe harus 0/1 dan jumlah positif */
            if ((t.tipe != TRANSAKSI_MASUK && t.tipe != TRANSAKSI_KELUAR) ||
                t.jumlah <= 0 || t.username[0] == '\0') {
                dilewati++;
                continue;
            }

            daftarTransaksi[jumlahTransaksi++] = t;
        }
        fclose(ft);
    }

    if (dilewati > 0)
        printf("Peringatan: %d baris data rusak dilewati saat memuat file.\n", dilewati);
    return 1;
}
