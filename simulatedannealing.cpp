#include "core.h"
#include "simulatedannealing.h"
#include <cstdlib>
#include <cmath>
#include <ctime>

int jumlahIterasi = 0;

// -------------------------------------------------------
// Parameter Simulated Annealing
// -------------------------------------------------------
static const double T_AWAL    = 10.0;    // Suhu awal
static const double T_MIN     = 0.001;   // Suhu minimum sebelum restart
static const double ALPHA     = 0.9993;  // Laju pendinginan
static const int    MAX_ITER  = 500000;  // Batas maksimum iterasi
static const int    MAX_STUCK = 500;     // Batas iterasi tanpa pewarnaan baru

static const int    SA_BUFFER = 10000;
static Langkah      saSolusi[SA_BUFFER];
static int          saJumlahLangkah;

// -------------------------------------------------------
// Pratinjau geser tanpa mengubah state
// Mengembalikan jumlah sel baru yang akan diwarnai
// dan posisi akhir setelah luncuran
// -------------------------------------------------------
static int pratinjauGeser(int r, int c, int dir,
                           int* nr_out, int* nc_out) {
    int nr = r + DR[dir];
    int nc = c + DC[dir];

    if (nr < 0 || nr >= BARIS || nc < 0 || nc >= KOLOM || grid[nr][nc] == 0) {
        *nr_out = -1;
        *nc_out = -1;
        return -1;
    }

    int count    = 0;
    bool berhenti = false;
    while (!berhenti) {
        if (!diwarnai[nr][nc]) {
            count++;
        }
        int nnr = nr + DR[dir];
        int nnc = nc + DC[dir];
        if (nnr < 0 || nnr >= BARIS ||
            nnc < 0 || nnc >= KOLOM ||
            grid[nnr][nnc] == 0) {
            berhenti = true;
        } else {
            nr = nnr;
            nc = nnc;
        }
    }

    *nr_out = nr;
    *nc_out = nc;
    return count;
}

// -------------------------------------------------------
// SIMULATED ANNEALING
//
// Karena semua luncuran yang dipilih pasti mewarnai
// sel baru (ΔE selalu negatif), luncuran selalu
// diterima. Keacakan SA sepenuhnya berada pada
// mekanisme pemilihan arah berbasis suhu.
//
// Jika tidak ada arah berguna yang tersedia atau
// suhu terlalu rendah, dilakukan restart:
//   - Setiap kelipatan 3: restart penuh ke awal
//   - Selainnya: restart ke state terbaik
// -------------------------------------------------------
bool simulatedAnnealing(int startR, int startC) {
    srand(42);

    int    posR         = startR;
    int    posC         = startC;
    double T            = T_AWAL;
    int    stuckCount   = 0;
    int    restartCount = 0;
    saJumlahLangkah     = 0;

    // --- Inisialisasi state terbaik ---
    bool    bestDiwarnai[MAKS][MAKS];
    int     bestSelDiwarnai = selDiwarnai;
    int     bestPosR        = startR;
    int     bestPosC        = startC;
    Langkah bestSaSolusi[SA_BUFFER];
    int     bestSaLangkah   = 0;

    for (int i = 0; i < BARIS; i++) {
        for (int j = 0; j < KOLOM; j++) {
            bestDiwarnai[i][j] = diwarnai[i][j];
        }
    }

    // ===================== LOOP UTAMA =====================
    for (jumlahIterasi = 0; jumlahIterasi < MAX_ITER; jumlahIterasi++) {
        nodesEksplorasi++;

        // Kondisi selesai: semua sel sudah diwarnai
        if (selDiwarnai == totalSel) {
            jumlahLangkah = (saJumlahLangkah < MAKS_SEL)
                            ? saJumlahLangkah
                            : MAKS_SEL;
            for (int i = 0; i < jumlahLangkah; i++) {
                solusi[i] = saSolusi[i];
            }
            return true;
        }

        // --- Kumpulkan arah yang MEWARNAI sel baru (cnt > 0) ---
        int validDir[4], validCount[4];
        int validNr[4],  validNc[4];
        int nValid = 0;

        for (int d = 0; d < 4; d++) {
            int nr, nc;
            int cnt = pratinjauGeser(posR, posC, d, &nr, &nc);
            if (cnt > 0) {             // hanya terima luncuran berguna
                validDir[nValid]   = d;
                validCount[nValid] = cnt;
                validNr[nValid]    = nr;
                validNc[nValid]    = nc;
                nValid++;
            }
        }

        // Tidak ada arah berguna → macet
        if (nValid == 0) {
            stuckCount += 10;
            goto do_restart;
        }

        {
            // --- Pilih arah: greedy atau acak ---
            // greedyProb makin besar seiring suhu turun
            double greedyProb = 1.0 - (T / T_AWAL);
            int    bestDir    = -1;
            int    bestCount  = -1;
            int    bestNr     = -1;
            int    bestNc     = -1;

            if ((double)rand() / RAND_MAX < greedyProb) {
                // Greedy: pilih arah yang mewarnai sel terbanyak
                for (int i = 0; i < nValid; i++) {
                    if (validCount[i] > bestCount) {
                        bestCount = validCount[i];
                        bestDir   = validDir[i];
                        bestNr    = validNr[i];
                        bestNc    = validNc[i];
                    }
                }
            } else {
                // Acak: pilih salah satu arah valid secara acak
                int idx   = rand() % nValid;
                bestDir   = validDir[idx];
                bestCount = validCount[idx];
                bestNr    = validNr[idx];
                bestNc    = validNc[idx];
            }

            // --- Eksekusi luncuran ---
            Posisi sel_baru[MAKS_SEL];
            int    n_sel, nr, nc;
            geser(posR, posC, bestDir, &nr, &nc, sel_baru, &n_sel);

            // Semua luncuran dipilih dari yang berguna (n_sel > 0)
            // sehingga ΔE = -n_sel selalu negatif → selalu diterima
            if (saJumlahLangkah < SA_BUFFER) {
                saSolusi[saJumlahLangkah++] = {posR, posC, nr, nc, ARAH[bestDir]};
            }
            posR       = nr;
            posC       = nc;
            stuckCount = 0;

            // Update state terbaik jika ada perbaikan
            if (selDiwarnai > bestSelDiwarnai) {
                bestSelDiwarnai = selDiwarnai;
                bestPosR        = posR;
                bestPosC        = posC;
                for (int i = 0; i < BARIS; i++) {
                    for (int j = 0; j < KOLOM; j++) {
                        bestDiwarnai[i][j] = diwarnai[i][j];
                    }
                }
                for (int i = 0; i < saJumlahLangkah; i++) {
                    bestSaSolusi[i] = saSolusi[i];
                }
                bestSaLangkah = saJumlahLangkah;
            }
        }

        // --- Turunkan suhu: T = T × α ---
        T *= ALPHA;

        // --- Restart jika macet atau suhu terlalu rendah ---
        do_restart:
        if (stuckCount > MAX_STUCK || T < T_MIN) {
            restartCount++;

            if (restartCount % 3 == 0) {
                // Restart penuh: kembali ke posisi awal
                for (int i = 0; i < BARIS; i++) {
                    for (int j = 0; j < KOLOM; j++) {
                        diwarnai[i][j] = (i == startR && j == startC);
                    }
                }
                selDiwarnai     = 1;
                posR            = startR;
                posC            = startC;
                saJumlahLangkah = 0;
            } else {
                // Restart dari state terbaik
                for (int i = 0; i < BARIS; i++) {
                    for (int j = 0; j < KOLOM; j++) {
                        diwarnai[i][j] = bestDiwarnai[i][j];
                    }
                }
                selDiwarnai     = bestSelDiwarnai;
                posR            = bestPosR;
                posC            = bestPosC;
                saJumlahLangkah = bestSaLangkah;
                for (int i = 0; i < bestSaLangkah; i++) {
                    saSolusi[i] = bestSaSolusi[i];
                }
            }

            stuckCount = 0;
            T          = T_AWAL;   // reheat
        }
    }

    return false;   // solusi tidak ditemukan dalam batas iterasi
}