#include <stdio.h>
#include "user.h"
#include "header/transaksi.h"
#include "laporan.h"
#include "file.h"

int main(void)
{
    int pilihan, menu;

    muatData();

    do {
        printf("\n=== SISTEM KEUANGAN MAHASISWA ===\n");
        printf("1. Registrasi\n");
        printf("2. Login\n");
        printf("3. Keluar\n");
        printf("Pilih: ");
        scanf("%d", &pilihan);
        getchar();

        switch (pilihan) {
            case 1:
                registrasiPengguna();
                simpanData();
                break;

            case 2:
                if (loginPengguna()) {
                    do {
                        printf("\n=== MENU UTAMA ===\n");
                        printf("1. Tambah uang masuk\n");
                        printf("2. Tambah pengeluaran\n");
                        printf("3. Riwayat transaksi\n");
                        printf("4. Cari transaksi\n");
                        printf("5. Ubah transaksi\n");
                        printf("6. Hapus transaksi\n");
                        printf("7. Laporan keuangan\n");
                        printf("8. Laporan bulanan\n");
                        printf("9. Logout\n");
                        printf("Pilih: ");
                        scanf("%d", &menu);
                        getchar();

                        switch (menu) {
                            case 1: tambahUangMasuk(); break;
                            case 2: tambahPengeluaran(); break;
                            case 3: riwayatTransaksi(); break;
                            case 4: cariTransaksi(); break;
                            case 5: ubahTransaksi(); break;
                            case 6: hapusTransaksi(); break;
                            case 7: laporanKeuangan(); break;
                            case 8: laporanBulanan(); break;
                            case 9: logoutPengguna(); break;
                            default:
                                printf("Pilihan tidak valid.\n");
                        }

                        simpanData();

                    } while (penggunaSudahLogin());
                }
                break;

            case 3:
                simpanData();
                printf("Program selesai.\n");
                break;

            default:
                printf("Pilihan tidak valid.\n");
        }

    } while (pilihan != 3);

    return 0;
}