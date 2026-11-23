#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "transaksi.h"
#include "laporan.h"

#define MAKS_JUMLAH      1000000000L   /* batas wajar satu transaksi: Rp 1 miliar */
#define MAKS_PERCOBAAN   3             /* kesempatan mengulang input yang salah */

Transaksi daftarTransaksi[MAX_TRANSAKSI];
int       jumlahTransaksi = 0;

/* ---------- fungsi bantu (private) ---------- */

/* Membaca satu baris. Mengembalikan 0 jika input habis (EOF), 1 jika berhasil. */
static int bacaString(const char *prompt, char *buf, int ukuran)
{
    size_t len;
    int c;

    printf("%s", prompt);
    if (fgets(buf, ukuran, stdin) == NULL) {
        buf[0] = '\0';
        return 0;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    /* buang '\r' di akhir (jika ada) */
    len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\r' || buf[len - 1] == ' ' || buf[len - 1] == '\t'))
        buf[--len] = '\0';
    return 1;
}

/* Membaca jumlah rupiah. Format yang diterima:
   50000, 50.000, 50,000, Rp 50.000, Rp. 50.000
   Pemisah ribuan harus tepat 3 digit di belakangnya, sehingga "50.5"
   ditolak (bukan dianggap Rp 505). Mengembalikan 1 jika valid. */
static int parseJumlah(const char *s, long *hasil)
{
    long nilai = 0;
    int digitSejak = 0;     /* jumlah digit sejak pemisah terakhir */
    int adaPemisah = 0;
    int adaDigit = 0;

    while (*s == ' ' || *s == '\t') s++;
    if ((s[0] == 'R' || s[0] == 'r') && (s[1] == 'P' || s[1] == 'p')) {
        s += 2;
        if (*s == '.') s++;
        while (*s == ' ') s++;
    }

    for (; *s; s++) {
        if (isdigit((unsigned char)*s)) {
            int d = *s - '0';
            if (nilai > (MAKS_JUMLAH - d) / 10) return 0;   /* terlalu besar */
            nilai = nilai * 10 + d;
            digitSejak++;
            adaDigit = 1;
        } else if (*s == '.' || *s == ',') {
            if (digitSejak == 0) return 0;
            if (adaPemisah ? digitSejak != 3 : digitSejak > 3) return 0;
            adaPemisah = 1;
            digitSejak = 0;
        } else {
            return 0;
        }
    }

    if (!adaDigit || nilai <= 0) return 0;
    if (adaPemisah && digitSejak != 3) return 0;
    *hasil = nilai;
    return 1;
}

static int tahunKabisat(int t)
{
    return (t % 4 == 0 && t % 100 != 0) || (t % 400 == 0);
}

/* Memeriksa tanggal. Mengembalikan 1 jika d/m/y adalah tanggal yang ada. */
static int tanggalAda(int d, int m, int y)
{
    int hari[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (m < 1 || m > 12 || y < 1900 || y > 2100) return 0;
    if (m == 2 && tahunKabisat(y)) hari[1] = 29;
    return d >= 1 && d <= hari[m - 1];
}

/* Mengubah input tanggal menjadi format baku DD-MM-YYYY (disimpan di keluar,
   ukuran minimal PANJANG_TANGGAL).
   Diterima: 23-11-2026, 23/11/2026, 3-1-2026. Input kosong = tanggal hari ini.
   Mengembalikan 1 jika valid. */
static int normalisasiTanggal(const char *masuk, char *keluar)
{
    int d, m, y;
    char s1, s2, sisa;

    if (masuk[0] == '\0') {
        time_t sekarang = time(NULL);
        struct tm *lt = localtime(&sekarang);
        if (lt == NULL) return 0;
        d = lt->tm_mday;
        m = lt->tm_mon + 1;
        y = lt->tm_year + 1900;
    } else {
        if (sscanf(masuk, "%d%c%d%c%d%c", &d, &s1, &m, &s2, &y, &sisa) != 5)
            return 0;
        if ((s1 != '-' && s1 != '/') || (s2 != '-' && s2 != '/'))
            return 0;
    }

    if (!tanggalAda(d, m, y)) return 0;
    snprintf(keluar, PANJANG_TANGGAL, "%02d-%02d-%04d", d, m, y);
    return 1;
}

static int idBaru(void)
{
    int i, maks = 0;
    for (i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].id > maks) maks = daftarTransaksi[i].id;
    }
    return maks + 1;
}

/* Cari indeks transaksi berdasarkan id, hanya milik user tertentu */
static int cariIndeksById(int id, const char *username)
{
    int i;
    for (i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].id == id &&
            strcmp(daftarTransaksi[i].username, username) == 0)
            return i;
    }
    return -1;
}

static void cetakHeader(void)
{
    printf("\n%-4s %-11s %-8s %-12s %-15s %s\n",
        "ID", "Tanggal", "Tipe", "Jumlah(Rp)", "Kategori", "Keterangan");
    printf("--------------------------------------------------------------------------\n");
}

static void cetakBaris(const Transaksi *t)
{
    printf("%-4d %-11s %-8s %-12ld %-15s %s\n",
        t->id, t->tanggal,
        t->tipe == TRANSAKSI_MASUK ? "Masuk" : "Keluar",
        t->jumlah, t->kategori, t->keterangan);
}

static const char *userAktifAtauPesan(void)
{
    const char *u = getUsernameAktif();
    if (u == NULL) printf("Silakan login terlebih dahulu.\n");
    return u;
}

/* Meminta jumlah rupiah (maks MAKS_PERCOBAAN kali). 1 = berhasil, 0 = batal. */
static int mintaJumlah(const char *prompt, long *jumlah)
{
    char buf[32];
    int i;

    for (i = 0; i < MAKS_PERCOBAAN; i++) {
        if (!bacaString(prompt, buf, sizeof buf)) return 0;
        if (parseJumlah(buf, jumlah)) return 1;
        printf("Jumlah tidak valid. Contoh: 50000 atau 50.000 (harus > 0).\n");
    }
    return 0;
}

/* Meminta tanggal (maks MAKS_PERCOBAAN kali). 1 = berhasil, 0 = batal. */
static int mintaTanggal(const char *prompt, char *hasil)
{
    char buf[32];
    int i;

    for (i = 0; i < MAKS_PERCOBAAN; i++) {
        if (!bacaString(prompt, buf, sizeof buf)) return 0;
        if (normalisasiTanggal(buf, hasil)) return 1;
        printf("Tanggal tidak valid. Contoh: 23-11-2026 (kosongkan = hari ini).\n");
    }
    return 0;
}

/* ---------- tambah ---------- */

static void tambahTransaksi(int tipe)
{
    const char *user = userAktifAtauPesan();
    Transaksi t;
    long jumlah;

    if (user == NULL) return;

    if (jumlahTransaksi >= MAX_TRANSAKSI) {
        printf("Data transaksi penuh (maks %d).\n", MAX_TRANSAKSI);
        return;
    }

    memset(&t, 0, sizeof t);
    printf("\n=== %s ===\n", tipe == TRANSAKSI_MASUK ? "TAMBAH UANG MASUK" : "TAMBAH PENGELUARAN");

    if (!mintaJumlah("Jumlah (Rp)           : ", &jumlah)) {
        printf("Transaksi dibatalkan. Saldo tidak berubah.\n");
        return;
    }

    bacaString("Kategori              : ", t.kategori, sizeof t.kategori);
    if (t.kategori[0] == '\0') strcpy(t.kategori, "Lainnya");

    bacaString("Keterangan            : ", t.keterangan, sizeof t.keterangan);

    if (!mintaTanggal("Tanggal (DD-MM-YYYY, kosong = hari ini) : ", t.tanggal)) {
        printf("Transaksi dibatalkan. Saldo tidak berubah.\n");
        return;
    }

    if (tipe == TRANSAKSI_KELUAR && jumlah > hitungSaldo(user))
        printf("Peringatan: pengeluaran melebihi saldo, saldo akan negatif.\n");

    t.id     = idBaru();
    t.tipe   = tipe;
    t.jumlah = jumlah;
    strcpy(t.username, user);

    daftarTransaksi[jumlahTransaksi++] = t;
    printf("Transaksi berhasil dicatat (ID %d, tanggal %s).\n", t.id, t.tanggal);
    printf("Saldo sekarang : Rp %ld\n", hitungSaldo(user));
}

void tambahUangMasuk(void)  { tambahTransaksi(TRANSAKSI_MASUK); }
void tambahPengeluaran(void){ tambahTransaksi(TRANSAKSI_KELUAR); }

/* ---------- riwayat ---------- */

void riwayatTransaksi(void)
{
    const char *user = userAktifAtauPesan();
    int i, jumlah = 0;

    if (user == NULL) return;

    printf("\n=== RIWAYAT TRANSAKSI ===");
    cetakHeader();
    for (i = 0; i < jumlahTransaksi; i++) {
        if (strcmp(daftarTransaksi[i].username, user) == 0) {
            cetakBaris(&daftarTransaksi[i]);
            jumlah++;
        }
    }
    if (jumlah == 0) {
        printf("(belum ada transaksi)\n");
        return;
    }
    printf("--------------------------------------------------------------------------\n");
    printf("Total masuk  : Rp %ld\n", hitungTotalMasuk(user));
    printf("Total keluar : Rp %ld\n", hitungTotalKeluar(user));
    printf("Saldo        : Rp %ld\n", hitungSaldo(user));
}

/* ---------- ubah ---------- */

void ubahTransaksi(void)
{
    const char *user = userAktifAtauPesan();
    char buf[PANJANG_KETERANGAN];
    char tgl[PANJANG_TANGGAL];
    int id, idx;
    long jumlah;
    Transaksi *t;

    if (user == NULL) return;

    printf("\n=== UBAH TRANSAKSI ===\n");
    bacaString("ID transaksi yang diubah : ", buf, sizeof buf);
    id = atoi(buf);
    idx = cariIndeksById(id, user);
    if (idx == -1) {
        printf("Transaksi dengan ID tersebut tidak ditemukan.\n");
        return;
    }
    t = &daftarTransaksi[idx];

    cetakHeader();
    cetakBaris(t);
    printf("\n(Kosongkan input untuk mempertahankan nilai lama)\n");

    bacaString("Jumlah baru (Rp)          : ", buf, sizeof buf);
    if (buf[0] != '\0') {
        if (!parseJumlah(buf, &jumlah)) {
            printf("Jumlah tidak valid. Perubahan dibatalkan.\n");
            return;
        }
        t->jumlah = jumlah;
    }

    bacaString("Kategori baru             : ", buf, sizeof buf);
    if (buf[0] != '\0') {
        strncpy(t->kategori, buf, sizeof t->kategori - 1);
        t->kategori[sizeof t->kategori - 1] = '\0';
    }

    bacaString("Keterangan baru           : ", buf, sizeof buf);
    if (buf[0] != '\0') {
        strncpy(t->keterangan, buf, sizeof t->keterangan - 1);
        t->keterangan[sizeof t->keterangan - 1] = '\0';
    }

    bacaString("Tanggal baru (DD-MM-YYYY) : ", buf, sizeof buf);
    if (buf[0] != '\0') {
        if (!normalisasiTanggal(buf, tgl)) {
            printf("Tanggal tidak valid. Tanggal tidak diubah.\n");
        } else {
            strcpy(t->tanggal, tgl);
        }
    }

    printf("Transaksi berhasil diperbarui.\n");
}
