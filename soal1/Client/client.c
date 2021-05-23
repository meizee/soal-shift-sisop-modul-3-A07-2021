#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#define fk 0.001
#define tool 512-(212*1)

const int maincheck = sizeof(char) * tool;
const int sizes = sizeof(int) * tool;
const int input = sizeof(int)* tool * 1;
char cmd1[tool];
bool check = false;
bool pstn = true;
bool into = false;

void *InputCheck(void *fdc);
void *OutputCheck(void *fdc);
void activeserver(int ld, char *input);
void sent(int ld);
void Print(int ld);

int create_socket()
{
    struct sockaddr_in s_address;
    int ld = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int res_val, opt = 1;
    struct hostent *local_host; 
    if (ld == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(ld, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    s_address.sin_family = AF_INET;
    s_address.sin_port = htons(7000);
    local_host = gethostbyname("127.0.0.1");
    s_address.sin_addr = *((struct in_addr *)local_host->h_addr);
    res_val = connect(ld, (struct sockaddr *)&s_address, sizeof(struct sockaddr_in));
    if (res_val == -1) {
        fprintf(stderr, "connect failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ld;
}

void *OutputCheck(void *fdc) 
{
  
    int ld;
    char Input[tool] = {0};
    chdir("/home/hasna/sisop/praktikum3-1/Client");
    ld = *(int *) fdc;

    while (1) 
    {
        memset(Input, 0, maincheck);
        activeserver(ld, Input);
        printf("%s", Input);
        if (strcmp(Input, "Filepath: ") == 0) {
            check = true;
        } else if (strcmp(Input, "Start to send file\n") == 0) {
            sent(ld);
            check = false;
        } else if (strcmp(Input, "file that you uploaded already exists\n") == 0) {
            check = false;
        } else if (strcmp(Input, "Start to send file\n") == 0) {
            Print(ld);
        } 
        fflush(stdout);
    }
}

void *InputCheck(void *fdc)
{
    int ld;
    char Input[tool] = {0};
    chdir("/home/hasna/sisop/praktikum3-1/Client");
    ld = *(int *) fdc;
    while (1) {
        gets(Input);
        send(ld, Input, maincheck, 0);
        if (check) {
            strcpy(cmd1, Input);
    }
    }
}
void activeserver(int ld, char *input)
{
    if (recv(ld, input, tool, 0) == 0) {
        printf("The server has expired\n");
        exit(EXIT_SUCCESS);
    }
}

void sent(int ld)
{
    
    int res_val;
    FILE *sends;
    char buf[tool] = {0};
    printf("Send file %s to server\n", cmd1);
    sends = fopen(cmd1, "r");
   
    if (sends) {
         int sizess;
        send(ld, "Files found", maincheck, 0);
        fseek(sends, 0L, SEEK_END);
        sizess = ftell(sends);
        rewind(sends);
        sprintf(buf, "%d", sizess);
        send(ld, buf, maincheck, 0);
        while ((res_val = fread(buf, 1, 1, sends)) > 0) 
        {
            send(ld, buf, 1, 0);
        } 
        printf("File has been sent\n");
        fclose(sends);
    } 
    else 
    {
        printf("File is not found\n");
        send(ld, "File is not found", maincheck, 0);
    }
}

void Print(int ld)
{
    char buff[tool] = {0};
    int res_val, sizess;
    FILE *sends;
    recv(ld, buff, tool, 0);
    res_val = recv(ld, buff, tool, 0);
    sizess = atoi(buff);
    sends = fopen(buff, "w+");
    while (sizess > 0) {
        res_val = recv(ld, buff, tool, 0);
        fwrite(buff, 1, res_val, sends);
        memset(buff, 0, maincheck);
        sizess -= res_val;
    }
    puts("File sent successfully to server");
    send(ld, "File has been sent", maincheck, 0);
    fclose(sends);
}

int main(int argc, char const *argv[])
{
    pthread_t tid[2];
    int fdc = create_socket();
    pthread_create(&(tid[0]), NULL, &Output, (void *) &client_fd);
    pthread_create(&(tid[1]), NULL, &InputCheck, (void *) &fdc);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    close(fdc);
    return 0;
}
