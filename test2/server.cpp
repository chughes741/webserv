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
        std::cerr << "Error - File not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    
    // Allocate a buffer to hold the file contents
    std::string file_data(file_size, '\0');
    
    // Read the file contents into the buffer
    size_t bytes_read = fread(&file_data[0], 1, file_size, file);
    if (static_cast<long>(bytes_read) != file_size) {
        // Handle file read error
    }
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    
    //THIS is print test for localhost:8080 in a web browser
    //std::string data = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"; // displays plain text
    //std::string data = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";   // displays the picture
    std::string data = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";  // displays the HTML format

    int pictureLen = data.length();
    int total_response_len = pictureLen + file_size;

    // Allocate a buffer to hold the entire HTTP response
    std::string response_data(total_response_len, '\0');
   
    
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

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    std::fill(address.sin_zero, address.sin_zero + sizeof address.sin_zero, 0);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        std::cerr << "In bind" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "In listen" << std::endl;
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        std::cout << std::endl << "******* Waiting for new connection *******" << std::endl << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            std::cerr << "In accept" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string content_length_header(50, '\0');
        content_length_header = "Content-Length: " + std::to_string(file_size) + "\r\n";
        
        std::string buffer(30000, '\0');
        valread = read( new_socket , &buffer[0], 30000);
        std::cout << buffer << std::endl;
         // Copy the headers to the response buffer
        std::copy(data.begin(), data.begin() + pictureLen, response_data.begin());

        std::copy(file_data.begin(), file_data.begin() + file_size, response_data.begin() + pictureLen);

        // Copy the file data to the response buffer
        std::copy(file_data.begin(), file_data.begin() + file_size, response_data.begin() + pictureLen + content_length_header.length());


        // Copy the file data to the response buffer
        std::copy(file_data.begin(), file_data.begin() + file_size, response_data.begin() + pictureLen);
        // Send the entire HTTP response to the client
        send(new_socket, &response_data[0], total_response_len, 0);
        //write(new_socket , picture , strlen(picture));
        std::cout << "**********************Hello message sent**********************" << std::endl;
        close(new_socket);

         // Free the buffers
        //delete[] file_data;
        //delete[] response_data;
    }
    fclose(file);
    return 0;
}