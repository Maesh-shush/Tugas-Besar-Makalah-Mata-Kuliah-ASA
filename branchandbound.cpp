#include "core.h"
#include "branchandbound.h"

int jumlahPruning = 0;

// -------------------------------------------------------
// Flood fill adaptif untuk game bertipe sliding.
// Menandai sel mana saja yang BENAR-BENAR bisa dijadikan
// tempat berhenti oleh kotak dari posisi saat ini.
// -------------------------------------------------------
static void floodFillSliding(int r, int c, bool terjangkau[MAKS][MAKS]) {
    // Jika posisi ini sudah pernah dievaluasi sebagai titik henti, lewati
    if (terjangkau[r][c]) { return; }
    terjangkau[r][c] = true;

    // Coba simulasikan peluncuran ke 4 arah untuk melihat titik henti berikutnya
    for (int dir = 0; dir < 4; dir++) {
        int nr = r;
        int nc = c;

        // Cari tahu di mana kotak akan BERHENTI jika meluncur ke arah 'dir'
        while (true) {
            int nnr = nr + DR[dir];
            int nnc = nc + DC[dir];

            // Jika membentur batas grid atau dinding, kotak berhenti di (nr, nc)
            if (nnr < 0 || nnr >= BARIS || nnc < 0 || nnc >= KOLOM || grid[nnr][nnc] == 0) {
                break;
            }
            nr = nnr;
            nc = nnc;
        }

        // Jika kotak berhasil pindah ke posisi baru, lakukan rekursi dari titik henti tersebut
        if (nr != r || nc != c) {
            floodFillSliding(nr, nc, terjangkau);
        }
    }
}

// -------------------------------------------------------
// Fungsi bound: Memeriksa apakah masih ada sel kosong
// yang mustahil dijangkau/dihentikan dari posisi sekarang.
// -------------------------------------------------------
static bool cekKonektivitas(int posR, int posC) {
    if (selDiwarnai == totalSel) { return true; }

    // Matriks untuk mencatat sel mana saja yang bisa dilewati/diinjak
    bool terjangkau[MAKS][MAKS] = {  };
    
    // Jalankan flood fill dengan mekanika sliding/meluncur
    floodFillSliding(posR, posC, terjangkau);

    // Cek setiap sel di grid
    for (int i = 0; i < BARIS; i++) {
        for (int j = 0; j < KOLOM; j++) {
            // Jika ada sel jalan ( bukan dinding) yang BELUM diwarnai,
            // tetapi secara mekanik meluncur SUDAH TIDAK BISA dijangkau lagi, 
            // maka struktur cabang ini harus dipangkas (pruned).
            if (grid[i][j] != 0 && !diwarnai[i][j]) {
                
                // Cari tahu apakah sel (i,j) ini dilewati jika kita meluncur dari 
                // titik-titik pemberhentian yang valid
                bool bisaDilewati = false;
                for (int dir = 0; dir < 4; dir++) {
                    int step = 1;
                    while (true) {
                        int checkR = i - DR[dir] * step;
                        int checkC = j - DC[dir] * step;
                        
                        if (checkR < 0 || checkR >= BARIS || checkC < 0 || checkC >= KOLOM || grid[checkR][checkC] == 0) {
                            break; 
                        }
                        if (terjangkau[checkR][checkC]) {
                            bisaDilewati = true;
                            break;
                        }
                        step++;
                    }
                    if (bisaDilewati) { break; }
                }
                // Terisolasi! Memicu Pruning.
                if (!bisaDilewati) { return false; }
            }
        }
    }

    return true;
}

// -------------------------------------------------------
// BRANCH AND BOUND dengan perbaikan urutan state
// -------------------------------------------------------
bool branchAndBound(int r, int c, bool transitVisited[MAKS][MAKS]) {
    nodesEksplorasi++;

    if (selDiwarnai == totalSel) { return true; }

    for (int dir = 0; dir < 4; dir++) {
        Posisi sel_baru[MAKS_SEL];
        int n_sel, nr, nc;

        // Fungsi geser() otomatis langsung mewarnai grid & memperbarui selDiwarnai
        geser(r, c, dir, &nr, &nc, sel_baru, &n_sel);
        if (nr == -1) { continue; }

        if (n_sel > 0) {
            if (!cekKonektivitas(nr, nc)) {
                jumlahPruning++; // Pruning sekarang akan aktif mendeteksi dead-end

                // Kembalikan state (Uncoloring) karena cabang ini dipangkas
                for (int i = 0; i < n_sel; i++) {
                    diwarnai[sel_baru[i].baris][sel_baru[i].kolom] = false;
                    selDiwarnai--;
                }
                continue; // Langsung skip arah ini tanpa masuk rekursi dalam
            }

            // Jika lolos pengecekan bound, lanjutkan langkah rekursif
            bool newTransit[MAKS][MAKS] = {  };
            newTransit[nr][nc] = true;

            solusi[jumlahLangkah++] = {r, c, nr, nc, ARAH[dir]};
            if (branchAndBound(nr, nc, newTransit)) { return true; }

            // Backtracking standard jika cabang di bawahnya gagal menemukan solusi keseluruhan
            jumlahLangkah--;
            for (int i = 0; i < n_sel; i++) {
                diwarnai[sel_baru[i].baris][sel_baru[i].kolom] = false;
                selDiwarnai--;
            }
        } else {
            // Luncuran transit: Jika tidak ada sel baru yang diwarnai
            if (transitVisited[nr][nc]) { continue; }

            transitVisited[nr][nc] = true;
            solusi[jumlahLangkah++] = {r, c, nr, nc, ARAH[dir]};
            if (branchAndBound(nr, nc, transitVisited)) { return true; }

            jumlahLangkah--;
            transitVisited[nr][nc] = false;
        }
    }
    return false;
}