#pragma once

#include <vector>
#include <coroutine>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <functional>

#include <coroutine>
#include <stdexcept>
#include <thread>

#ifdef __WIN32__
# include <winsock2.h>
#else
# include <sys/socket.h>
#include <arpa/inet.h>
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
    public:
        TCPReader() = default;
        TCPReader(const TCPReader& orig) = delete;
        virtual ~TCPReader() = default;

        void operator()(std::string& url, std::function<void(std::istream& content)> apply){
        int s;
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) {
            printf("Error creating socket.\n");
            return;
        }
        struct sockaddr_in sa;
        memset (&sa, 0, sizeof(sa));
        sa.sin_family      = AF_INET;
        sa.sin_addr.s_addr = inet_addr("128.6.244.95"); // address of files.rcsb.org
        sa.sin_port        = htons (443); 
        socklen_t socklen = sizeof(sa);
        if (connect(s, (struct sockaddr *)&sa, socklen)) {
            printf("Error connecting to server.\n");
            return;
        }
        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        const SSL_METHOD *meth = TLSv1_2_client_method();
        SSL_CTX *ctx = SSL_CTX_new (meth);
        ssl = SSL_new (ctx);
        if (!ssl) {
            printf("Error creating SSL.\n");
            log_ssl();
            return;
        }
        sock = SSL_get_fd(ssl);
        SSL_set_fd(ssl, s);
        int err = SSL_connect(ssl);
        if (err <= 0) {
            printf("Error creating SSL connection.  err=%x\n", err);
            log_ssl();
            fflush(stdout);
            return ;
        }
        printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

        std::string request = "GET "+url+" HTTP/1.0\r\n\r\n";

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
                auto it=rs.begin();
                if(it !=rs.end()){
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
