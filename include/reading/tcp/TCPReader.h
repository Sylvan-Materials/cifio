#pragma once

#include <vector>
#include <coroutine>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

#include "Url.h"

#include <coroutine>
#include <stdexcept>
#include <thread>

#ifdef __WIN32__
# include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL *ssl;
int sock;


namespace sylvanmats::reading{

	template<typename T>
	class generator;

	namespace detail
	{
		template<typename T>
		class generator_promise
		{
		public:

			using value_type = std::remove_reference_t<T>;
			using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
			using pointer_type = value_type*;

			generator_promise() = default;

			generator<T> get_return_object() noexcept;

			constexpr std::suspend_always initial_suspend() const { return {}; }
			constexpr std::suspend_always final_suspend() const { return {}; }

			template<
				typename U = T,
				std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
			std::suspend_always yield_value(std::remove_reference_t<T>& value) noexcept
			{
				m_value = std::addressof(value);
				return {};
			}

			std::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept
			{
				m_value = std::addressof(value);
				return {};
			}

			void unhandled_exception()
			{
				m_exception = std::current_exception();
			}

			void return_void()
			{
			}

			reference_type value() const noexcept
			{
				return static_cast<reference_type>(*m_value);
			}

			// Don't allow any use of 'co_await' inside the generator coroutine.
			template<typename U>
			std::suspend_never await_transform(U&& value) = delete;

			void rethrow_if_exception()
			{
				if (m_exception)
				{
					std::rethrow_exception(m_exception);
				}
			}

		private:

			pointer_type m_value;
			std::exception_ptr m_exception;

		};

        struct generator_sentinel {};

		template<typename T>
		class generator_iterator
		{
			using coroutine_handle = std::coroutine_handle<generator_promise<T>>;

		public:

			using iterator_category = std::input_iterator_tag;
			// What type should we use for counting elements of a potentially infinite sequence?
			using difference_type = std::ptrdiff_t;
			using value_type = typename generator_promise<T>::value_type;
			using reference = typename generator_promise<T>::reference_type;
			using pointer = typename generator_promise<T>::pointer_type;

			// Iterator needs to be default-constructible to satisfy the Range concept.
			generator_iterator() noexcept
				: m_coroutine(nullptr)
			{}
			
			explicit generator_iterator(coroutine_handle coroutine) noexcept
				: m_coroutine(coroutine)
			{}

			friend bool operator==(const generator_iterator& it, generator_sentinel) noexcept
			{
				return !it.m_coroutine || it.m_coroutine.done();
			}

			friend bool operator!=(const generator_iterator& it, generator_sentinel s) noexcept
			{
				return !(it == s);
			}

			friend bool operator==(generator_sentinel s, const generator_iterator& it) noexcept
			{
				return (it == s);
			}

			friend bool operator!=(generator_sentinel s, const generator_iterator& it) noexcept
			{
				return it != s;
			}

			generator_iterator& operator++()
			{
				m_coroutine.resume();
				if (m_coroutine.done())
				{
					m_coroutine.promise().rethrow_if_exception();
				}

				return *this;
			}

			// Need to provide post-increment operator to implement the 'Range' concept.
			void operator++(int)
			{
				(void)operator++();
			}

			reference operator*() const noexcept
			{
				return m_coroutine.promise().value();
			}

			pointer operator->() const noexcept
			{
				return std::addressof(operator*());
			}

		private:

			coroutine_handle m_coroutine;
		};
	}

	template<typename T>
	class [[nodiscard]] generator
	{
	public:

		using promise_type = detail::generator_promise<T>;
		using iterator = detail::generator_iterator<T>;

		generator() noexcept
			: m_coroutine(nullptr)
		{}

		generator(generator&& other) noexcept
			: m_coroutine(other.m_coroutine)
		{
			other.m_coroutine = nullptr;
		}

		generator(const generator& other) = delete;

		~generator()
		{
			if (m_coroutine)
			{
				m_coroutine.destroy();
			}
		}

		generator& operator=(generator other) noexcept
		{
			swap(other);
			return *this;
		}

		iterator begin()
		{
			if (m_coroutine)
			{
				m_coroutine.resume();
				if (m_coroutine.done())
				{
					m_coroutine.promise().rethrow_if_exception();
				}
			}

			return iterator{ m_coroutine };
		}

		detail::generator_sentinel end() noexcept
		{
			return detail::generator_sentinel{};
		}

		void swap(generator& other) noexcept
		{
			std::swap(m_coroutine, other.m_coroutine);
		}

	private:

		friend class detail::generator_promise<T>;

		explicit generator(std::coroutine_handle<promise_type> coroutine) noexcept
			: m_coroutine(coroutine)
		{}

		std::coroutine_handle<promise_type> m_coroutine;

	};

	template<typename T>
	void swap(generator<T>& a, generator<T>& b)
	{
		a.swap(b);
	}

	namespace detail
	{
		template<typename T>
		generator<T> generator_promise<T>::get_return_object() noexcept
		{
			using coroutine_handle = std::coroutine_handle<generator_promise<T>>;
			return generator<T>{ coroutine_handle::from_promise(*this) };
		}
	}

	template<typename FUNC, typename T>
	generator<std::invoke_result_t<FUNC&, typename generator<T>::iterator::reference>> fmap(FUNC func, generator<T> source)
	{
		for (auto&& value : source)
		{
			co_yield std::invoke(func, static_cast<decltype(value)>(value));
		}
	}

    class TCPReader{
    protected:
            char buf[1'000'000];
            int port = 443;
    public:
        TCPReader() = default;
        TCPReader(const TCPReader& orig) = delete;
        virtual ~TCPReader() = default;

        bool operator()(std::string& urlStr, std::function<void(std::istream& content)> apply){
        url::Url url(urlStr);
        struct addrinfo hints = {}, *infoptr;
        char port_str[16] = {};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        sprintf(port_str, "%d", port);
        int result = getaddrinfo(url.host().c_str(), port_str, &hints, &infoptr);
        if (result) {
            printf("getaddrinfo: %s\n", gai_strerror(result));
            return false;
        }
        struct addrinfo *ptr;
        struct sockaddr_in  *sockaddr_ipv4;
            struct sockaddr_in6 *sockaddr_ipv6;
        char host[256];
        int sd = -1;
        for (ptr = infoptr; ptr != NULL; ptr = ptr->ai_next) {

            getnameinfo(ptr->ai_addr, ptr->ai_addrlen, host, sizeof (host), NULL, 0, NI_NUMERICHOST);
            //puts(host);
        std::cout<<"host: "<<host<<" "<<(ptr->ai_family==AF_INET)<<std::endl;
            sd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (sd < 0) {
                printf("Error creating socket.\n");
                break;
            }
            switch (ptr->ai_family) {
            case AF_UNSPEC:
                printf("Unspecified\n");
                break;
            case AF_INET:
                printf("AF_INET (IPv4)\n");
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
                printf("\tIPv4 address %s\n",
                    inet_ntoa(sockaddr_ipv4->sin_addr) );
                break;
            case AF_INET6:
                printf("AF_INET6 (IPv6)\n");
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
                printf("Other %ld\n", ptr->ai_family);
                break;
        }
        printf("\tSocket type: ");
        switch (ptr->ai_socktype) {
            case 0:
                printf("Unspecified\n");
                break;
            case SOCK_STREAM:
                printf("SOCK_STREAM (stream)\n");
                break;
            case SOCK_DGRAM:
                printf("SOCK_DGRAM (datagram) \n");
                break;
            case SOCK_RAW:
                printf("SOCK_RAW (raw) \n");
                break;
            case SOCK_RDM:
                printf("SOCK_RDM (reliable message datagram)\n");
                break;
            case SOCK_SEQPACKET:
                printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_socktype);
                break;
        }
        printf("\tProtocol: ");
        switch (ptr->ai_protocol) {
            case 0:
                printf("Unspecified\n");
                break;
            case IPPROTO_TCP:
                printf("IPPROTO_TCP (TCP)\n");
                break;
            case IPPROTO_UDP:
                printf("IPPROTO_UDP (UDP) \n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_protocol);
                break;
        }
        std::cout<<"connect: "<<host<<" "<<std::endl;
            if (connect(sd, ptr->ai_addr, ptr->ai_addrlen) == 0) {
                std::cout<<"connected: "<<std::endl;
                break;
            }
            close(sd);
            printf("Error connecting to server.\n");
            return false;
        }

        freeaddrinfo(infoptr);
        //int s = socket(AF_UNSPEC, SOCK_STREAM, 0);
        if (sd < 0) {
            //printf("Error creating socket.\n");
            return false;
        }

        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        const SSL_METHOD *meth = TLS_client_method();
        SSL_CTX *ctx = SSL_CTX_new (meth);
        ssl = SSL_new (ctx);
        if (!ssl) {
            printf("Error creating SSL.\n");
            log_ssl();
            return false;
        }
        sock = SSL_get_fd(ssl);
        SSL_set_fd(ssl, sd);
        int err = SSL_connect(ssl);
        if (err <= 0) {
            printf("Error creating SSL connection.  err=%x\n", err);
            log_ssl();
            fflush(stdout);
            return false;
        }
        printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
        std::string escapedUrl=url::Url::escape_reserved_unsafe(urlStr);
std::cout<<"escapedUrl "<<escapedUrl<<std::endl;
        std::string request = "GET "+urlStr+" HTTP/1.0\r\n\r\n";

        SendPacket(request.c_str());
        std::stringstream ss;
        RecvPacket(ss);
        std::string content=ss.str();
        std::string::size_type n=content.find("\r\n\r\n");
        if(n!=std::string::npos){
        ss.str("");
        ss<<content.substr(n+4);
        }
        apply(ss);
        return true;
        };

        generator<std::pair<int, char *>> read(){
            int len=100;
                len=SSL_read(ssl, buf, 100);
                if(len>=0)buf[len]=0;
                //printf("%s",buf);
                //fprintf(fp, "%s",buf);
            co_yield std::make_pair(len, buf);
        }

        int RecvPacket(std::stringstream& ss)
        {
            int len=100;
            do {
                generator<std::pair<int, char *>> rs = read();
                for(auto it=rs.begin();it !=rs.end();it++){
                    len=(*it).first;
                    if(len>0)ss<<(*it).second;
                }
            } while (len>0);
            if (len < 0) {
                int err = SSL_get_error(ssl, len);
            if (err == SSL_ERROR_WANT_READ)
                    return 0;
                if (err == SSL_ERROR_WANT_WRITE)
                    return 0;
                if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
                    return -1;
            }
            return 0;
        }

        int SendPacket(const char *buf)
        {
            int len = SSL_write(ssl, buf, strlen(buf));
            if (len < 0) {
                int err = SSL_get_error(ssl, len);
                switch (err) {
                case SSL_ERROR_WANT_WRITE:
                    return 0;
                case SSL_ERROR_WANT_READ:
                    return 0;
                case SSL_ERROR_ZERO_RETURN:
                case SSL_ERROR_SYSCALL:
                case SSL_ERROR_SSL:
                default:
                    return -1;
                }
            }
            return 0;
        }

        void log_ssl()
        {
            int err;
            while (err = ERR_get_error()) {
                char *str = ERR_error_string(err, 0);
                if (!str)
                    return;
                printf(str);
                printf("\n");
                fflush(stdout);
            }
        }

    };
}
