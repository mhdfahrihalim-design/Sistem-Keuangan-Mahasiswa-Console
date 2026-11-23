#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "transaksi.h"
#include "laporan.h"

Transaksi daftarTransaksi[MAX_TRANSAKSI];
int       jumlahTransaksi = 0;

/* ---------- fungsi bantu (private) ---------- */

static void bacaString(const char *prompt, char *buf, int ukuran)
{
    size_t len;
    int c;

    printf("%s", prompt);
    if (fgets(buf, ukuran, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}

static int parseJumlah(const char *s, long *hasil)
{
    char *akhir;
    long nilai;

    if (s[0] == '\0') return 0;
    nilai = strtol(s, &akhir, 10);
    if (*akhir != '\0' || nilai <= 0) return 0;
    *hasil = nilai;
    return 1;
}

static int tahunKabisat(int t)
{
    return (t % 4 == 0 && t % 100 != 0) || (t % 400 == 0);
}

/* Format DD-MM-YYYY */
static int validasiTanggal(const char *tgl)
{
    int hari[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int d, m, y, i;

    if (strlen(tgl) != 10 || tgl[2] != '-' || tgl[5] != '-')
        return 0;
    for (i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)tgl[i])) return 0;
    }
    if (sscanf(tgl, "%d-%d-%d", &d, &m, &y) != 3) return 0;
    if (m < 1 || m > 12 || y < 1900 || y > 2100) return 0;
    if (m == 2 && tahunKabisat(y)) hari[1] = 29;
    return d >= 1 && d <= hari[m - 1];
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

static int mengandungKataKunci(const char *teks, const char *kunci)
{
    char a[PANJANG_KETERANGAN + 1], b[PANJANG_KETERANGAN + 1];
    int i;

    strncpy(a, teks, sizeof a - 1);  a[sizeof a - 1] = '\0';
    strncpy(b, kunci, sizeof b - 1); b[sizeof b - 1] = '\0';
    for (i = 0; a[i]; i++) a[i] = (char)tolower((unsigned char)a[i]);
    for (i = 0; b[i]; i++) b[i] = (char)tolower((unsigned char)b[i]);
    return strstr(a, b) != NULL;
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

/* ---------- tambah ---------- */

static void tambahTransaksi(int tipe)
{
    const char *user = userAktifAtauPesan();
    Transaksi t;
    char buf[32];
    long jumlah;

    if (user == NULL) return;

    if (jumlahTransaksi >= MAX_TRANSAKSI) {
        printf("Data transaksi penuh (maks %d).\n", MAX_TRANSAKSI);
        return;
    }

    printf("\n=== %s ===\n", tipe == TRANSAKSI_MASUK ? "TAMBAH UANG MASUK" : "TAMBAH PENGELUARAN");

    bacaString("Jumlah (Rp)           : ", buf, sizeof buf);
    if (!parseJumlah(buf, &jumlah)) {
        printf("Jumlah harus berupa angka bulat lebih dari 0.\n");
        return;
    }

    bacaString("Kategori              : ", t.kategori, sizeof t.kategori);
    if (t.kategori[0] == '\0') strcpy(t.kategori, "Lainnya");

    bacaString("Keterangan            : ", t.keterangan, sizeof t.keterangan);

    bacaString("Tanggal (DD-MM-YYYY)  : ", t.tanggal, sizeof t.tanggal);
    if (!validasiTanggal(t.tanggal)) {
        printf("Format atau nilai tanggal tidak valid.\n");
        return;
    }

    if (tipe == TRANSAKSI_KELUAR && jumlah > hitungSaldo(user))
        printf("Peringatan: pengeluaran melebihi saldo, saldo akan negatif.\n");

    t.id     = idBaru();
    t.tipe   = tipe;
    t.jumlah = jumlah;
    strcpy(t.username, user);

    daftarTransaksi[jumlahTransaksi++] = t;
    printf("Transaksi berhasil dicatat (ID %d).\n", t.id);
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

/* ---------- cari ---------- */

void cariTransaksi(void)
{
    const char *user = userAktifAtauPesan();
    char pilih[8], kunci[PANJANG_KETERANGAN];
    int i, ketemu = 0, mode;

    if (user == NULL) return;

    printf("\n=== CARI TRANSAKSI ===\n");
    printf("1. Berdasarkan kata kunci (kategori/keterangan)\n");
    printf("2. Berdasarkan tanggal (DD-MM-YYYY)\n");
    bacaString("Pilih : ", pilih, sizeof pilih);
    mode = atoi(pilih);

    if (mode != 1 && mode != 2) {
        printf("Pilihan tidak valid.\n");
        return;
    }

    bacaString(mode == 1 ? "Kata kunci : " : "Tanggal    : ", kunci, sizeof kunci);
    if (kunci[0] == '\0') {
        printf("Input tidak boleh kosong.\n");
        return;
    }

    for (i = 0; i < jumlahTransaksi; i++) {
        const Transaksi *t = &daftarTransaksi[i];
        int cocok;

        if (strcmp(t->username, user) != 0) continue;

        if (mode == 1)
            cocok = mengandungKataKunci(t->kategori, kunci) ||
                    mengandungKataKunci(t->keterangan, kunci);
        else
            cocok = (strcmp(t->tanggal, kunci) == 0);

        if (cocok) {
            if (!ketemu) cetakHeader();
            cetakBaris(t);
            ketemu++;
        }
    }
    if (!ketemu) printf("Transaksi tidak ditemukan.\n");
    else         printf("\nDitemukan %d transaksi.\n", ketemu);
}

/* ---------- ubah ---------- */

void ubahTransaksi(void)
{
    const char *user = userAktifAtauPesan();
    char buf[PANJANG_KETERANGAN];
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
        if (!validasiTanggal(buf)) {
            printf("Tanggal tidak valid. Tanggal tidak diubah.\n");
        } else {
            strcpy(t->tanggal, buf);
        }
    }

    printf("Transaksi berhasil diperbarui.\n");
}

/* ---------- hapus ---------- */

void hapusTransaksi(void)
{
    const char *user = userAktifAtauPesan();
    char buf[16];
    int id, idx, i;

    if (user == NULL) return;

    printf("\n=== HAPUS TRANSAKSI ===\n");
    bacaString("ID transaksi yang dihapus : ", buf, sizeof buf);
    id = atoi(buf);
    idx = cariIndeksById(id, user);
    if (idx == -1) {
        printf("Transaksi dengan ID tersebut tidak ditemukan.\n");
        return;
    }

    cetakHeader();
    cetakBaris(&daftarTransaksi[idx]);

    bacaString("Yakin hapus? (y/n) : ", buf, sizeof buf);
    if (tolower((unsigned char)buf[0]) != 'y') {
        printf("Penghapusan dibatalkan.\n");
        return;
    }

    for (i = idx; i < jumlahTransaksi - 1; i++)
        daftarTransaksi[i] = daftarTransaksi[i + 1];
    jumlahTransaksi--;

    printf("Transaksi berhasil dihapus.\n");
}
