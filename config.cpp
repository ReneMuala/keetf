/*  KEEFT - TCP/IP FILE TRANSFER TOOL
 *  © Landia (Rene Muala)
 *
 */

#include "config.hpp"
#include "net.hpp"

namespace keeft {

char 
    param_filename  [keeft_filename_len],
    param_IPv4      [INET_ADDRSTRLEN] ,
    param_IPv6      [INET6_ADDRSTRLEN], // currently unsused
    param_key       [keeft_key_len];
    
uint16_t
    param_port;

size_t 
    param_filesize,
    param_buffsize;
    
extern int 
    main_sock;
    
extern hostent * 
    temp_sock;

extern sockaddr_in 
    server_addr;
    
void set_default_key(){
    strcpy(param_key, "K00000");
}

void set_default_port(){
    param_port = htons(9000);
}

void set_default_address(){
    strcpy(param_IPv4, keeft::get_machine_IPv4_addrs()[0].data());
}

void set_default_buff_size (){
    param_buffsize = 10240;
}

void set_defaults(){
    set_default_key();
    set_default_port();
    set_default_address();
    set_default_buff_size();
}

void configure_sock_addr(sockaddr_in & addr , int port)
{
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
}


bool configure_server(int port)
{
     configure_sock_addr(server_addr, port);
     if(bind(main_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return false;
     return true;
}

bool configure_client(const char * address, int port)
{
     temp_sock = gethostbyname(address);
     configure_sock_addr(server_addr, port);
     memcpy((char*)&server_addr.sin_addr.s_addr, (char*)temp_sock->h_addr_list[0], temp_sock->h_length);
     return temp_sock;
}

}
