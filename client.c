//Joud Ghaben 144206
//Sara Qasem 143884
//Siwar Daraiseh 148536
//Lama Obeidat 146265


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#define BUFFER_SIZE 1024
int sockfd;


// Signal handler for SIGQUIT
void sigquit_handler(int sig) {
    printf("\nGoodbye!\n");
    close(sockfd);
    exit(0);
}




//menu function
void menu() {
    printf("\nSelect an operation:\n");
    printf("1. Count words (OP1)\n");
    printf("2. Count vowels (OP2)\n");
    printf("3. Count special characters (OP3)\n");
    printf("4. Find the longest word (OP4)\n");
    printf("5. Find repeated words (OP5)\n");
    printf("Enter your choice (1-5): ");
}





int main(int argc, char *argv[]) {


    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }



    //declarations
    struct sockaddr_in servaddr;
    char input_line[BUFFER_SIZE];
    char buffer[BUFFER_SIZE]; 
    char operation[10];
    int choice;




    //open input.txt file and read the lines
    FILE *file = fopen(argv[3], "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }



    

    // Setup signal handler for SIGQUIT
    signal(SIGQUIT, sigquit_handler);




    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }




   
    //setup server address structure
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    servaddr.sin_port=htons(atoi(argv[2]));




    // Connect to the server
    int connfd=connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
    if(connfd<0){
        perror("connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server at %s:%s\n", argv[1], argv[2]);




    


    // Process each line in the input file
    while (fgets(input_line, sizeof(input_line), file)) {
        input_line[strcspn(input_line, "\n")] = '\0'; // Remove trailing newline
        while (1) {
        menu();

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number between 1 and 5.\n");

        // Clear the input buffer
        while (getchar() != '\n');  // Discard remaining characters in the input buffer
        continue;
    }

    switch (choice) {
        case 1:
            strcpy(operation, "OP1");
            break;
        case 2:
            strcpy(operation, "OP2");
            break;
        case 3:
            strcpy(operation, "OP3");
            break;
        case 4:
            strcpy(operation, "OP4");
            break;
        case 5:
            strcpy(operation, "OP5");
             break;
        case 6: strcpy(operation,"OP6");
            break;
        default:
            printf("Invalid option. Please try again.\n");
            continue;
    }
    break;
}


        // Prepare the request
        snprintf(buffer, sizeof(buffer), "%.*s %.*s", BUFFER_SIZE / 4, operation, BUFFER_SIZE / 2, input_line);



        // Send the request to the server
        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            break;
        }





        // Receive the response from the server
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer), 0) <= 0) {
            perror("Receive failed");
            break;
        }
        printf("Server response: %s\n", buffer);
    }




    printf("End of input file. Exiting...\n");
    fclose(file);
    close(sockfd);

    return 0;
}
