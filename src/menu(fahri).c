#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "user.h"
#include "transaksi.h"
#include "laporan.h"
#include "file.h"

int bacaPilihan(void)
{
    char buf[16];
    char *akhir;
    long nilai;

    if (fgets(buf, sizeof buf, stdin) == NULL)
        return INPUT_HABIS;

    /* buang sisa baris jika input lebih panjang dari buffer */
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }

    nilai = strtol(buf, &akhir, 10);
    while (*akhir == ' ' || *akhir == '\t' || *akhir == '\r' || *akhir == '\n')
        akhir++;
    if (akhir == buf || *akhir != '\0')
        return -1;
    return (int)nilai;
}

static void cetakMenuAwal(void)
{
    printf("\n=== SISTEM KEUANGAN MAHASISWA ===\n");
    printf("1. Registrasi\n");
    printf("2. Login\n");
    printf("3. Keluar\n");
    printf("Pilih: ");
}

static void cetakMenuUtama(void)
{
    printf("\n=== MENU UTAMA (%s) ===\n", getUsernameAktif());
    printf(" 1. Tambah uang masuk\n");
    printf(" 2. Tambah pengeluaran\n");
    printf(" 3. Lihat saldo\n");
    printf(" 4. Riwayat transaksi\n");
    printf(" 5. Cari transaksi\n");
    printf(" 6. Ubah transaksi\n");
    printf(" 7. Hapus transaksi\n");
    printf(" 8. Laporan keuangan\n");
    printf(" 9. Laporan bulanan\n");
    printf("10. Simpan data\n");
    printf("11. Logout\n");
    printf("Pilih: ");
}

void menuUtama(void)
{
    int pilihan;

    do {
        cetakMenuUtama();
        pilihan = bacaPilihan();

        if (pilihan == INPUT_HABIS) {   /* input berakhir: keluar dengan aman */
            logoutPengguna();
            break;
        }

        switch (pilihan) {
            case 1:  tambahUangMasuk();   break;
            case 2:  tambahPengeluaran(); break;
            case 3:  lihatSaldo();        break;
            case 4:  riwayatTransaksi();  break;
            case 5:  cariTransaksi();     break;
            case 6:  ubahTransaksi();     break;
            case 7:  hapusTransaksi();    break;
            case 8:  laporanKeuangan();   break;
            case 9:  laporanBulanan();    break;
            case 10:
                if (simpanData()) printf("Data berhasil disimpan.\n");
                break;
            case 11: logoutPengguna();    break;
            default:
                printf("Pilihan tidak valid.\n");
        }

        simpanData();   /* simpan otomatis setiap selesai satu aksi */
    } while (penggunaSudahLogin());
}

void menuAwal(void)
{
    int pilihan;

    do {
        cetakMenuAwal();
        pilihan = bacaPilihan();

        if (pilihan == INPUT_HABIS)     /* input berakhir: anggap Keluar */
            pilihan = 3;

        switch (pilihan) {
            case 1:
                if (registrasiPengguna())
                    simpanData();
                break;
            case 2:
                if (loginPengguna())
                    menuUtama();
                break;
            case 3:
                printf("Program selesai.\n");
                break;
            default:
                printf("Pilihan tidak valid.\n");
        }
    } while (pilihan != 3);
}

void jalankanAplikasi(void)
{
    muatData();
    menuAwal();
    simpanData();
}
