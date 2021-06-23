

#include "SSH_Helper.h"

#define STDOUT 1

void SSH_Helper::Read_Local_SSH_Keys(char* key_contents, char* local_filepath)
{
    char filepath[100];
    StringH::Erase_Num_Chars(filepath, 100);
    bool pub_key_file_exists = false;


    // if running on the linux mint laptop
    if(FileH::File_Exists("/home/mint/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/mint/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    // if running on Pi Cal Box
    if( FileH::File_Exists("/home/pi/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/pi/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    if(!pub_key_file_exists)
    {
        printf("could not find SSH public key file. ( ~/ssh/id_rsa.pub\n");
    }
    //printf("\npubkey filepath:\t%s\n", filepath);
    strcpy(local_filepath, filepath);

    FILE* pub_key_file = FileH::Open_Existing_File(filepath);
    char readline[650];

    while( (fgets(readline, 650, pub_key_file) != NULL) )
    {
        //printf("line read[%ld]: \n" COLOR_YELLOW "'%s'\n" COLOR_RESET, strlen(readline), readline);
        bool contains;
        char* first_token = StringH::Get_First_Token(readline);
        contains = StringH::String_Contains_Ignore_Case(first_token, "ssh-rsa");    // public key begins with ssh-rsa
        free(first_token);
        if(contains)
        {
            strncpy(key_contents, readline, 700);
            StringH::Trim_WhiteSpace(key_contents);
        }
    }
    FileH::Close_File(pub_key_file);
}

bool SSH_Helper::Have_Local_SSH_Keys_Generated()
{
    char filepath[100];
    StringH::Erase_Num_Chars(filepath, 100);
    bool pub_key_file_exists = false;

    // check that local computer has the ssh keys generated already
    // must detect what user currently is. If mint, check: /home/mint/ssh
    // if pi, check /home/pi/
    if(!FileH::Dir_Exists("/home/mint/.ssh") && !FileH::Dir_Exists("/home/pi/.ssh"))    // check local directory "~/.ssh/" exists
    {
        printf(".ssh dir does not exist on local computer. Cannot send keys\n");
        return false;
    }

    // if running on the linux mint laptop
    if(FileH::File_Exists("/home/mint/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/mint/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    // if running on Pi Cal Box
    if( FileH::File_Exists("/home/pi/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/pi/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    if(!pub_key_file_exists)
    {
        printf("local public key file does not exist, not generated yet or wrong location.\n");
        printf("expecting to find key file in [user home dir]/.ssh \t\tCannot send keys\n");
        return false;
    }

    FILE* pub_key_file = FileH::Open_Existing_File(filepath);
    char readline[256];

    while( (fgets(readline, 256, pub_key_file) != NULL) )
    {
        bool contains;
        printf("line read from key file: %s\n", readline);
        char* first_token = StringH::Get_First_Token(readline);
        printf("first token:  %s\n", first_token);
        contains = StringH::String_Contains_Ignore_Case(first_token, "ssh-rsa");    // public key begins with ssh-rsa
        printf("contains result: %d\n", contains);
        if(contains)
            return true;
    }
    return false;
}





void SSH_Helper::Create_Remote_SSH_DIR()
{ 
    
    const char* SAVE_DIR = "~/.ssh";

    if( !FileH::Dir_Exists( SAVE_DIR) )
    {
        mkdir( SAVE_DIR, 0700);
        printf("\tSSH DIR:  " COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, SAVE_DIR);
    }
    else
    {
        printf("dir already exists\n");
    }

    return;
}



/*
|--------------------------------------------------------------------------
| Exec_Shell_Command
|--------------------------------------------------------------------------
|
| Execute a command at the terminal prompt. The command is passed as argument.
| A string of the shell output response is returned. The string captures
| stdout and stderr.
|
| @param:  command    the shell command to execute
| @return:            a string of the shell response is returned
*/
char** SSH_Helper::Exec_Shell_Command(char* command)
{
    int MAX_LINES = 100;
    int MAX_LINE_LENGTH = 150;
    char read_line[250];

    FILE* command_shell = popen(command, POPEN_READ);                 //  The popen() function opens a process by creating a pipe, forking and invoking the shell.
    if (command_shell == NULL)
    {
        printf("Error opening pipe for shell command!!\n" );
        exit(1);
    }

    int line_count = 0;
    char**  string_array = (char**)malloc(sizeof(char*) * MAX_LINES);        // allocate memory for pointer to strings
    while (fgets(read_line, sizeof(read_line), command_shell) != NULL)     /* Read the output a line at a time - output it. */
    {
        StringH::Trim_WhiteSpace(read_line);
        printf("RESPONSE:\t%s\n", read_line);
        //string_array[line_count] = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH); // allocate memory for the
		string_array[line_count] = (char*)malloc(sizeof(char) * ( strlen(read_line) +10 ) ); // allocate memory for the return string
        strcpy(string_array[line_count], read_line);
        ++line_count;
    }
    string_array[line_count] = NULL;   // Add a marker for the last string in the array
    pclose(command_shell);     // close file pointer to STDOUT
    return string_array;
}

void SSH_Helper::Free_String_Array_Memory(char** string_array)
{
    int i = 0;
    while(string_array[i] != NULL)
    {
        free(string_array[i]);
        //printf("free string\n");
        ++i;
    }
    free(string_array);
    //printf("free array\n");

    return;
}


int SSH_Helper::show_remote_processes(ssh_session session)
{
  printf("\n\n");
  ssh_channel channel;
  int rc;
  char buffer[256];
  int nbytes;
  channel = ssh_channel_new(session);
  if (channel == NULL)
    return SSH_ERROR;
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK)
  {
    ssh_channel_free(channel);
    return rc;
  }
  rc = ssh_channel_request_exec(channel, "ps aux");
  if (rc != SSH_OK)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  printf(COLOR_BOLD_GREEN "\n\nBUFFER [%d]:\n %s\n" COLOR_RESET, nbytes, buffer);
  printf(COLOR_RESET);
  printf("--BUFFER DONE--\n");
  while (nbytes > 0)
  {
    if (write(STDOUT, buffer, nbytes) != (unsigned int) nbytes) // printing to terminal, writes contents of the buffer to standard out
    {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }
  if (nbytes < 0)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_ERROR;
  }

  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  return SSH_OK;
}



bool SSH_Helper::Pi_Has_SSH_DIR(ssh_session session)
{
    printf("\n\n");
    ssh_channel channel;
    int rc;
    char buffer[600];
    char temp[256];
    int nbytes;
    channel = ssh_channel_new(session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, "ls -al | grep .ssh");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(buffer, 600);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(buffer, temp, nbytes);
    }

    printf(COLOR_RESET);
    printf("\n");
    while (nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    printf("\nSearching remote pi for .ssh dir:\tls -al | grep .ssh \n");
    printf(COLOR_GREEN "%s" COLOR_RESET, buffer);
    printf(COLOR_RESET);

    bool has_ssh_dir = StringH::String_Contains(buffer, ".ssh");
    if( has_ssh_dir)
    {
        printf("\t[remote Pi has .ssh dir]\n");
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return has_ssh_dir;
}

// pre condition is that the folder .ssh exists on the Pi in user directory
bool SSH_Helper::Pi_Has_Authorized_Keys_File(ssh_session session)
{
    printf("\n\n");
    ssh_channel channel;
    int rc;
    char buffer[600];
    char temp[256];
    int nbytes;
    channel = ssh_channel_new(session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, "ls .ssh/");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(buffer, 600);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(buffer, temp, nbytes);
    }

    printf(COLOR_RESET);
    printf("\n");
    while (nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            //printf("temp: %s\n", temp);
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    printf("searching remote pi for authorized keys file:\tls .ssh/ \n");
    printf(COLOR_GREEN "%s" COLOR_RESET, buffer);
    printf(COLOR_RESET);

    bool has_keys_file = StringH::String_Contains(buffer, "authorized_keys");
    if( has_keys_file)
    {
        printf("\t[remote Pi has authorized_keys file]\n\n");
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return has_keys_file;
}


int SSH_Helper::Append_SSH_Key_To_Remote_Host(ssh_session session, char* key)
{
    printf("\n\n");
    ssh_channel channel;
    int rc;
    char buffer[1200];
    char temp[256];
    int nbytes;
    channel = ssh_channel_new(session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }

    int keylen = strlen(key);
    int cmd_len = keylen + strlen("echo \"\" >> /home/pi/.ssh/authorized_keys")+1;
    char append_cmd[cmd_len];
    sprintf(append_cmd, "echo \"%s\" >> /home/pi/.ssh/authorized_keys", key);

    rc = ssh_channel_request_exec(channel, append_cmd); // execute the command: echo "[ssh key]" >> /home/pi/.ssh/authorized_keys
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(buffer, 1200);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(buffer, temp, nbytes);
    }

    printf("\n");
    while (nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            printf("temp: %s\n", temp);
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    printf(COLOR_BOLD_GREEN "%s\n" COLOR_RESET, buffer);
    printf(COLOR_RESET);

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

int SSH_Helper::SCP_Read_Remote_File(ssh_session session, const char* remote_filepath, char* file_contents)
{
  ssh_scp scp;
  int rc;
 
  printf(COLOR_BOLD_YELLOW "\n\t---READING PI REMOTE KEYFILE--\n" COLOR_RESET);
  printf(COLOR_RESET);
  scp = ssh_scp_new(session, SSH_SCP_READ | SSH_SCP_RECURSIVE, remote_filepath);
  if (scp == NULL)
  {
    fprintf(stderr, "Error allocating scp session: %s\n", ssh_get_error(session));
    return SSH_ERROR;
  }
 
  rc = ssh_scp_init(scp);
  if (rc != SSH_OK)
  {
    fprintf(stderr, "Error initializing scp session: %s\n",
    ssh_get_error(session));
    ssh_scp_free(scp);
    return rc;
  }
 
  int size, mode;
  char *filename, *buffer;
 
  rc = ssh_scp_pull_request(scp);
  if (rc != SSH_SCP_REQUEST_NEWFILE)
  {
    fprintf(stderr, "Error receiving information about file: %s\n", ssh_get_error(session));
    return SSH_ERROR;
  }
 
  size = ssh_scp_request_get_size(scp);
  filename = strdup(ssh_scp_request_get_filename(scp));
  mode = ssh_scp_request_get_permissions(scp);
  printf("\nReceiving file " COLOR_MAGENTA "%s" COLOR_RESET ", size %d bytes, permissions 0%o\n", filename, size, mode);
  free(filename);
 
  buffer = (char*)malloc(size);
  if (buffer == NULL)
  {
    fprintf(stderr, "Memory allocation error\n");
    return SSH_ERROR;
  }
 
  ssh_scp_accept_request(scp);
  rc = ssh_scp_read(scp, buffer, size);
  if (rc == SSH_ERROR)
  {
    fprintf(stderr, "Error receiving file data: %s\n",
    ssh_get_error(session));
    free(buffer);
    return rc;
  }
 
  StringH::Erase_Num_Chars(file_contents, 650);
  strncpy(file_contents, buffer, size);
  free(buffer);
 
  rc = ssh_scp_pull_request(scp);
  if (rc != SSH_SCP_REQUEST_EOF)
  {
    fprintf(stderr, "Unexpected request: %s\n", ssh_get_error(session));
    return SSH_ERROR;
  }
 
  ssh_scp_close(scp);
  ssh_scp_free(scp);
  return SSH_OK;
}

int SSH_Helper::verify_knownhost(ssh_session session)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    ssh_key srv_pubkey = NULL;
    size_t hlen;
    char buf[10];
    char *hexa;
    char *p;
    int cmp;
    int rc;
    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA1,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    state = ssh_session_is_known_server(session);
    switch (state) {
        case SSH_KNOWN_HOSTS_OK:
            /* OK */
            //printf(COLOR_GREEN "\nSSH known hosts OK\n" COLOR_RESET);
            printf(COLOR_RESET);
            break;
        case SSH_KNOWN_HOSTS_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            //ssh_print_hexa("Public key hash", hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);
            printf("\nSSH known hosts changed\n");
            return -1;
        case SSH_KNOWN_HOSTS_OTHER:
            fprintf(stderr, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);
            printf("\nSSH known hosts other\n");
            return -1;
        case SSH_KNOWN_HOSTS_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_KNOWN_HOSTS_UNKNOWN:
            printf("\nSSH hosts unknown\n");
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
            fprintf(stderr, "Public key hash: %s\n", hexa);
            fprintf(stderr, "To accept you must type in 'yes' and hit enter\n");
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            p = fgets(buf, sizeof(buf), stdin);
            if (p == NULL) {
                return -1;
            }
            cmp = strncasecmp(buf, "yes", 3);
            if (cmp != 0) {
                return -1;
            }
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0) {
                fprintf(stderr, "Error %s\n", strerror(errno));
                return -1;
            }
            break;
        case SSH_KNOWN_HOSTS_ERROR:
            fprintf(stderr, "Error %s", ssh_get_error(session));
            ssh_clean_pubkey_hash(&hash);
            return -1;
    }
    ssh_clean_pubkey_hash(&hash);
    return 0;
}


int SSH_Helper::SCP_Write_Remote_File(ssh_session session, const char* local_filepath, const char* remote_filepath, int mode)
{
    char* remote_filename = FileH::Extract_Filename(remote_filepath);   // extracts "doc1.txt" from "/home/pi/test/new_dir/doc1.txt"
    char* remote_dir = FileH::Extract_Parent_Dir(remote_filepath);      // extracts "/home/pi/test/" from "/home/pi/test/new_dir"
    
    SSH_Helper::SCP_Create_Remote_Dir(session, remote_dir); // creates remote directory if it does not exist. If it does exist, no change

    ssh_scp scp;
    int rc;
    scp = ssh_scp_new(session, SSH_SCP_WRITE, remote_dir);   // initialize scp module in the directory where we want to write the file

    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n", ssh_get_error(session));
        return SSH_ERROR;
    }
    
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
  {
    fprintf(stderr, "Error initializing scp session: %s\n", ssh_get_error(session));
    ssh_scp_free(scp);
    return rc;
  }
 
    // verify source file actually exists
    if( !FileH::File_Exists(local_filepath) )
    {
        printf("Error: file '%s' does not exist\n", local_filepath);
        ssh_scp_close(scp);
        ssh_scp_free(scp);
        return SSH_ERROR;
    }

    printf("reading local file: '%s' and writing to remote filepath: " COLOR_MAGENTA "%s\n\n" COLOR_RESET, local_filepath, remote_filepath);

    //long size = FileH::Get_File_Size(local_filepath);   // get file size, to know how large of a buffer to create
    std::ifstream input_file_stream(local_filepath);    // create input stream of the file
    std::string file_str(std::istreambuf_iterator<char>{input_file_stream}, {});    // read entire file into C++ string

    // Convert the C++ string to C string
    char* file_contents = new char [file_str.length()+1];  // allocate memory to store contents of file read as a string
    strcpy (file_contents, file_str.c_str());                 // copy the contents into the allocated memory
    int file_length = strlen(file_contents);

    printf("%s [%d chars]:\n", remote_filepath, file_length);
    printf("------------------------------------\n");
    printf( COLOR_YELLOW "%s\n" COLOR_RESET, file_contents);
    printf("------------------------------------\n\n");

    rc = ssh_scp_push_file(scp, remote_filename, file_length, mode);    // prepare for file transfer

    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't open remote file: %s\n", ssh_get_error(session));
        return rc;
    }
    
    rc = ssh_scp_write(scp, file_contents, file_length);                // actually write the file
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't write to remote file: %s\n", ssh_get_error(session));
        return rc;
    }

    delete[] file_contents;
    free(remote_filename);
    free(remote_dir);
    ssh_scp_close(scp);
    ssh_scp_free(scp);
    return SSH_OK;
}

// remote dir argument should be fully qualified path
// for example: /home/pi/test/new_dir
//      -- creates the directory "new_dir" within already existing directory "test"
int SSH_Helper::SCP_Create_Remote_Dir(ssh_session session, const char* remote_dir)
{
    char* new_dir_location = FileH::Extract_Parent_Dir(remote_dir); // extracts "/home/pi/test/" from "/home/pi/test/new_dir"
    char* new_dir_name = FileH::Extract_Filename(remote_dir);
    
    ssh_scp scp;
    int rc;
    scp = ssh_scp_new(session, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, new_dir_location);   // initialize

    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n", ssh_get_error(session));
        return SSH_ERROR;
    }
    
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n", ssh_get_error(session));
        ssh_scp_free(scp);
        return rc;
    }
    
    rc = ssh_scp_push_directory(scp, new_dir_name, S_IRWXU);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't create remote directory: %s\n", ssh_get_error(session));
        return rc;
    }

    printf("created remote dir: " COLOR_MAGENTA "/%s" COLOR_RESET " in %s\n", new_dir_name, new_dir_location);
    free(new_dir_location);
    free(new_dir_name); 
    printf("\n");
    ssh_scp_close(scp);
    ssh_scp_free(scp);
    return SSH_OK;
}