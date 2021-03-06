/*  KEEFT - TCP/IP FILE TRANSFER TOOL
 *  © Landia (Rene Muala)
 *
 */


#include "net.hpp"
#include "io.hpp"

namespace keeft
{
    
int 
    main_sock,
    file_sock;
    
hostent * 
    temp_sock;

sockaddr_in 
    server_addr;
    
socklen_t 
    server_addr_len;
    
extern size_t 
    param_buffsize;

int create_stream_tcpip_socket()
{
    return socket(AF_INET, SOCK_STREAM, 0); 
}

void perror(const char * message)
{
    fprintf(stderr, "Error: %s", message);
    exit(1);
}

bool init()
{
    if(!(main_sock = create_stream_tcpip_socket()))
        perror("unanble to create main_sock at init_ser_keeft");
    return true;
}

bool wait_for_client()
{
    if(listen(main_sock, 5) < 0)
        perror("Unable to listen");
    server_addr_len = sizeof(server_addr);
    return (file_sock = accept(main_sock, (sockaddr*)&server_addr, &server_addr_len)) and (file_sock >=0 );
}

bool verify_password(const char* key)
{
    char received_key [keeft_key_len];
    recv(file_sock, received_key,keeft_key_len,0);
    if(!strlen(key) or strcmp(key, received_key) == 0){
        ::send(file_sock, "1", 1, 0);
        return true;
    } else {
        ::send(file_sock, "0", 1, 0);
        return false;
    }
}

bool get_file_specs(char* filename, size_t& filesize)
{
    return 
    recv(file_sock, filename, keeft_filename_len, 0) and
    recv(file_sock, &filesize, sizeof(filesize), 0);
}

bool receive_file(FILE*  file, size_t size, size_t buf_size)
{
    ::fseek(file, 0, SEEK_SET);
    int buffer[buf_size];
    size_t
        piece_size = 0,
        received_size = 0;
    if(!size) return true;
    printf("-----------received");
    while(received_size < size){
        piece_size = recv(file_sock, &buffer, buf_size,0);
        received_size += piece_size;
        if(!piece_size)            return false;
        print_progress(received_size, size);
        fwrite(&buffer, 1, piece_size, file);
        bzero(buffer, buf_size);
    } return true;
}

void ignore_path_in_filename(char*filename)
{
    std::string filename_str = filename;
    size_t init_pos = filename_str.find_last_of('/')+1;
    init_pos = (init_pos < filename_str.length()) ? init_pos : 0;
    bzero(filename, keeft_filename_len);
    strcpy(filename, filename_str.substr(init_pos, filename_str.length() - init_pos).data());
}

size_t get_file_size(FILE* file, size_t buff_size)
{
    printf("-analyzing-file...");
        setbuf(stdout, NULL);
    ::fseek(file, 0, SEEK_SET);
    size_t size = 0;
    char buffer[buff_size];
    while (!feof(file)){
        size += fread(buffer, 1, buff_size, file);
        bzero(buffer, buff_size);
    } printf("\r                   \r");
    return size;
}

bool connect_to_server()
{
    return (connect(main_sock, (sockaddr*)&server_addr, sizeof(server_addr)) >= 0);
}

bool send_password(const char* password)
{
    char feedback[2];
    ::send(main_sock, password, keeft_key_len, 0);
    recv(main_sock, feedback, 2, 0);
    return feedback[0] == '1';
}

bool send_file_specs(const char* filename, size_t filesize)
{
    return 
    ::send(main_sock, filename, keeft_filename_len, 0) and 
    ::send(main_sock, &filesize, sizeof(filesize), 0);
}

bool send_file(FILE*file , size_t size , size_t buff_size)
{
    ::fseek(file, 0, SEEK_SET);
    char buffer[buff_size];
    size_t piece_size = 0, sent_size = 0;
    printf("-----------sent");
    while(!feof(file) and sent_size < size){
        piece_size = fread(&buffer, 1, buff_size, file);
        sent_size += piece_size;
        print_progress(sent_size, size); 
        ::send(main_sock, &buffer, piece_size, 0);
        if(!piece_size) return false;
        bzero(buffer, buff_size);
    } return true;
}

bool end()
{
    close(main_sock);
    close(file_sock);
    return !main_sock and !file_sock;
}


std::string get_current_name(size_t length)
{
    std::string name;
    char buffer [length];
    gethostname(buffer, length);
    name = buffer; 
    return name;
}

std::vector<std::string> get_machine_IPv4_addrs()
{
    std::vector<std::string> IPv4_addrs;
    char IPv4[INET_ADDRSTRLEN];
    in_addr * temp_addr;
    ifaddrs * all, * ifa;
    getifaddrs(&all);
    for(ifa = all ; ifa ; ifa = ifa->ifa_next){
        if(!ifa->ifa_addr) continue;
        if(ifa->ifa_addr->sa_family == AF_INET){
            temp_addr = &((sockaddr_in*)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, temp_addr, IPv4, INET_ADDRSTRLEN);
            IPv4_addrs.resize(IPv4_addrs.size()+1);
            IPv4_addrs.at(IPv4_addrs.size()-1) = IPv4;
        }
    } freeifaddrs(all);
    return IPv4_addrs;
}

std::vector<std::string> get_machine_IPv6_addrs()
{
    std::vector<std::string> IPv6_addrs;
    char IPv6[INET6_ADDRSTRLEN];
    in6_addr * temp_addr;
    ifaddrs * all, * ifa;
    getifaddrs(&all);
    for(ifa = all ; ifa ; ifa = ifa->ifa_next){
        if(!ifa->ifa_addr) continue;
        if(ifa->ifa_addr->sa_family == AF_INET6){
            temp_addr = &((sockaddr_in6*)ifa->ifa_addr)->sin6_addr;
            inet_ntop(AF_INET6, temp_addr, IPv6, INET6_ADDRSTRLEN);
            IPv6_addrs.resize(IPv6_addrs.size()+1);
            IPv6_addrs.at(IPv6_addrs.size()-1) = IPv6;
        }
    } freeifaddrs(all);
    return IPv6_addrs;
}
}



