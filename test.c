#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void zera_vetor(char *vet){
    for(int i=0; i<20; i++){
        vet[i] = 0;
    }
}

int main () {

    int fd, retorno;
    char buffer[21] = {0};
    buffer[20] = '\0';

    fd = open("/dev/mychardev", O_RDWR);
    if (fd == -1) {
        printf("erro abrindo o arquivo\n");
    }

    // Teste 1:

    write(fd, "Feliz Natal!", 12);
    retorno = read(fd, buffer, 20);
    printf("buffer: %s\n", buffer);
    printf("retorno: %d\n", retorno);
    zera_vetor(buffer);
    

    // Teste 2:

    write(fd, "Tenha um mal natal!", 19);
    
    retorno = read(fd, buffer, 20);
    printf("buffer: %s\n", buffer);
    printf("retorno: %d\n", retorno);
    zera_vetor(buffer);


    // Teste 3:

    write(fd, "Tenha um mal natal! E um pessimo ano novo!", 42);
    
    retorno = read(fd, buffer, 20);
    printf("buffer: %s\n", buffer);
    printf("retorno: %d\n", retorno);
    zera_vetor(buffer);
    
    
    // Teste 4:
    
    retorno = read(fd, buffer, 10);
    printf("buffer: %s\n", buffer);
    printf("retorno: %d\n", retorno);
    zera_vetor(buffer);
    
    if(close(fd) != 0) {
        printf("erro fechando arquivo\n");
    }
    return 0;
}

