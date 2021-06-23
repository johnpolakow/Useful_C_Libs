
#define LIBSSH_STATIC 1

#include <errno.h>
#include <string.h>
#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h> 
#include <stdbool.h>
#include <iterator>
#include <iostream>
#include <fstream>

#include "SSH_Helper.h"
#include "StringH.h"
#include "network.h"
#include "Parameters.h"
#include "colors.h"

// compile with:
//  g++ -o ssh_con lssh_con.c SSH_Helper.cpp StringH.cpp FileH.cpp -l ssh

int main()
{
    // handle case where ssh folders do not yet exist on the PI
    // test if raspberry password doesnt work- use blackberry

    printf(COLOR_BOLD_YELLOW "\n\t--DISCOVERING LOCAL NETWORK INTERFACES ON THIS CLIENT--\n" COLOR_RESET);
    char REMOTE_HOST_IP[20];
    Net_Interface* local_interfaces = new Net_Interface();

    printf(COLOR_BOLD_YELLOW "\n\t--SEARCHING LOCAL ETH NETWORKS FOR RASPBERRY PIS--\n" COLOR_RESET);
    remote_pi** remote_pis = local_interfaces->Find_Connected_PIs();
    printf("  Raspberry PIs Found:\n");
    Net_Interface::Print_Connected_Pi_List(remote_pis);

    if(remote_pis != NULL)
    {
        remote_pi* connect_pi = remote_pis[0];
        strncpy(REMOTE_HOST_IP, connect_pi->pi_ip_addr, 20);
            usleep(100);
        printf("------------------------------\n");
        printf(COLOR_MAGENTA "Connecting to  Pi @ IP Addr: %s\n" COLOR_RESET, REMOTE_HOST_IP);
        printf(COLOR_RESET);
        printf("------------------------------\n\n");
    }
    Net_Interface::Free_Mem_Pi_List(remote_pis);
    delete local_interfaces;


    ssh_session this_session;
    int rc;
    int port = SSH_PORT;
    int verbosity = SSH_LOG_WARNING; 
    // other logging options:
        // SSH_LOG_NOLOG
        // SSH_LOG_WARNING
        // SSH_LOG_PROTOCOL
        // SSH_LOG_PACKET
        // SSH_LOG_FUNCTION

    const char *password = REMOTE_PASSWORD;

    // Open session and set options
    this_session = ssh_new();
    if (this_session == NULL)
        exit(-1);
    ssh_options_set(this_session, SSH_OPTIONS_HOST, REMOTE_HOST_IP);
    ssh_options_set(this_session, SSH_OPTIONS_USER, REMOTE_SSH_USER);
    ssh_options_set(this_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(this_session, SSH_OPTIONS_CIPHERS_C_S,"aes128-ctr");
    //ssh_options_set(this_session, SSH_OPTIONS_PORT, &port);

    // Connect to server
    rc = ssh_connect(this_session);
    if (rc != SSH_OK)  
    {
        fprintf(stderr, "Error connecting: %s\n", ssh_get_error(this_session)); 
        ssh_free(this_session);
        exit(-1);
    }

    printf("\nlogging into host " COLOR_GREEN "%s" COLOR_RESET " as user " COLOR_MAGENTA "%s" COLOR_RESET " with passwd " COLOR_MAGENTA "%s\n" COLOR_RESET, 
            REMOTE_HOST_IP, REMOTE_SSH_USER, REMOTE_PASSWORD);
    printf(COLOR_RESET);
    
    
    if (SSH_Helper::verify_knownhost(this_session) < 0)     // Verify the server's identity
    {
        printf("error, could not get authentication\n");
        printf("you may need to regenerate SSH keys, or reconfigure PI SSH params.\n");
        ssh_disconnect(this_session);
        ssh_free(this_session);
        exit(-1);
    }
  
    // Authenticate ourselves, using password defined in "Parameters.h"
    rc = ssh_userauth_password(this_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
        fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(this_session));
        ssh_disconnect(this_session);
        ssh_free(this_session);
        exit(-1);
    }
    printf(" ... Success\n");


    // Handle SSH Keys
    // SSH Keys work as a password, to avoid having to retype a password for every SSH login
    // It functions as follows:
    // you (the user) generate SSH keys on your PC. On linux the lublic keys will be stored in ~/.ssh/id_rsa.pub
    // you take these keys and upload them to a PC you want to login to without a password
    // the upload location is: '~/.ssh/authorized_keys'  you append your keys to this file
    // whenever your computer attempts to login to server computer, password is no longer required



    char remote_keyfile_contents[5000];
    char local_key[700];
    char local_filepath[100];
    StringH::Erase_Num_Chars(remote_keyfile_contents, 5000);
    StringH::Erase_Num_Chars(local_key, 700);
    StringH::Erase_Num_Chars(local_filepath, 100);

    SSH_Helper::Pi_Has_SSH_DIR(this_session);
    SSH_Helper::Pi_Has_Authorized_Keys_File(this_session);          // check to see if Pi has SSH Keys
    SSH_Helper::Read_Local_SSH_Keys(local_key, local_filepath);     // read our local SSH keys
    printf("LOCAL KEY[%ld]:\t" COLOR_MAGENTA "%s\n" COLOR_RESET COLOR_YELLOW "%s\n" COLOR_RESET, strlen(local_key), local_filepath, local_key);

    const char* remote_key_filepath = REMOTE_KEYS_FILE;                 // defined in "Parameters.h", represents the location of the pi ssh key file
    SSH_Helper::SCP_Read_Remote_File(this_session, remote_key_filepath, remote_keyfile_contents);   // read PI SSH keys. 
    printf("\nREMOTE KEYFILE[%ld]:\t" COLOR_MAGENTA "%s\n" COLOR_RESET COLOR_YELLOW "%s\n" COLOR_RESET, strlen(remote_keyfile_contents), remote_key_filepath, remote_keyfile_contents);

    bool remote_host_has_our_key = StringH::String_Contains(remote_keyfile_contents, local_key);
    if(!remote_host_has_our_key)    // if our ssh_key is not installed on remote pi, add it
    {
        printf(COLOR_MAGENTA "\n--ADDING SSH KEY TO REMOTE HOST--\n" COLOR_RESET);
        SSH_Helper::Append_SSH_Key_To_Remote_Host(this_session, local_key);
    }
    else
    {
        printf(COLOR_BOLD_YELLOW "\n\t--REMOTE HOST ALREADY HAS OUR KEYS--\t" COLOR_RESET);
        printf("... continuing\n\n");
    }



    printf(COLOR_BOLD_YELLOW "\n\t--CREATING REMOTE FILES AND DIRECTORIES--\n\n" COLOR_RESET);
    rc = SSH_Helper::SCP_Create_Remote_Dir(this_session, PI_FIRMWARE_SRC_DIR);
    rc = SSH_Helper::SCP_Create_Remote_Dir(this_session, PI_CAL_FILES_DIR);





    rc = SSH_Helper::SCP_Write_Remote_File(this_session, "./CAL_DATA/COOLER_mA_LUT.h", "/home/pi/test/COOLER_mA_LUT.h");
    rc = SSH_Helper::SCP_Write_Remote_File(this_session, "./CAL_DATA/DAC_COUNT_LUT.h", "/home/pi/test/DAC_COUNT_LUT.h");
    rc = SSH_Helper::SCP_Write_Remote_File(this_session, "./CAL_DATA/DIODE_V_LUT.h", "/home/pi/test/DIODE_V_LUT.h");
    rc = SSH_Helper::SCP_Write_Remote_File(this_session, "./CAL_DATA/LOAD_mA_LUT.h", "/home/pi/test/LOAD_mA_LUT.h");
    rc = SSH_Helper::SCP_Write_Remote_File(this_session, "./CAL_DATA/LOAD_V_LUT.h", "/home/pi/test/LOAD_V_LUT.h");

    //printf("size: %ld \n", size);

    //std::ifstream ifs("./Parameters.h");
    //std::string str(std::istreambuf_iterator<char>{ifs}, {});

    //int length = str.length();
    //printf("file length in chars: %d\n", length);




    //SSH_Helper::show_remote_processes(this_session);
    ssh_disconnect(this_session);
    ssh_free(this_session);
}