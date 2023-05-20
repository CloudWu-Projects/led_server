//
// throttling_proxy.cpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
#include <asio/experimental/channel.hpp>
#include <iostream>

using asio::as_tuple;
using asio::awaitable;
using asio::buffer;
using asio::co_spawn;
using asio::detached;
using asio::experimental::channel;
using asio::io_context;
using asio::ip::tcp;
using asio::steady_timer;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;
using namespace asio::experimental::awaitable_operators;
using namespace std::literals::chrono_literals;

using token_channel = channel<void(asio::error_code, std::size_t)>;
#include <map>
namespace ASIOTcpServer {
	struct Sesson {
		Sesson(tcp::socket& s) :server(s.get_executor()), client(std::move(s)) {

		}
		void send(char* _data, int nlen)
		{
			memcpy(data, _data, nlen);
			client.write_some(buffer(data, nlen));
		}

		char  data[4096];
		tcp::socket client;
		tcp::socket server;
	};

	using SessonPtr = std::shared_ptr<Sesson>;

	struct SessonMgr {
		std::recursive_mutex g_mutex;
		std::map<int, SessonPtr> g_maps;
		void append(int key, SessonPtr ptr) {
			std::lock_guard  lock_guard(g_mutex);
			g_maps.emplace(key, ptr);

			checkMapSize();
		}
		void checkMapSize() {
			std::lock_guard lock(g_mutex);
			printf("%d \n", g_maps.size());
		}
		void remove(int key)
		{
			std::lock_guard lock(g_mutex);
			g_maps.erase(key);
			checkMapSize();
		}
		int boardcast(char* data, int nlen) {
			std::lock_guard lock(g_mutex);
			int icount = 0;
			for (auto& a : g_maps)
			{
				a.second->send(data, nlen);
				icount++;
			}
			return icount;
		}
	};

	class TcpServer {
	public:
		int start(const char*localPort,const char*target_port)
		{
			try
			{
				io_context ctx;
				const char* localAddr = "127.0.0.1";
				auto listen_endpoint =
					*tcp::resolver(ctx).resolve(localAddr, localPort, tcp::resolver::passive);

				auto target_endpoint =
					*tcp::resolver(ctx).resolve(localAddr, target_port);

				tcp::acceptor acceptor(ctx, listen_endpoint);
				co_spawn(ctx, listen(acceptor, target_endpoint), detached);				
				ctx.run();
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
			}
			return 0;
		}
		int boardCast(char* data, int nLen)
		{
			return sessonMgr.boardcast(data, nLen);
		}
	private:
		SessonMgr sessonMgr;

		awaitable<void> produce_tokens(std::size_t bytes_per_token,
			steady_timer::duration token_interval, token_channel& tokens)
		{
			steady_timer timer(co_await this_coro::executor);
			for (;;)
			{
				co_await tokens.async_send(
					asio::error_code{}, bytes_per_token,
					use_awaitable);

				timer.expires_after(token_interval);
				co_await timer.async_wait(use_awaitable);

			}
		}



		awaitable<void> transfer(tcp::socket& from,
			tcp::socket& to, token_channel& tokens)
		{
			std::array<unsigned char, 4096> data;
			for (;;)
			{
				std::size_t bytes_available = co_await tokens.async_receive(use_awaitable);
				while (bytes_available > 0)
				{
					std::size_t n = co_await from.async_read_some(
						buffer(data, bytes_available), use_awaitable);
					co_await async_write(to, buffer(data, n), use_awaitable);

					bytes_available -= n;
				}
			}
		}

		awaitable<void> proxy(tcp::socket aclient, tcp::endpoint target)
		{
			SessonPtr sessonPtr = std::make_shared<Sesson>(aclient);

			constexpr std::size_t number_of_tokens = 100;
			constexpr size_t bytes_per_token = 20 * 1024;
			constexpr steady_timer::duration token_interval = 100ms;

			auto ex = sessonPtr->client.get_executor();

			token_channel client_tokens(ex, number_of_tokens);
			token_channel server_tokens(ex, number_of_tokens);
			;
			sessonMgr.checkMapSize();
			try {
				co_await sessonPtr->server.async_connect(target, use_awaitable);

				sessonMgr.append(sessonPtr->server.local_endpoint().port(), sessonPtr);
				co_await(
					produce_tokens(bytes_per_token, token_interval, client_tokens) &&
					transfer(sessonPtr->client, sessonPtr->server, client_tokens) &&
					produce_tokens(bytes_per_token, token_interval, server_tokens) &&
					transfer(sessonPtr->server, sessonPtr->client, server_tokens)
					);

			}
			catch (std::exception& e)
			{
				std::printf("echo Exception: %s\n", e.what());
			}
			{auto client_raddress = sessonPtr->client.remote_endpoint().address();
			auto client_rPort = sessonPtr->client.remote_endpoint().port();
			auto client_laddress = sessonPtr->client.local_endpoint().address();
			auto client_lPort = sessonPtr->client.local_endpoint().port();
			std::cout << "client:local:" << client_raddress.to_string() << "   :" << client_rPort << std::endl;
			std::cout << "client:remote:" << client_laddress.to_string() << "   :" << client_lPort << std::endl;
			}
			{auto client_raddress = sessonPtr->server.remote_endpoint().address();
			auto client_rPort = sessonPtr->server.remote_endpoint().port();
			auto client_laddress = sessonPtr->server.local_endpoint().address();
			auto client_lPort = sessonPtr->server.local_endpoint().port();

			std::cout << "server:local:" << client_raddress.to_string() << "   :" << client_rPort << std::endl;
			std::cout << "server:remote:" << client_laddress.to_string() << "   :" << client_lPort << std::endl;
			}
			sessonMgr.remove(sessonPtr->server.local_endpoint().port());
		}

		awaitable<void> listen(tcp::acceptor& acceptor, tcp::endpoint target)
		{
			for (;;)
			{
				auto [e, client] = co_await acceptor.async_accept(as_tuple(use_awaitable));
				if (!e)
				{
					auto ex = client.get_executor();
					co_spawn(ex, proxy(std::move(client), target), detached);
				}
				else
				{
					std::cerr << "Accept failed: " << e.message() << "\n";
					steady_timer timer(co_await this_coro::executor);
					timer.expires_after(100ms);
					co_await timer.async_wait(use_awaitable);
				}
			}
		}
	
	};
};
