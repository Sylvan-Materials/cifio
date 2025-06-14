#include <unistd.h>
#include "reading/tcp/TCPReader.h"

namespace sylvanmats::reading{

    bool TCPReader::operator()(std::string& urlStr, std::function<void(std::istream& content)> apply){
    url::Url url(urlStr);
    struct addrinfo hints = {}, *infoptr;
    char port_str[16] = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(port_str, "%d", port);
    int result = getaddrinfo(url.host().c_str(), port_str, &hints, &infoptr);
    if (result) {
       std::cout<<"getaddrinfo: "<<gai_strerror(result)<<std::endl;
        return false;
    }
    struct addrinfo *ptr;
    struct sockaddr_in  *sockaddr_ipv4;
        struct sockaddr_in6 *sockaddr_ipv6;
    char host[256];
    int sd = -1;
    int infoCount=0;
    for (ptr = infoptr; ptr != NULL; ptr = ptr->ai_next) {
        infoCount++;
        //if(infoCount<=2)continue;
        getnameinfo(ptr->ai_addr, ptr->ai_addrlen, host, sizeof (host), NULL, 0, NI_NUMERICHOST);
        //puts(host);
    std::cout<<"host: "<<host<<" "<<(ptr->ai_family==AF_INET)<<std::endl;//" "<<ptr->ai_canonname<<std::endl;
        sd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sd < 0) {
            std::cout<<"Error creating socket."<<std::endl;
            break;
        }
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            std::cout<<"Unspecified"<<std::endl;
            break;
        case AF_INET:
            std::cout<<"AF_INET (IPv4)"<<std::endl;
            sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
            //printf("\tIPv4 address %s\n",
            //    inet_ntoa(sockaddr_ipv4->sin_addr) );
            break;
        case AF_INET6:
            std::cout<<"AF_INET6 (IPv6)"<<std::endl;
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n",
            //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ipv6 = (sockaddr_in6*) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            //ipbufferlength = 46;
            /*iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, 
                ipstringbuffer, &ipbufferlength );
            if (iRetval)
                printf("WSAAddressToString failed with %u\n", WSAGetLastError() );
            else    
                printf("\tIPv6 address %s\n", ipstringbuffer);*/
            break;
        /*case AF_NETBIOS:
            printf("AF_NETBIOS (NetBIOS)\n");
            break;*/
        default:
            std::cout<<"Other "<<ptr->ai_family<<std::endl;
            break;
    }
    std::cout<<"\tSocket type: "<<std::endl;
    switch (ptr->ai_socktype) {
        case 0:
            std::cout<<"Unspecified"<<std::endl;
            break;
        case SOCK_STREAM:
            std::cout<<"SOCK_STREAM (stream)"<<std::endl;
            break;
        case SOCK_DGRAM:
            std::cout<<"SOCK_DGRAM (datagram) "<<std::endl;
            break;
        case SOCK_RAW:
            std::cout<<"SOCK_RAW (raw) "<<std::endl;
            break;
        case SOCK_RDM:
            std::cout<<"SOCK_RDM (reliable message datagram)"<<std::endl;
            break;
        case SOCK_SEQPACKET:
            std::cout<<"SOCK_SEQPACKET (pseudo-stream packet)"<<std::endl;
            break;
        default:
            std::cout<<"Other "<<ptr->ai_socktype<<std::endl;
            break;
    }
    std::cout<<"\tProtocol: "<<std::endl;
    switch (ptr->ai_protocol) {
        case 0:
            std::cout<<"Unspecified"<<std::endl;
            break;
        case IPPROTO_TCP:
            std::cout<<"IPPROTO_TCP (TCP)"<<std::endl;
            break;
        case IPPROTO_UDP:
            std::cout<<"IPPROTO_UDP (UDP) "<<std::endl;
            break;
        default:
            std::cout<<"Other %ld\n"<<ptr->ai_protocol<<std::endl;
            break;
    }
        if (connect(sd, ptr->ai_addr, ptr->ai_addrlen) == 0) {
            std::cout<<"connected "<<std::endl;
            std::cout.flush();
            break;
        }
        close(sd);
        std::cout<<"Error connecting to server."<<std::endl;
        return false;
    }

    freeaddrinfo(infoptr);
    //int s = socket(AF_UNSPEC, SOCK_STREAM, 0);
    if (sd < 0) {
        std::cout<<"Error creating socket."<<std::endl;
        return false;
    }

    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *meth = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new (meth);
    // Set minimum TLS version to 1.3
    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
    SSL_CTX_set_cipher_list(ctx, "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");
    //SSL_CTX_set_cipher_list(ctx, "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
    SSL_CTX_load_verify_dir(ctx, "/etc/ssl/certs");
    //SSL_CTX_use_certificate_file(ctx, "/home/roger/sylvanmats/cifio/server_chain.pem", SSL_FILETYPE_PEM);
    //SSL_CTX_use_certificate_file(ctx, "/etc/ssl/certs/ca-certificates.crt", SSL_FILETYPE_PEM);
    //SSL_CTX_use_PrivateKey_file(ctx, "/etc/ssl/private/ssl-cert-snakeoil.key", SSL_FILETYPE_PEM);
    SSL_CTX_set_options(ctx, SSL_OP_NO_TICKET);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, [](int preverify_ok, X509_STORE_CTX *x509_ctx)->int{
        std::cout<<"preverify_ok "<<preverify_ok<<" "<<std::endl;
        return preverify_ok;  
    });

    ssl = SSL_new (ctx);
    if (!ssl) {
        std::cout<<"Error creating SSL."<<std::endl;
        log_ssl();
        return false;
    }
    //BIO *bio = BIO_new_socket(sd, BIO_NOCLOSE);
    //std::cout<<"bio "<<bio<<" "<<SSL_get_blocking_mode(ssl)<<std::endl;
    //SSL_set_bio(ssl, bio, bio);
    //std::cout<<"bio'd "<<bio<<" "<<SSL_get_blocking_mode(ssl)<<std::endl;
    SSL_CTX_set_info_callback(ctx, [](const SSL *s, int where, int ret)->void{
        const char *str;
        int w;

        w=where& ~SSL_ST_MASK;

        if (w & SSL_ST_CONNECT) str="SSL_connect";
        else if (w & SSL_ST_ACCEPT) str="SSL_accept";
        else str="undefined";

        if (where & SSL_CB_LOOP){
            printf("%s:%s\n",str,SSL_state_string_long(s));
        }
        else if (where & SSL_CB_ALERT){
        str=(where & SSL_CB_READ)?"read":"write";
        printf("SSL3 alert %s:%s:%s\n",
                str,
                SSL_alert_type_string_long(ret),
                SSL_alert_desc_string_long(ret));
        }
        else if (where & SSL_CB_EXIT){
            if (ret == 0)
                printf("%s:failed in %s\n",
                        str,SSL_state_string_long(s));
            else if (ret < 0){
                printf("%s:error in %s\n",
                        str,SSL_state_string_long(s));
            }
        }
    });
    //sock = SSL_get_fd(ssl);
    if (SSL_set_fd(ssl, sd)==0){
        std::cout<<"Failed to set fd. "<<std::endl;
    }
    SSL_set_tlsext_host_name(ssl, "files.rcsb.org");
    int err = SSL_connect(ssl);
    if (err <= 0) {
        std::cout<<"Error creating SSL connection.  err="<<err<<std::endl;
        log_ssl();
        fflush(stdout);
        return false;
    }
    std::cout<<"SSL connection using "<<SSL_get_cipher (ssl)<<std::endl;
    std::string escapedUrl=url::Url::escape_reserved_unsafe(urlStr);
//std::cout<<"escapedUrl "<<escapedUrl<<std::endl;
    std::string request = "GET "+url.path()+" HTTP/1.1\r\n";
    request+="Host: "+url.host()+":443\r\n";
    //request+="User-Agent: Sylvan-Materials/cifio\r\n";
    request+="Connection: keep-alive\r\n";
    request+="Accept: */*\r\n";
    request+="\r\n";
    //std::cout<<"request "<<request<<std::endl;
    if(SendPacket(request.c_str())<0){
        std::cout<<"SendPacket <0"<<std::endl;
    }
    std::stringstream ss;
    if(RecvPacket(ss)<0){
        std::cout<<"RecvPacket <0"<<std::endl;
    }
    std::string content=ss.str();
    std::string::size_type n=content.find("\r\n\r\n");
    if(n!=std::string::npos){
    n+=4;
    std::string::size_type breakIndex=content.find("\r\n", n);
//    std::cout<<"header |"<<content.substr(0, breakIndex+2)<<"|"<<std::endl;
    ss.str("");
    size_t chunkSize=0;
    do{
        chunkSize=0;
        size_t shifter=0;
        for(size_t index=breakIndex-1;index>=n;index--){
            size_t digit=(content.at(index)>='A') ? content.at(index)-'A'+10: content.at(index)-'0';
            if(content.at(index)>='a')digit=content.at(index)-'a'+10;
            chunkSize+=(digit<<shifter);
            shifter+=4;
        }
        n=(breakIndex+2+chunkSize+2);
        if(chunkSize>0 && n<content.size()){
            ss<<content.substr(breakIndex+2, chunkSize);
            breakIndex=content.find("\r\n", n);
        }
    }while(chunkSize>0 && n<content.size());
//    std::cout<<"content |"<<ss.str()<<"|"<<std::endl;
    }
    apply(ss);
    return true;
    };

}
