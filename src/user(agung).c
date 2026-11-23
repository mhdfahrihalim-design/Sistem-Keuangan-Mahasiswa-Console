#include <stdio.h>
#include <string.h>
#include "../include/user.h"

User daftarUser[MAX_USER];
int  jumlahUser     = 0;
int  indeksUserAktif = -1;

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
        while ((c = getchar()) != '\n' && c != EOF) { }  /* buang sisa input */
    }
}

static int cariIndeksUser(const char *username)
{
    int i;
    for (i = 0; i < jumlahUser; i++) {
        if (strcmp(daftarUser[i].username, username) == 0)
            return i;
    }
    return -1;
}

static int mengandungSpasi(const char *s)
{
    for (; *s; s++) {
        if (*s == ' ' || *s == '\t') return 1;
    }
    return 0;
}

/* ---------- fungsi publik ---------- */

int validasiUsernamePassword(const char *username, const char *password)
{
    size_t lu = strlen(username);
    size_t lp = strlen(password);

    if (lu < 3 || lu >= PANJANG_NAMA) {
        printf("Username harus 3-%d karakter.\n", PANJANG_NAMA - 1);
        return 0;
    }
    if (mengandungSpasi(username)) {
        printf("Username tidak boleh mengandung spasi.\n");
        return 0;
    }
    if (lp < 4 || lp >= PANJANG_NAMA) {
        printf("Password harus 4-%d karakter.\n", PANJANG_NAMA - 1);
        return 0;
    }
    if (mengandungSpasi(password)) {
        printf("Password tidak boleh mengandung spasi.\n");
        return 0;
    }
    return 1;
}

int registrasiPengguna(void)
{
    char username[PANJANG_NAMA];
    char password[PANJANG_NAMA];
    char konfirmasi[PANJANG_NAMA];

    printf("\n=== REGISTRASI PENGGUNA ===\n");

    if (jumlahUser >= MAX_USER) {
        printf("Kapasitas pengguna penuh (maks %d).\n", MAX_USER);
        return 0;
    }

    bacaString("Username : ", username, sizeof username);
    bacaString("Password : ", password, sizeof password);

    if (!validasiUsernamePassword(username, password))
        return 0;

    if (cariIndeksUser(username) != -1) {
        printf("Username sudah dipakai. Gunakan username lain.\n");
        return 0;
    }

    bacaString("Ulangi password : ", konfirmasi, sizeof konfirmasi);
    if (strcmp(password, konfirmasi) != 0) {
        printf("Konfirmasi password tidak sama.\n");
        return 0;
    }

    strcpy(daftarUser[jumlahUser].username, username);
    strcpy(daftarUser[jumlahUser].password, password);
    jumlahUser++;

    printf("Registrasi berhasil! Silakan login.\n");
    return 1;
}

int loginPengguna(void)
{
    char username[PANJANG_NAMA];
    char password[PANJANG_NAMA];
    int percobaan, idx;

    printf("\n=== LOGIN PENGGUNA ===\n");

    if (indeksUserAktif != -1) {
        printf("Anda sudah login sebagai %s.\n", daftarUser[indeksUserAktif].username);
        return 1;
    }
    if (jumlahUser == 0) {
        printf("Belum ada pengguna terdaftar. Silakan registrasi dulu.\n");
        return 0;
    }

    for (percobaan = 1; percobaan <= 3; percobaan++) {
        bacaString("Username : ", username, sizeof username);
        bacaString("Password : ", password, sizeof password);

        idx = cariIndeksUser(username);
        if (idx != -1 && strcmp(daftarUser[idx].password, password) == 0) {
            indeksUserAktif = idx;
            printf("Login berhasil. Selamat datang, %s!\n", username);
            return 1;
        }
        printf("Username atau password salah (percobaan %d/3).\n", percobaan);
    }

    printf("Login gagal. Kesempatan habis.\n");
    return 0;
}

void logoutPengguna(void)
{
    if (indeksUserAktif == -1) {
        printf("Anda belum login.\n");
        return;
    }
    printf("Sampai jumpa, %s!\n", daftarUser[indeksUserAktif].username);
    indeksUserAktif = -1;
}

int penggunaSudahLogin(void)
{
    return indeksUserAktif != -1;
}

const char *getUsernameAktif(void)
{
    if (indeksUserAktif == -1) return NULL;
    return daftarUser[indeksUserAktif].username;
}