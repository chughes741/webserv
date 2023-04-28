// Server side C program to demonstrate Socket programming
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <new>
#include <memory>
#include <functional>

#define PORT 8080
int main(int argc, char const *argv[])
{
    (void) argc;
    (void) argv;

    FILE *file = fopen("index.html", "rb");
    if (!file)
    {
        printf("TROUDCUL\n");
        exit(EXIT_FAILURE);
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    
    // Allocate a buffer to hold the file contents
    char *file_data = new char[file_size];
    
    // Read the file contents into the buffer
    size_t bytes_read = fread(file_data, 1, file_size, file);
    if (static_cast<long>(bytes_read) != file_size) {
        // Handle file read error
    }
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    
    //THIS is print test for localhost:8080 in a web browser
    //const char *data = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n"; // displays image
    //const char *data = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";  // displays HTML file
    const char *data = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"; // displays plain text

    int pictureLen = strlen(data);
    int total_response_len = pictureLen + file_size;

    // Allocate a buffer to hold the entire HTTP response
    char *response_data = new char[total_response_len];
   
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cerr <<  "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    //THIS is for testing purposes, shuts down the socket immediately
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Failed to shut down the socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<int> hello;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char content_length_header[50];
        snprintf(content_length_header, sizeof(content_length_header), "Content-Length: %ld\r\n", file_size);

        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
         // Copy the headers to the response buffer
        memcpy(response_data, data, pictureLen);

        memcpy(response_data + pictureLen, content_length_header, strlen(content_length_header));

        // Copy the file data to the response buffer
        memcpy(response_data + pictureLen + strlen(content_length_header), file_data, file_size);


        // Copy the file data to the response buffer
        memcpy(response_data + pictureLen, file_data, file_size);
        // Send the entire HTTP response to the client
        send(new_socket, response_data, total_response_len, 0);
        //write(new_socket , picture , strlen(picture));
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);

         // Free the buffers
        //delete[] file_data;
        //delete[] response_data;
    }
    fclose(file);
    return 0;
}
//Server side C++ program to demonstrate Socket programming
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <unistd.h>
//#include <fcntl.h>
//
//#define PORT 8080
//
//int main(int argc, char const *argv[])
//{
//    (void) argc;
//    (void) argv;
//    
//    int server_fd, new_socket, valread;
//    struct sockaddr_in address;
//    int addrlen = sizeof(address);
//    
//    FILE *file = fopen("Cori.jpeg", "rb");
//    if (!file)
//    {
//        printf("TROUDCUL\n");
//        exit(EXIT_FAILURE);
//    }
//
//    // Determine the size of the file
//    fseek(file, 0, SEEK_END);
//    long file_size = ftell(file);
//    rewind(file);
//    
//    // Allocate a buffer to hold the file contents
//    char *file_data = new char[file_size];
//    
//    // Read the file contents into the buffer
//    size_t bytes_read = fread(file_data, 1, file_size, file);
//    if (static_cast<long>(bytes_read) != file_size) {
//        // Handle file read error
//    }
//    
//    const char *picture = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";
//    //const char *picture = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\nContent-length: 5\r\n\r\nsalut";
//    int pictureLen = strlen(picture);
//    int total_response_len = pictureLen + file_size;
//
//    // Allocate a buffer to hold the entire HTTP response
//    char *response_data = new char[total_response_len];
//    
//    // Creating socket file descriptor
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
//    {
//        perror("In socket");
//        exit(EXIT_FAILURE);
//    }
//
//    int opt = 1;
//    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//    
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons( PORT );
//    
//    memset(address.sin_zero, '\0', sizeof address.sin_zero);
//    
//    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
//    {
//        perror("In bind");
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd, 10) < 0)
//    {
//        perror("In listen");
//        exit(EXIT_FAILURE);
//    }
//    while(1)
//    {
//        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
//        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
//        {
//            perror("In accept");
//            exit(EXIT_FAILURE);
//        }
//
//        char content_length_header[50];
//        snprintf(content_length_header, sizeof(content_length_header), "Content-Length: %ld\r\n", file_size);
//
//        char buffer[30000] = {0};
//        valread = read( new_socket , buffer, 30000);
//        printf("%s\n",buffer );
//        
//        // Copy the headers to the response buffer
//        memcpy(response_data, picture, pictureLen);
//
//        memcpy(response_data + pictureLen, content_length_header, strlen(content_length_header));
//
//        // Copy the file data to the response buffer
//        memcpy(response_data + pictureLen + strlen(content_length_header), file_data, file_size);
//
//        // Send the entire HTTP response to the client
//        send(new_socket, response_data, total_response_len, 0);
//
//        printf("------------------Hello message sent-------------------\n");
//        close(new_socket);
//    }
//    //fclose(file);
//    return 0;
//}