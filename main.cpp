#include "core.h"
#include "backtrack.h"
#include "branchandbound.h"
#include "simulatedannealing.h"
#include <ctime>
#include <cstring>

// -------------------------------------------------------
// Inisialisasi level ke dalam grid global
// -------------------------------------------------------
void inisialisasiLevel(int level[][MAKS], int baris, int kolom,
                       int* startR, int* startC) {
    BARIS = baris; KOLOM = kolom;
    totalSel = 0; selDiwarnai = 0;
    *startR = -1; *startC = -1;

    for (int i = 0; i < BARIS; i++)
        for (int j = 0; j < KOLOM; j++) {
            grid[i][j] = level[i][j];
            diwarnai[i][j] = false;
            if (level[i][j] != 0) {
                totalSel++;
                if (level[i][j] == 2) { *startR = i; *startC = j; }
            }
        }
    diwarnai[*startR][*startC] = true;
    selDiwarnai = 1;
}

// -------------------------------------------------------
// Reset state sebelum algoritma berikutnya
// -------------------------------------------------------
void resetState(int startR, int startC) {
    for (int i = 0; i < BARIS; i++) {
        for (int j = 0; j < KOLOM; j++) {
            diwarnai[i][j] = false;
        }
    }
    diwarnai[startR][startC] = true;
    selDiwarnai     = 1;
    nodesEksplorasi = 0;
    jumlahLangkah   = 0;
}

// -------------------------------------------------------
// Tampilkan hasil pengujian
// -------------------------------------------------------
void tampilkanHasil(const char* nama, bool berhasil) {
    cout << "  [" << nama << "]\n";
    if (berhasil) {
        cout << "  Solusi     : DITEMUKAN\n";
        for (int i = 0; i < jumlahLangkah; i++) {
            Langkah& s = solusi[i];
            cout << "  " << i+1 << ". ("
                 << s.dariR << "," << s.dariC << ") --"
                 << s.arah  << "--> ("
                 << s.keR   << "," << s.keC   << ")\n";
        }
    } else {
        cout << "  Solusi     : TIDAK DITEMUKAN\n";
    }
    cout << "  Langkah    : " << jumlahLangkah   << "\n";
    cout << "  Nodes      : " << nodesEksplorasi << "\n";
}

// -------------------------------------------------------
// Jalankan ketiga algoritma pada satu level
// -------------------------------------------------------
void ujiLevel(const char* nama, int level[][MAKS], int baris, int kolom) {
    int startR, startC;
    inisialisasiLevel(level, baris, kolom, &startR, &startC);

    cout << "\n========================================\n";
    cout << nama << " (" << baris << "x" << kolom
         << ", " << totalSel << " sel)\n";
    cout << "========================================\n";
    tampilkanGrid();
    cout << "Posisi awal: (" << startR << "," << startC << ")\n\n";

    // --- Backtracking ---
    resetState(startR, startC);
    bool transitBT[MAKS][MAKS] = {};
    transitBT[startR][startC]  = true;
    clock_t t1  = clock();
    bool hasilBT = backtrack(startR, startC, transitBT);
    double wtBT  = (double)(clock()-t1)/CLOCKS_PER_SEC*1000;
    tampilkanHasil("Backtracking", hasilBT);
    cout << "  Waktu      : " << wtBT << " ms\n\n";

    // --- Branch and Bound ---
    jumlahPruning = 0;
    resetState(startR, startC);
    bool transitBnB[MAKS][MAKS] = {};
    transitBnB[startR][startC]  = true;
    clock_t t2   = clock();
    bool hasilBnB = branchAndBound(startR, startC, transitBnB);
    double wtBnB  = (double)(clock()-t2)/CLOCKS_PER_SEC*1000;
    tampilkanHasil("Branch and Bound", hasilBnB);
    cout << "  Waktu      : " << wtBnB << " ms\n";
    cout << "  Pruning    : " << jumlahPruning   << "\n\n";

    // --- Simulated Annealing ---
    jumlahIterasi = 0;
    resetState(startR, startC);
    clock_t t3   = clock();
    bool hasilSA  = simulatedAnnealing(startR, startC);
    double wtSA   = (double)(clock()-t3)/CLOCKS_PER_SEC*1000;
    tampilkanHasil("Simulated Annealing", hasilSA);
    cout << "  Waktu      : " << wtSA  << " ms\n";
    cout << "  Iterasi    : " << jumlahIterasi   << "\n\n";
}

int main() {

    // =====================================================
    // LEVEL 1: 5x5
    // =====================================================
    int level1[MAKS][MAKS] = {
        {0, 0, 0, 0, 0},
        {0, 2, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0}
    };
    ujiLevel("Level 1", level1, 5, 5);

    // =====================================================
    // LEVEL 2: 7x7
    // =====================================================
    int level2[MAKS][MAKS]={
        {0, 0, 0, 0, 0, 0, 0},
        {0, 2, 1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    ujiLevel("Level 2", level2, 7, 7);

    // =====================================================
    // LEVEL 3: 10x10
    // =====================================================
    int level3[MAKS][MAKS] = {
        {1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 0},
        {1, 0, 1, 1, 0, 0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 0},
        {0, 0, 1, 1, 0, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 2, 0, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    ujiLevel("Level 3", level3, 10, 10);

    return 0;
}