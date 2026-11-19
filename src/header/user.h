#ifndef USER_H
#define USER_H

#define MAX_USER        50
#define PANJANG_NAMA    30

typedef struct {
    char username[PANJANG_NAMA];
    char password[PANJANG_NAMA];
} User;

/* Data global (dipakai juga oleh file.c untuk simpan/muat data) */
extern User daftarUser[MAX_USER];
extern int  jumlahUser;
extern int  indeksUserAktif;      /* -1 = belum login */

/* Mengembalikan 1 jika berhasil, 0 jika gagal */
int  registrasiPengguna(void);
int  loginPengguna(void);
void logoutPengguna(void);

/* Cek format username & password (1 = valid, 0 = tidak valid) */
int  validasiUsernamePassword(const char *username, const char *password);

/* Bantuan untuk modul lain */
int         penggunaSudahLogin(void);
const char *getUsernameAktif(void);   /* NULL jika belum login */

#endif