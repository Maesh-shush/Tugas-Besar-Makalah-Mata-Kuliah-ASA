#ifndef CORE_H
#define CORE_H

#include <iostream>
using namespace std;

// -------------------------------------------------------
// Konstanta
// -------------------------------------------------------
const int MAKS = 10;
const int MAKS_SEL = 100;

// Prioritas arah: 0=Kanan, 1=Bawah, 2=Kiri, 3=Atas
extern const int DR[4];
extern const int DC[4];
extern const char* ARAH[4];

// -------------------------------------------------------
// Struktur data
// -------------------------------------------------------
struct Posisi { int baris, kolom; };
struct Langkah { int dariR, dariC, keR, keC; const char* arah; };

// -------------------------------------------------------
// Variabel global (didefinisikan di core.cpp)
// -------------------------------------------------------
// Konvensi grid: 0=dinding, 1=sel kosong, 2=posisi awal
extern int grid[MAKS][MAKS];
extern bool diwarnai[MAKS][MAKS];
extern int BARIS, KOLOM;
extern int totalSel, selDiwarnai;
extern int nodesEksplorasi, jumlahLangkah;
extern Langkah solusi[MAKS_SEL];

// -------------------------------------------------------
// Deklarasi fungsi inti
// -------------------------------------------------------
void tampilkanGrid();
void geser(int r, int c, int dir,
           int* baris_baru, int* kolom_baru,
           Posisi sel_baru[], int* n_sel);

#endif