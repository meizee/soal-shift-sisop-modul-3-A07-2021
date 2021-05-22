# soal-shift-sisop-modul-3-A07-2021

Soal Shift Modul 3 |  Sistem Operasi 2021

1. Zahra Dyah Meilani 05111940000069
2. Ghifari Astaudi' Ukmullah 05111940000012
3. Fairuz Hasna Rofifah 05111940000003

## SOAL NO 1
Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. 
Di dalam proyek itu, Keverk diminta: 
## 1A
Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama `akun.txt` dengan format :
### akun.txt`
```
id:password
id2:password2
```
## 1B
Sistem memiliki sebuah database yang bernama files.tsv. Isi dari files.tsv ini adalah path file saat berada di server, publisher, dan tahun publikasi. Setiap penambahan dan penghapusan file pada folder file yang bernama  FILES pada server akan memengaruhi isi dari files.tsv. Folder FILES otomatis dibuat saat server dijalankan. Tidak hanya itu, 

## 1 C
Keverk juga diminta membuat fitur agar client dapat menambah file baru ke dalam server. Direktori FILES memiliki struktur direktori di bawah ini : 
### Direktori FILES
```
File1.ekstensi
File2.ekstensi
```
Pertama client mengirimkan input ke server dengan struktur sebagai berikut :
Contoh Command Client :
```
add
```

Output Client console:
```
Publisher:
Tahun Publikasi:
Filepath:
```
Kemudian, dari aplikasi client akan dimasukan data buku tersebut (perlu diingat bahwa Filepath ini merupakan path file yang akan dikirim ke server). Lalu client nanti akan melakukan pengiriman file ke aplikasi server dengan menggunakan socket. Ketika file diterima di server, maka row dari files.tsv akan bertambah sesuai dengan data terbaru yang ditambahkan.

## 1D
Dan client dapat mendownload file yang telah ada dalam folder FILES di server, sehingga sistem harus dapat mengirim file ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut. 

Contoh Command client
```
download TEMPfile.pdf
```
## 1E
Setelah itu, client juga dapat menghapus file yang tersimpan di server. Akan tetapi, Keverk takut file yang dibuang adalah file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus.

Contoh Command Client:
```
delete TEMPfile.pdf
```
## 1F
Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Output dari perintah tersebut keluar dengan format. 
Contoh Command Client :
```
see
```

Contoh Format Output pada Client:
```
Nama:
Publisher:
Tahun publishing:
Ekstensi File : 
Filepath :

Nama:
Publisher:
Tahun publishing:
Ekstensi File : 
Filepath : 
```
## 1G
Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f.
Contoh Client Command:
```
find TEMP
```
## 1H
Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama running.log. Contoh isi dari log ini adalah
### running.log
```
Tambah : File1.ektensi (id:pass)
Hapus : File2.ektensi (id:pass)
```
Struktur Direktori:
```
├── Client
│   ├── client.c
│   ├── File2.extensi
│   └── File1.extensi
└── Server
    ├── akun.txt
    ├── files.tsv
    ├── server.c
    ├── running.log
    └── FILES
            ├── File2.extensi
            └── File1.ekstensi
```

## **2.**
 
### **2a. Perkalian Matriks**

> Source Code 
 > **[soal2a.c](https://github.com/hasnarof/soal-shift-sisop-modul-2-A07-2021/blob/main/soal1/soal1.c)**

>"Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka)."

Pertama, kita inisasi terlebih dahulu variabel global yang dibutuhkan, `tid` yang digunakan untuk thread, `a_glob` dan `b_glob` untuk menyimpan variabel matriks a dan b, dan `result_matrix` sebagai matriks hasil perkalian.
```C
pthread_t tid[4];
long long int a_glob[4][3], b_glob[3][6];
long long int (*result_matrix)[6];
```

Fungsi untuk melakukan perhitungan perkalian dari matriks dengan menggunakan thread sebanyak empat. Setiap thread akan menghitung nilai sel matriks dalam satu barisnya, sesuai aturan perkalian matriks. 

```C
void *mult_matrix(void *arg){
    pthread_t id = pthread_self();

    if(pthread_equal(id, tid[0])) {
        for(int i=0; i<6; i++) {
            result_matrix[0][i] = (a_glob[0][0] * b_glob[0][i]) + (a_glob[0][1] * b_glob[1][i]) + (a_glob[0][2] * b_glob[2][i]);
        }
    }

    else if(pthread_equal(id, tid[1])) {
        for(int i=0; i<6; i++) {
            result_matrix[1][i] = a_glob[1][0] * b_glob[0][i] + a_glob[1][1] * b_glob[1][i] + a_glob[1][2] * b_glob[2][i];
        }
    }

    else if(pthread_equal(id, tid[2])) {
        for(int i=0; i<6; i++) {
            result_matrix[2][i] = a_glob[2][0] * b_glob[0][i] + a_glob[2][1] * b_glob[1][i] + a_glob[2][2] * b_glob[2][i];
        }
    }

    else{
        for(int i=0; i<6; i++) {
            result_matrix[3][i] = a_glob[3][0] * b_glob[0][i] + a_glob[3][1] * b_glob[1][i] + a_glob[3][2] * b_glob[2][i];
        }
    }
}
```

Dalam main, kita inisiasi variabel-variabel yang dibutuhkan untuk melakukan shared memory, `key`, `shmid`, dan `result_matrix` (yang mana merupakan matriks hasil perkalian yang nantinya digunakan di soal 2b.)
```C
    key_t key = 1112;
    int shmid = shmget(key, sizeof(int[4][6]), 0666 | IPC_CREAT); 
    result_matrix = shmat(shmid,NULL,0);
```

Dan inisiasi variabel `matrix_a` yang berukuran 4x3 dan `matrix_b` yang berukuran 3x6.
```C
long long int matrix_a[4][3] = {
    {4, 1, 4},
    {2, 1, 3},
    {4, 2, 2},
    {1, 1, 4}
}; 
    
long long int matrix_b[3][6] ={
    {2, 1, 3, 2, 0, 3},
    {1, 4, 4, 0, 0, 2},
    {1, 1, 0, 1, 2, 1}
};
```

Selanjutnya berikut ini untuk menampilkan setiap nilai matriks A dan matriks B:
```C
printf("Matrix a:\n");
    for(int i=0; i<4; i++) {
        for(int j=0; j<3; j++){
            a_glob[i][j] = matrix_a[i][j];
            printf("%4lld ", a_glob[i][j]);
        }
        printf("\n");
    }

    printf("\nMatrix b:\n");
    for(int i=0; i<3; i++) {
        for(int j=0; j<6; j++){
            b_glob[i][j] = matrix_b[i][j];
            printf("%4lld ", b_glob[i][j]);
        }
        printf("\n");
    }


Setelah itu, kita perlu membuat thread sebanyak empat untuk melakukan perkalian matrix dengan fungsi `mult_matrix`.
```C
for(int i=0; i<4; i++)
    {
        pthread_create(&(tid[i]), NULL, &mult_matrix, NULL); 
    }
````

Menggabungkan semua thread
```C
pthread_join(tid[0], NULL);
pthread_join(tid[1], NULL);
pthread_join(tid[2], NULL);
pthread_join(tid[3], NULL);
```

Print hasil matriks perkalian yang telah dihitung
```C
    printf("\nResult Matrix:\n");
    for(int i=0; i<4; i++){
        for(int j=0; j<6; j++)
        {
            printf("%4lld ", result_matrix[i][j]);
        }
        printf("\n");
    }
```

Menyimpan hasil perkalian matriks di shared memory
```C
shmdt(result_matrix);
exit(0);
```

Screenshot:
![2a]()

### **2b. Operasi Matriks dengan Shared Memory**

>"Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang a da. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matri(dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel)."


Print hasil matriks perkalian yang telah dihitung
```C

```

Screenshot:
![2b](Screenshot/ss%20soal1/1cd.png)


### **2c. Menampilkan Proses dengan Pipe**

>"Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)"

Menyimpan hasil perkalian matriks di shared memory
```C
shmdt(result_matrix);
exit(0);
```

### **Screenshot***:
![2c]()
