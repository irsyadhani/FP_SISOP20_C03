#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define lim              18    
#define enc_poslength    2   //encode handling
#define ukuran_buf       4096    
#define Idx_Akar         ukuran_buf   //indeks akar BST

/* Konsep kompressi : salah satu algoritma yang sederhana ditunjukkan dengan algoritma
 yang dikenal denga Run-length encoding (RLE). Prosees ini juga disebut proses encoding, 
 dengan mentransformasikan informasi ke format lain. Karena tujuannya untuk mengecilkan ukuran, 
 maka perhitungan rasio menjadi suatu tolak ukur keberhasilan sebuah algoritma kompressi. 
*/
void masukkanSimpul(int chr);
void hapusSimpul(int nod);
void encode();
int file_dalam, file_luar;
//buffer ukuran N, dengan byte F-1 ekstra untuk perbandingan string
unsigned char bufferteks[ukuran_buf + lim - 1]; 
int  m_posisi, m_panjang,  child_kiri[ukuran_buf + 1], child_kanan[ukuran_buf + 257], ortu[ukuran_buf + 1];  //elemen bst 
    
/* 
void isi_tree() s
{
    int  i;
    for (i = ukuran_buf + 1; i <= ukuran_buf + 256; i++) child_kanan[i] = Idx_Akar;
    for (i = 0; i < ukuran_buf; i++) ortu[i] = Idx_Akar;
}
*/

int main(int arg1, char *arg2[]){
  //misalkan zip –m tes.zip tes.txt (menghapus file asli setelah di zip)
    if(strcmp(arg2[1],"-m")==0){
        for(int i = 3; i<arg1; i++){
            if ((file_luar = open(arg2[2], O_WRONLY | O_CREATE)) == -1){
                printf(2, "zip: tidak bisa membuka file %s\n", arg2[2]);
                close(file_luar);
                exit();
             }
            if (strcmp(arg2[1], arg2[i]) == 0){
                printf(2, "zip: tidak bisa memproses 2 file sama\n");
                exit();
            }
            if ((file_dalam = open(arg2[i], O_RDONLY)) == -1){
                printf(2, "zip: tidak bisa membuka file %s\n", arg2[i]);
                close(file_dalam);
                exit();
            }
 
            else encode(); //lakukan fungsi encode untuk kompressi
            close(file_dalam);
            if(unlink(arg2[i]) < 0){
                printf(2, "zip: %s gagal dihapus\n", arg2[i]);
                break;
            }
        } 
        close(file_luar);        
    }
    //misal: zip tes.zip tes.txt (mengcompress file tes)
    //zip zipfile.zip file1.txt file2.txt (Meng-compress file1.txt dan file2.txt kedalam file zipfile.zip)
    else if(strcmp(arg2[1],"-r")!=0 && strcmp(arg2[1],"-m")!=0 ){
        for(int i = 2; i<arg1; i++){
            if ((file_luar = open(arg2[1], O_WRONLY | O_CREATE)) == -1){
                printf(2, "zip: tidak bisa membuka file %s\n", arg2[1]);
                close(file_luar);
                exit();
             }
            if (strcmp(arg2[1], arg2[i]) == 0){
                printf(2, "zip: tidak bisa mengoperasikan 2 file sama\n");
                exit();
            }
            if ((file_dalam = open(arg2[i], O_RDONLY)) == -1){
                printf(2, "zip: tidak bisa membuka file %s\n", arg2[i]);
                close(file_dalam);
                exit();
            }
            else encode(); //lakukan fungsi encode untuk kompressi

            close(file_dalam);
        } 
        close(file_luar);
    }
    //zip -r zipfile.zip direktori (untuk zip direktori secara rekursif)
    //belum kelar
    else {
        //sebagai error handling argumen
        printf(2, "Masukkan argumen yang benar : '-m'/'-r'\n");
        exit();
    }
    close(file_dalam);  
    close(file_luar);  
    
    exit();
}

//fungsi untuk melakukan inserting pada simpul pohon/node tree
//Menyisipkan string dengan panjang "lim", bufferteks [chr..chr + lim-1], ke dalam salah satu tree
void masukkanSimpul(int chr)

{
    int  i, nod, bdg;
    unsigned char  *tanda;
    tanda = &bufferteks[chr];  
    m_panjang = 0;
    bdg = 1;  
    int l = ukuran_buf+1;
    nod = tanda[0]+ l ;
    child_kanan[chr] = child_kiri[chr] = Idx_Akar;  

  	while(1) {
        if (bdg >= 0) {
            if (child_kanan[nod] != Idx_Akar) nod = child_kanan[nod];
            else {  child_kanan[nod] = chr;  
            ortu[chr] = nod;  
            return;  
            }
        } else if (bdg<0){
            if (child_kiri[nod] != Idx_Akar) nod = child_kiri[nod];
            else {  
                child_kiri[nod] = chr;  
                ortu[chr] = nod;  
                return;  
                }
        }
        for (i = 1; i < lim; i++)
            if ((bdg = tanda[i] - bufferteks[nod + i]) != 0)  break;
        if (i > m_panjang) {
            m_posisi = nod;
            if ((m_panjang = i) >= lim)  break;
        }
    }
    ortu[chr] = ortu[nod];  
    child_kiri[chr] = child_kiri[nod];  
    child_kanan[chr] = child_kanan[nod];
    ortu[child_kiri[nod]] = chr;  
    ortu[child_kanan[nod]] = chr;
    if (child_kanan[ortu[nod]] == nod) child_kanan[ortu[nod]] = chr;
    else if (child_kanan[ortu[nod]] != nod) child_kiri[ortu[nod]] = chr;
    ortu[nod] = Idx_Akar; //menghapuskan nod
}

//fungsi untuk menghapus node/simpul akar.
void hapusSimpul(int nod)  
{
    int nod1;
        //jika tdk dalam tree
    if (ortu[nod] == Idx_Akar) return;  
    if (child_kanan[nod] == Idx_Akar) nod1 = child_kiri[nod];
    else if (child_kiri[nod] == Idx_Akar) nod1 = child_kanan[nod];
    else {
        nod1 = child_kiri[nod];
        if (child_kanan[nod1] != Idx_Akar) {
            do {  
                nod1 = child_kanan[nod1];  
            } while (child_kanan[nod1] != Idx_Akar);
            child_kanan[ortu[nod1]] = child_kiri[nod1];  
            ortu[child_kiri[nod1]] = ortu[nod1];
            child_kiri[nod1] = child_kiri[nod];  
            ortu[child_kiri[nod]] = nod1;
        }
        child_kanan[nod1] = child_kanan[nod]; 
        ortu[child_kanan[nod]] = nod1;
    }
    ortu[nod1] = ortu[nod];
    if (child_kanan[ortu[nod]] == nod) child_kanan[ortu[nod]] = nod1;  
    else child_kiri[ortu[nod]] = nod1;
    ortu[nod] = Idx_Akar; 
}

//fungsi untuk melakukan encode -> penting untuk melakukan compression
void encode()
{
    int hitung_ukuranteks = 0, hitung_ukurankode = 0,  hitung_proses = 0;
    int  i,j, panjang, chr, strings, m_panjang_akhir, kode_pointer;
    unsigned char  bufferkode[17], x, cmpr;
    
    for (j = ukuran_buf + 1; j <= ukuran_buf + 256; j++) child_kanan[j] = Idx_Akar;
    for (j = 0; j < ukuran_buf; j++) ortu[j] = Idx_Akar;
    //isi_tree();  	
    strings = 0;  
    bufferkode[0] = 0; 
    /*bufferkode [1..16] menyimpan delapan unit kode, dan
        bufferkode [0] berfungsi sebagai delapan flag, "1" mewakili unit tersebut
        adalah huruf tanpa kode (1 byte), "0" pasangan posisi-dan-panjang
        (2 byte). Jadi, delapan unit membutuhkan paling banyak 16 byte kode. */ 
    kode_pointer = x = 1;

    chr = ukuran_buf - lim;
    for (i = strings; i < (ukuran_buf - lim); i++) bufferteks[i] = 0; 
    for (panjang = 0; panjang < lim && (read(file_dalam, &cmpr, 1) != 0); panjang++)
        bufferteks[chr + panjang] = cmpr;  
    if ((hitung_ukuranteks = panjang) == 0) return;  
    //Masukkan string 'lim', masing-masing dimulai dengan satu atau lebih karakter 'spasi'. 
    for (i = 1; i <= lim; i++) masukkanSimpul(chr - i); 
    //masukkan isi string tadi ke node
    masukkanSimpul(chr);  

    do{
        if (m_panjang > panjang) m_panjang = panjang;  
        if (m_panjang <= enc_poslength) {
            m_panjang = 1;     
            bufferkode[0] |= x;  
            bufferkode[kode_pointer++] = bufferteks[chr]; 
        } else {
            bufferkode[kode_pointer++] = (unsigned char) m_posisi;
            bufferkode[kode_pointer++] = (unsigned char)
                (((m_posisi >> 4) & 0xf0)
             | (m_panjang - (enc_poslength + 1))); 
        }
        if ((x <<= 1) == 0) { 
            for (i = 0; i < kode_pointer; i++) //kirim 8 unit
            write(file_luar, &bufferkode[i], 1);
            bufferkode[0] = 0; 
            hitung_ukurankode = hitung_ukurankode+kode_pointer;
            kode_pointer = x = 1;
        }
        m_panjang_akhir = m_panjang;
        for (i = 0; i < m_panjang_akhir &&
                (read(file_dalam, &cmpr, 1) != 0) ; i++) {
            hapusSimpul(strings);     //hapus strings yg lama   
            bufferteks[strings] = cmpr;    //baca byte baru
            //lakukan extend pada buffer agar perbandingannya lebih mudah dilakukan
            if (strings < lim - 1) bufferteks[strings + ukuran_buf] = cmpr; 
            strings = (strings + 1) & (ukuran_buf - 1);  
            chr = (chr + 1) & (ukuran_buf - 1);
            masukkanSimpul(chr);    
        }
        if ((hitung_ukuranteks = hitung_ukuranteks+i) > hitung_proses) {
            hitung_proses = hitung_proses+1024;
            //merekam proses tiap kali ukuran teks melebihi kelipatan 1024 
        }
        while (m_panjang_akhir>i++ ) {    //setelah teks akhir
            hapusSimpul(strings);              //delete aja    
            strings = (strings + 1) & (ukuran_buf - 1);  
            chr = (chr + 1) & (ukuran_buf - 1);
            if (--panjang) masukkanSimpul(chr);  //buffer tidak kosong
        }
    } while (panjang > 0);   //sampe panjang yg diproses =0
    
    if (kode_pointer > 1) {      //mengirim kode yg ada
        for (i = 0; i < kode_pointer; i++) write(file_luar, &bufferkode[i], 1);
        hitung_ukurankode = hitung_ukurankode+kode_pointer;
    }
}
