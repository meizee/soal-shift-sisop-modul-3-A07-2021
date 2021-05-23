#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#define tool 512-212*1
#define terima 2

int sockets = -1;
bool sockets2 = false;
const int ceksize = sizeof(int) * tool;
const int input = sizeof(int)* tool * 1;
char validator[terima][300]; 
const int SIZE_BUFFER = sizeof(char) * 300;

int create_socket()
{
    struct sockaddr_in saddr;
    int ld = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int res_val, opt = 1;
    
    if (ld == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(ld, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7000);
    saddr.sin_addr.s_addr = INADDR_ANY;
    res_val = bind(ld, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (res_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(ld);
        exit(EXIT_FAILURE);
    }
    res_val = listen(ld, 5);
    if (res_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(ld);
        exit(EXIT_FAILURE);
    }
    return ld;
}
void *inti(void *argv);
void see(char *buzz, int ld, bool isFind);
void download(char *FILENM, int ld);
void delete(char *FILENM, int ld);
void daftar(char *messages, int ld);
void add(char *messages, int ld);
void runninglog(char *command, char *FILENM);
bool downloaded(FILE *file, char *FILENM);
bool registered(FILE *file, char *id);
void dividingfile(char *FileP, char *nameFILE, char *ext);
int valid(int ld, char *id, char *pass);
void login(char *messages, int ld);
char *CeckNameFile(char *filePath);
int take_input(int ld, char *prompt, char *arah);
int sends(int ld, char *FILENM);
int INPUTFile(int ld, char *DIRname, char *targetfileN);
bool loginsuc(FILE *file, char *id, char *pass);

void *inti(void *argv)
{
    int ld; 
    ld = *(int *) argv;
    char command[300];
    chdir("/home/hasna/sisop/praktikum3-1/Server");

    while (recv(ld, command, 300, MSG_PEEK | MSG_DONTWAIT) != 0) {
        if (ld != sockets) {
            if (take_input(ld, "\nWelcome in Server A07 Please choice this command\n 1. register\n2. login\nYour Choice: ", command) == 0) break;
            write(ld, "\n", SIZE_BUFFER);
            if (strcmp(command, "register") == 0) 
            {
                daftar(command, ld);
            } 
            else if (strcmp(command, "login") == 0) 
            {
                login(command, ld);
            } 
            else 
            {
                send(ld, "Error: Invalid command\n", SIZE_BUFFER, 0);
            }
        } else 
        { 
            char prompt[300];
            strcpy(prompt, "\nPlease choose the following command\n1. add\n2. downlod\n3. delete\n4. see\n5. find Your Choice = ");
            if (take_input(ld, prompt, command) == 0) break;
            write(ld, "\n", SIZE_BUFFER);
            if (strcmp(command, "see") == 0) 
            {
                see(command, ld, false);
            } 
            else if (strcmp(command, "add") == 0) 
            {
                  add(command, ld);
            }
            else 
            {
                //cekking 
                char *temp1 = strtok(command, " ");
                char *temp2 = strtok(NULL, " ");
                if (!temp2) {
                    send(ld, "The command that was written does not exist\n", SIZE_BUFFER, 0);
                } 
                else if (strcasecmp(temp1, "delete") == 0) {
                    delete(temp2, ld);
                    
                } 
                else if (strcasecmp(temp1, "download") == 0) {
                    download(temp2, ld);
                }
                else if (strcasecmp(temp1, "find") == 0) {
                    see(temp2, ld, true);
                }
                else {
                    send(ld, "Error: Invalid command\n", SIZE_BUFFER, 0);
                }
            }
        }
        sleep(0.001);
    }
    if (ld == sockets) 
    {
        sockets = -1;
    }
    printf("The connection has been lost in the port: %d\n", ld-3);
    close(ld);
}

void delete(char *FILENM, int ld)
{
    //buka file
    FILE *file = fopen("files.tsv", "a+");
    char bid[300], currFilePath[300], publisher[300], year[300];
    if (downloaded(file, FILENM)) {
        rewind(file);
        FILE *tmp_fp = fopen("temp.tsv", "a+");
        //buat sebuah temp supaya pada saat pertukaran data tidak berubah2
        while (fgets(bid, SIZE_BUFFER, file)) {
            sscanf(bid, "%s\t%s\t%s", currFilePath, publisher, year);
            if (strcmp(CeckNameFile(currFilePath), FILENM) != 0) { 
                fprintf(tmp_fp, "%s", bid);
            }
            memset(bid, 0, SIZE_BUFFER);
        }
        fclose(tmp_fp);
        fclose(file);
        remove("files.tsv");
        rename("temp.tsv", "files.tsv");
        char deletedFileName[300];
        sprintf(deletedFileName, "FILES/%s", FILENM);
        char newFileName[300];
        sprintf(newFileName, "FILES/old-%s", FILENM);
        rename(deletedFileName, newFileName);
        send(ld, "This file was successfully deleted\n", SIZE_BUFFER, 0);
        runninglog("delete", FILENM);
    } 
    else {
        send(ld, "File failed to download\n", SIZE_BUFFER, 0);
        fclose(file);
    }
}

void see(char *buzz, int ld, bool isFind)
{
    int tambah = 0;
    FILE *src = fopen("files.tsv", "r");
    if (!src) {
        write(ld, "Files.tsv not found\n", SIZE_BUFFER);
        return;
    }

    char temp[300 + 85], nameFILE[300/3], ext[5],
        FileP[300/3], publisher[300/3], year[10];
        
    while (fscanf(src, "%s\t%s\t%s", FileP, publisher, year) != EOF) {
        dividingfile(FileP, nameFILE, ext);
        if (isFind && strstr(nameFILE, buzz) == NULL) continue;
        tambah++;

        sprintf(temp, 
            "Nama: %s\nPublisher: %s\nTahun publishing: %s\nEkstensi File: %s\nFilepath: %s\n\n",
            nameFILE, publisher, year, ext, FileP
        );
        write(ld, temp, SIZE_BUFFER);
        sleep(0.001);
    }
    if(tambah == 0) {
        if (isFind) write(ld, "the command is not found in files.tsv\n", SIZE_BUFFER);
        else write(ld, "Data is not found in database files.tsv\n", SIZE_BUFFER);
    } 
    fclose(src);
}

void add(char *messages, int ld)
{
    char *DIR = "FILES";
    char publisher[300], year[300], client_path[300];
    sleep(0.001);
    if (take_input(ld, "Publisher: ", publisher) == 0) return;
    if (take_input(ld, "Tahun Publikasi: ", year) == 0) return;
    if (take_input(ld, "Filepath: ", client_path) == 0) return;

    FILE *file = fopen("files.tsv", "a+");
    char *fileName = CeckNameFile(client_path);

    if (downloaded(file, fileName)) {
        send(ld, "file that you uploaded already exists\n", SIZE_BUFFER, 0);
    } else {
        send(ld, "Start to send file\n", SIZE_BUFFER, 0);
        mkdir(DIR, 0777);
        if (INPUTFile(ld, DIR, fileName) == 0) {
            fprintf(file, "%s\t%s\t%s\n", client_path, publisher, year);
            printf("File send successfully\n");
            runninglog("add", fileName);
        } else {
            printf("Error occured when receiving file\n");
        }
    }
    fclose(file);
}

void download(char *FILENM, int ld)
{
    FILE *file = fopen("files.tsv", "a+");
    if (downloaded(file, FILENM)) {
        sends(ld, FILENM);
    } else {
        send(ld, "File is not found\n", SIZE_BUFFER, 0);
    }
    fclose(file);
}

void login(char *messages, int ld)
{
    if (sockets != -1) {
        send(ld, "Server is busy. Please wait....\n", SIZE_BUFFER, 0);
        return;
    }
    //buka akun
    char id[300], pass[300];
    FILE *file = fopen("akun.txt", "a+");
    if (valid(ld, id, pass) != 0) {
        if (loginsuc(file, id, pass)) {
            send(ld, "Login success!\n", SIZE_BUFFER, 0);
            sockets = ld;
            strcpy(validator[0], id);
            strcpy(validator[1], pass);
        } else {
            send(ld, "Username or Password is wrong!\n", SIZE_BUFFER, 0);
        }
    }
    fclose(file);
}

void daftar(char *messages, int ld)
{
    char id[300], pass[300];
    FILE *file;
    file = fopen("akun.txt", "a+");

    if (valid(ld, id, pass) != 0) {
        if (registered(file, id)) {
            send(ld, "The username already exists\n", SIZE_BUFFER, 0);
        } else {
            fprintf(file, "%s:%s\n", id, pass);
            send(ld, "Account registration is successful\n", SIZE_BUFFER, 0);
        }
    }
    fclose(file);
}

void runninglog(char *command, char *FILENM)
{
    FILE *file;
    file = fopen("running.log", "a+");
    command = (strcmp(command, "add") == 0) ? "Tambah" : "Hapus";
    fprintf(file, "%s : %s (%s:%s)\n", command, FILENM, validator[0], validator[1]);
    fclose(file);
}

void dividingfile(char *FileP, char *nameFILE, char *ext)
{
    char *temp;
    if (temp = strrchr(FileP, '.')) strcpy(ext, temp + 1);
    else strcpy(ext, "-");
    strcpy(nameFILE, CeckNameFile(FileP));
    strtok(nameFILE, ".");
}
int sends(int ld, char *FILENM)
{
    char buzz[300] = {0};
    int res_val;
    printf("Send File %s ke client\n", FILENM);
    strcpy(buzz, FILENM);
    sprintf(FILENM, "FILES/%s", buzz);
    FILE *file;
    file = fopen(FILENM, "r");

    if (!file) {
        printf("File is not found\n");
        send(ld, "File is not found\n", SIZE_BUFFER, 0);
        return -1;
    }
    send(ld, "Start to receive file\n", SIZE_BUFFER, 0);
    send(ld, buzz, SIZE_BUFFER, 0);

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    rewind(file);
    sprintf(buzz, "%d", size);
    send(ld, buzz, SIZE_BUFFER, 0);

    while ((res_val = fread(buzz, 1, 300, file)) > 0) {
        send(ld, buzz, res_val, 0);
    }
    recv(ld, buzz, 300, 0);
    printf("File sent successfully\n");
    fclose(file);
    return 0;
}

int INPUTFile(int ld, char *DIRname, char *targetfileN)
{
    int res_val, size;
    char buzz[300] = {0}, in[1];

    res_val = recv(ld, buzz, 300, 0);
    if (res_val == 0 || strcmp(buzz, "File was found") != 0) {
        if (res_val == 0) printf("Connection to cilent was disconnect\n");
        else puts(buzz);
            return -1;
    }
    recv(ld, buzz, SIZE_BUFFER, 0);
    size = atoi(buzz);

    printf("File sent %s to server\n", targetfileN);
    sprintf(buzz, "%s/%s", DIRname,targetfileN);
    FILE *file = fopen(buzz, "w+");

    while (size-- > 0) {
        if ((res_val = recv(ld, in, 1, 0)) < 0)
            return res_val;
            fwrite(in, 1, 1, file);
    }
    res_val = 0;
    printf("File sent successfully to server\n");
    fclose(file);
    return res_val;
}

char *CeckNameFile(char *filePath)
{
    char *res = strrchr(filePath, '/');
    if (res) 
        return res + 1;
    else 
        return filePath;
}


int valid(int ld, char *id, char *pass)
{
    if (take_input(ld, "Input your Username = ", id) == 0) return 0;
    if (take_input(ld, "Input your Password = ", pass) == 0) return 0;
    return 1;
}

bool loginsuc(FILE *file, char *id, char *pass)
{
    char bid[300], input[300];
    sprintf(input, "%s:%s", id, pass);
    while (fscanf(file, "%s", bid) != EOF) {
        if (strcmp(bid, input) == 0) return true;
    }
    return false;
}

int take_input(int ld, char *prompt, char *arah)
{
    send(ld, prompt, SIZE_BUFFER, 0);
    int jumlah, res_val;
    ioctl(ld, FIONREAD, &jumlah);
    jumlah /= 300;
    for (int i = 0; i <= jumlah; i++) {
        res_val = recv(ld, arah, 300, 0);
        if (res_val == 0) 
            return res_val;
    }
    while (strcmp(arah, "") == 0) {
        res_val = recv(ld, arah, 300, 0);
        if (res_val == 0) return res_val;
    }
    printf("Command Client = %s\n", arah);
    return res_val;
}

bool downloaded(FILE *file, char *FILENM)
{
    char bid[300], *temp1;
    while (fscanf(file, "%s", bid) != EOF) {
        temp1 = CeckNameFile(strtok(bid, "\t"));
        if (strcmp(temp1, FILENM) == 0) 
            return true;
    }
    return false;
}

bool registered(FILE *file, char *id)
{
    char bid[300], *temp1;
    while (fscanf(file, "%s", bid) != EOF) {
        temp1 = strtok(bid, ":");
        if (strcmp(temp1, id) == 0) 
            return true;
    }
    return false;
}
int main(int argc ,char const *argv1[])
{
    socklen_t addresslen;
    struct sockaddr_in new_address;
    pthread_t tid;
    char buzz[300], argv[300 + terima];
    int new_fd, res_val, server_fd = create_socket();

    while (1) {
        new_fd = accept(server_fd, (struct sockaddr *)&new_address, &addresslen);
        if (new_fd >= 0) {
            printf("the connection is connected to the port: %d\n", new_fd-3);
            pthread_create(&tid, NULL, &inti, (void *) &new_fd);
        } else {
            fprintf(stderr, "connection failed %s\n", strerror(errno));
        }
    }
    return 0;
}
