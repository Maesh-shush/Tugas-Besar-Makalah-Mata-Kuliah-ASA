#include "core.h"

// -------------------------------------------------------
// Definisi konstanta arah
// -------------------------------------------------------
const int DR[4] = { 0,  1,  0, -1};
const int DC[4] = { 1,  0, -1,  0};
const char* ARAH[4] = {"Kanan", "Bawah", "Kiri", "Atas"};

// -------------------------------------------------------
// Definisi variabel global
// -------------------------------------------------------
int grid[MAKS][MAKS];
bool diwarnai[MAKS][MAKS];
int BARIS, KOLOM;
int totalSel, selDiwarnai;
int nodesEksplorasi, jumlahLangkah;
Langkah solusi[MAKS_SEL];

// -------------------------------------------------------
// Tampilkan grid (* = diwarnai, # = dinding, . = belum)
// -------------------------------------------------------
void tampilkanGrid() {
    for (int i = 0; i < BARIS; i++) {
        for (int j = 0; j < KOLOM; j++) {
            if (grid[i][j] == 0) { cout << "# "; }
            else if (diwarnai[i][j]) { cout << "* "; }
            else { cout << ". "; }
        }
        cout << "\n";
    }
}

// -------------------------------------------------------
// Simulasi kotak meluncur dari (r,c) ke arah dir
// hingga menabrak dinding
// Mengisi sel_baru dengan sel yang BARU diwarnai
// -------------------------------------------------------
void geser(int r, int c, int dir,
           int* baris_baru, int* kolom_baru,
           Posisi sel_baru[], int* n_sel) {
    int nr = r + DR[dir];
    int nc = c + DC[dir];
    *n_sel = 0;

    // Langsung terhalang dinding
    if (nr < 0 || nr >= BARIS || nc < 0 || nc >= KOLOM || grid[nr][nc] == 0) {
        *baris_baru = -1;
        *kolom_baru = -1;
        return;
    }

    // Meluncur terus hingga sel berikutnya adalah dinding
    bool berhenti = false;
    while (!berhenti) {
        if (!diwarnai[nr][nc]) {
            diwarnai[nr][nc] = true;
            sel_baru[*n_sel].baris = nr;
            sel_baru[*n_sel].kolom = nc;
            (*n_sel)++;
            selDiwarnai++;
        }
        int nnr = nr + DR[dir];
        int nnc = nc + DC[dir];
        if (nnr < 0 || nnr >= BARIS || nnc < 0 || nnc >= KOLOM || grid[nnr][nnc] == 0) {
            berhenti = true;
        }
        else { 
            nr = nnr; nc = nnc;
        }
    }
    *baris_baru = nr;
    *kolom_baru = nc;
}