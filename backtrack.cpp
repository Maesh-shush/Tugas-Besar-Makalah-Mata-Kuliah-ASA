#include "core.h"
#include "backtrack.h"

// -------------------------------------------------------
// BACKTRACKING dengan dukungan transit move
//
// Dari posisi saat ini, mencoba meluncur ke setiap arah
// dengan prioritas: kanan, bawah, kiri, atas.
//
// Dua jenis luncuran:
//   1. Luncuran BERGUNA  : mewarnai minimal 1 sel baru
//      → selalu diterima, reset pelacak transit
//   2. Luncuran TRANSIT  : tidak ada sel baru (lewat sel
//      yang sudah diwarnai)
//      → diterima jika posisi tujuan belum pernah
//        dikunjungi via transit sejak luncuran berguna
//        terakhir (mencegah perulangan tak terbatas)
//
// Jika tidak ada arah yang menghasilkan solusi → backtrack
// -------------------------------------------------------
bool backtrack(int r, int c, bool transitVisited[MAKS][MAKS]) {
    nodesEksplorasi++;

    if (selDiwarnai == totalSel) return true;

    for (int dir = 0; dir < 4; dir++) {
        Posisi sel_baru[MAKS_SEL];
        int n_sel, nr, nc;

        geser(r, c, dir, &nr, &nc, sel_baru, &n_sel);
        if (nr == -1) continue;

        if (n_sel > 0) {
            // Luncuran berguna: reset pelacak transit
            bool newTransit[MAKS][MAKS] = {};
            newTransit[nr][nc] = true;

            solusi[jumlahLangkah++] = {r, c, nr, nc, ARAH[dir]};
            if (backtrack(nr, nc, newTransit)) return true;

            jumlahLangkah--;
            for (int i = 0; i < n_sel; i++) {
                diwarnai[sel_baru[i].baris][sel_baru[i].kolom] = false;
                selDiwarnai--;
            }
        } else {
            // Luncuran transit: cek perulangan
            if (transitVisited[nr][nc]) continue;

            transitVisited[nr][nc] = true;
            solusi[jumlahLangkah++] = {r, c, nr, nc, ARAH[dir]};
            if (backtrack(nr, nc, transitVisited)) return true;

            jumlahLangkah--;
            transitVisited[nr][nc] = false;
        }
    }
    return false;
}