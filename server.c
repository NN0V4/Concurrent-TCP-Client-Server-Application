#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#define ListenQ 4
#define BUFFER_SIZE 1024
#define LogFile "log.txt"




//creating log file contains the user logs
void log_request(const char *client_ip, const char *operation, const char *status) {
    FILE *log_file = fopen(LogFile, "a");
    if (!log_file) {
        perror("Failed to open log file");
        return;
    }
    time_t now = time(NULL);
    fprintf(log_file, "%s | %s | %s | %s\n", ctime(&now), client_ip, operation, status);
    fclose(log_file);
}





// signal handler to cleanup the child processes
void child_handler(int sig) {
    pid_t pid;
    int stat;
    
    while ((pid=waitpid(-1, &stat, WNOHANG)) > 0)
    printf("Child %d terminated.\n",pid);
    
}







// String operation functions
int count_words(const char *str) {
    int count = 0;
    char space = ' ';
    for (int i = 0; str[i] != '\0'; i++) {
        if (isspace(space) && !isspace(str[i]))
            count++;
        space = str[i];
    }
    return count;
}




int count_vowels(const char *str) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (strchr("aeiouAEIOU", str[i]))
            count++;
    }
    return count;
}

int special_chars(const char *str) {
    int count = 0;
    while (*str) {
        if (!isalnum(*str) && !isspace(*str)) count++;
        str++;
    }
    return count;
}

void longest_word(const char *str, char *longest) {
    const char *start = str, *end;
    size_t max_length = 0;
    while (*start) {
        while (isspace(*start)) start++;
        end = start;
        while (*end && !isspace(*end)) end++;
        size_t length = end - start;
        if (length > max_length) {
            max_length = length;
            strncpy(longest, start, length);
            longest[length] = '\0';
        }
        start = end;
    }
}

void repeated_words(const char *str, char *result) {
    char *copy = strdup(str);  // Make a copy of the input string
    char *words[BUFFER_SIZE];  // Array to store unique words
    int count[BUFFER_SIZE] = {0};  // Array to store counts of each word
    int word_index = 0;

    char *token = strtok(copy, " .,!?\n");  // Tokenize the input string
    while (token != NULL) {
        // Check if the word already exists in the array
        int found = 0;
        for (int i = 0; i < word_index; i++) {
            if (strcmp(words[i], token) == 0) {
                count[i]++;
                found = 1;
                break;
            }
        }

        // If not found, add it to the array
        if (!found) {
            words[word_index] = strdup(token);
            count[word_index] = 1;
            word_index++;
        }

        token = strtok(NULL, " .,!?\n");
    }

    // Construct the result string with repeated words
    strcpy(result, "");
    for (int i = 0; i < word_index; i++) {
        if (count[i] > 1) {
            strcat(result, words[i]);
            strcat(result, " ");
        }
    }

    // Free allocated memory
    for (int i = 0; i < word_index; i++) {
        free(words[i]);
    }
    free(copy);
}






// Function to handle the client request
void handle_client(int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    int client_port = ntohs(client_addr.sin_port);
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("New connection from %s: %d \n", client_ip,client_port);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) <= 0) break;

        char operation[10] = {0}, input[BUFFER_SIZE] = {0}, response[BUFFER_SIZE] = {0};
        sscanf(buffer, "%s %[^\n]", operation, input); // Read operation and input

        int op = atoi(operation);

        size_t input_len = strlen(input);
        if (input_len >= BUFFER_SIZE / 2) {
            input[BUFFER_SIZE / 2 - 1] = '\0'; // Truncate input safely
        }

        if (strcmp(operation, "OP1") == 0) {
            snprintf(response, BUFFER_SIZE, "OP1(%.500s) = %d", input, count_words(input));
        } else if (strcmp(operation, "OP2") == 0) {
            snprintf(response, BUFFER_SIZE, "OP2(%.500s) = %d", input, count_vowels(input));
        } else if (strcmp(operation, "OP3") == 0) {
            snprintf(response, BUFFER_SIZE, "OP3(%.500s) = %d", input, special_chars(input));
        } else if (strcmp(operation, "OP4") == 0) {
            char longest[BUFFER_SIZE / 2] = {0};
            longest_word(input, longest);
            snprintf(response, BUFFER_SIZE, "OP4(%.500s) = %.500s", input, longest);
        } else if (strcmp(operation, "OP5") == 0) {
            char repeated[BUFFER_SIZE / 2] = {0};
            repeated_words(input, repeated);
            snprintf(response, BUFFER_SIZE, "OP5(%.500s) = %.500s", input, repeated);
        } 
        else {
            snprintf(response, BUFFER_SIZE, "Unsupported operation: %.500s", operation);
        }

        send(client_socket, response, strlen(response), 0);
        log_request(client_ip, operation, strstr(response, "Unsupported") ? "Failed" : "Success");
    }

    close(client_socket);
    exit(0);
}




// Main function
int main(int argc, char *argv[]) {
    int PORT;
    if (argc==2) {
        int specified_port = atoi(argv[1]);
        if (specified_port > 0 && specified_port <= 65535) {
            PORT = specified_port;
        } else {
            fprintf(stderr, "Invalid port number: %d.\n", specified_port);
        }
        
        }
        else{
        
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    
    
    //declarations
    struct sockaddr_in cliaddr,servaddr;
    pid_t pid;
    socklen_t len;
    int  sockfd,connfd,b_retval,l_retval,retval;



    // Create server socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd <0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }




    // Set up the server address structure
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);




    // Bind the socket to the address and port
    b_retval=bind(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
    if (b_retval<0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }




    // Listen for incoming connections
    l_retval=listen(sockfd, ListenQ);
    if (l_retval<0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);



    //call the signal handler
    signal(SIGCHLD, child_handler);



    
    // Accept and handle client connections
    for(; ; ){
        len=sizeof(cliaddr);
        connfd=accept(sockfd,(struct sockaddr *) &cliaddr,&len);
        if(connfd<0){
            perror("connection faild!");
            exit(EXIT_FAILURE);
        }
        
        if((pid=fork())==0){
            retval=close(sockfd);
            if(retval<0){
                perror("close failed");
                exit(EXIT_FAILURE);
            }

            handle_client(connfd, cliaddr);

            exit(0);

        }
        retval=close(connfd);
            if(retval<0){
                perror("close failed");
                exit(EXIT_FAILURE);
            }

    }
    retval=close(sockfd);
            if(retval<0){
                perror("close failed");
                exit(EXIT_FAILURE);
            }
    return 0;
}

