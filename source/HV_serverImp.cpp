#include "HV_serverImp.h"
#include "hv_server.h"

#include "hv/TcpClient.h"

#include <iostream>
#include "ledServer.h"

#include "stringHelper.h"

int HV_server::start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server *ledServer)
{
	Imp = new HV_serverImp();

	return Imp->start(httpPort, ledSDKport, ledNeimaPort, ledServer);
}

#define TEST_TLS 0
static const char updateAEnd[4] = {0x00, 0x00, 0x0D, 0x0A};
static int makeNeiMa(char *pDestbuffer, std::vector<std::string> pDataVt)
{
	uint8_t updateAHeader[] = {0x55, 0xAA, 0x00, 0x00, 0x01, 0x01, 0x00, 0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	int send_len = 0;
	int recv_len = 0;
	// char send_buf[1024];
	auto pgetInterCode = [](char *destPos, int codeAreaIdx, std::string str)
	{
		uint8_t InterCodeHeader[] = {0x25, 0x64, 0x69, 0x73, 0x70, 0x30, 0x3A, 0x30};
		InterCodeHeader[5] = 0x30 + codeAreaIdx;
		memcpy(destPos, InterCodeHeader, sizeof(InterCodeHeader));
		destPos += sizeof(InterCodeHeader);
		memcpy(destPos, str.c_str(), str.length());
		return destPos + str.length();
	};
	char *pPos = pDestbuffer;
	memcpy(pPos, updateAHeader, sizeof(updateAHeader));
	pPos += sizeof(updateAHeader);
	for (int i = 0; i < pDataVt.size(); i++)
	{
		pPos = pgetInterCode(pPos, i, pDataVt[i]);
	}
	memcpy(pPos, updateAEnd, sizeof(updateAEnd));
	pPos += sizeof(updateAEnd);
	int nLen = pPos - pDestbuffer;

	return nLen;
}

int HV_serverImp::start(int httpPort, int ledSDKport, int ledNeimaPort, LED_Server *ledServer)
{
	m_ledSever = ledServer;
#ifdef NEED_TCPSERVER_FOR_NEIMA
	tcp_server(ledNeimaPort, ledSDKport);
#endif
	http_server(httpPort);
	return 0;
}

inline int HV_serverImp::list_Handler(HttpRequest *req, HttpResponse *resp)
{
	auto s = m_ledSever->getNetWorkIDList();
	m_tcpSrv.foreachChannel([&](auto channel)
							{
		if (channel->isConnected()) {
			std::string peeraddr = channel->peeraddr();
			s += fmt::format(">> {}", channel->peeraddr());
			} });
	return resp->String(s);
}

inline int HV_serverImp::CreatePGM_Handler(HttpRequest *req, HttpResponse *res)
{
	res->SetHeader("Access-Control-Allow-Origin", "*");
	ExtSeting extSetting;
	std::string sendValue = "test NUll";
	bool isJson = false;
	if (req->Method() == "POST")
	{
		if (req->GetHeader("Content-Type") != "application/json")
		{
			//	res.status = 500;
			//	res.reason = "post with json.";
			//	return;
		}

		/*
		{
		}
		*/
		sendValue = req->Body();
		isJson = true;
	}
	else
	{
	}

	{
		sendValue = req->GetParam("content");
	}
	auto fontSize = req->GetParam("fontsize", "-1");
	if (fontSize != "-1")
	{
		extSetting.FontSize = atoi(fontSize.data());
	}
#ifdef UNICODE
	SPDLOG_DEBUG(L"handle a set key:{}", sendValue);
#else
	SPDLOG_DEBUG("handle a set key:{}", sendValue);
#endif
	std::string htmlContent = "{";
	auto createRet = (req->Path() == "/create_onePGM") ? m_ledSever->create_onPGM_byCode(sendValue, extSetting)
													   : m_ledSever->createPGM_withLspj(sendValue, extSetting);

	htmlContent += fmt::format("\"ret\":{},\"msg\":\"{}\",", std::get<0>(createRet), std::get<1>(createRet));
	htmlContent += fmt::format("\"sendValue\":\"{}\",", (sendValue));
	htmlContent += m_ledSever->getNetWorkIDList();
	htmlContent += "}";
	/**/
	SPDLOG_DEBUG(htmlContent);
	return res->String(htmlContent);
	return res->Json(htmlContent);
}

inline int HV_serverImp::http_server(int httpPort)
{
	router.Static("/", "./webPage");

	router.POST("/leds/add", [this](HttpRequest *req, HttpResponse *resp)
				{
		std::string htmlContent;
			htmlContent = "{";
			htmlContent += true ? "sucess" : "failed";
			htmlContent += "}";
			return resp->Json(htmlContent); });
	router.GET("/leds/list", [this](HttpRequest *req, HttpResponse *resp)
			   { return list_Handler(req, resp); });
	router.GET("/list", [this](HttpRequest *req, HttpResponse *resp)
			   { return list_Handler(req, resp); });

	router.GET("/paths", [this](HttpRequest *req, HttpResponse *resp)
			   { return resp->Json(router.Paths()); });

	router.GET("/get", [](HttpRequest *req, HttpResponse *resp)
			   {
		resp->json["origin"] = req->client_addr.ip;
		resp->json["url"] = req->url;
		resp->json["args"] = req->query_params;
		resp->json["headers"] = req->headers;
		return 200; });

	router.POST("/echo", [](const HttpContextPtr &ctx)
				{ return ctx->send(ctx->body(), ctx->type()); });

	router.POST("/set", [this](HttpRequest *req, HttpResponse *resp)
				{ return CreatePGM_Handler(req, resp); });
	;

	router.GET("/create_onePGM", [this](HttpRequest *req, HttpResponse *resp)
			   { return CreatePGM_Handler(req, resp); });

	router.GET("/create_withPGM", [this](HttpRequest *req, HttpResponse *resp)
			   { return CreatePGM_Handler(req, resp); });

	router.GET("/empty_plot", [this](HttpRequest *req, HttpResponse *resp)
			   {
				   // url = f'{led_server_empty_plot}?ledids={ledids}&empty_plot={empty_plot}&pgmfilepath={pgmfilepath}'
				   std::unique_lock<std::mutex> lock(led_neima_mutex, std::defer_lock);
				   if (!lock.try_lock())
				   {
					   return resp->String("led_neima_mutex locked");
				   }
				   auto ledids = req->GetParam("ledids");
				   auto empty_plot = req->GetParam("empty_plot");
				   auto pgmfilepath = req->GetParam("pgmfilepath");
				   ExtSeting extSetting;
				   auto fontSize = req->GetParam("fontsize", "-1");
				   if(fontSize != "-1")
				   		extSetting.FontSize = atoi(fontSize.data());
				   auto fontColor = req->GetParam("fontcolor", "-1");
				   if(fontColor != "-1")
				   	   extSetting.FontColor = atoi(fontColor.data());

				   std::string htmlContent = "{";
				   auto createRet = m_ledSever->createPGM_withLspj(ledids, empty_plot, pgmfilepath, extSetting);

				   htmlContent += fmt::format("\"ret\":{},\"msg\":{{ {} }},", std::get<0>(createRet), std::get<1>(createRet));
				   htmlContent += fmt::format("\"ledids\":\"{}\",", (ledids));
				   htmlContent += fmt::format("\"empty_plot\":\"{}\",", (empty_plot));
				   htmlContent += fmt::format("\"pgmfilepath\":\"{}\",", (pgmfilepath));
				   htmlContent += m_ledSever->getNetWorkIDList();
				   htmlContent += "}";
				   /**/
				   SPDLOG_DEBUG(htmlContent);
				   return resp->String(htmlContent);
			   });
	router.GET("/reloadpgm", [this](HttpRequest *req, HttpResponse *res)
			   {		
			auto 	sendValue = req->GetParam("key");
			std::string htmlContent;
			htmlContent = "{";
			htmlContent += (IConfig.ReloadPGM(sendValue) ? "sucess" : "failed");
			htmlContent += "}";
			return res->Json(htmlContent); });

	router.GET("/neima", [this](HttpRequest *req, HttpResponse *res)
			   {
		if (auto key = req->GetParam("key"); key != hv::empty_string)
		{
			std::lock_guard<std::mutex> lock(led_neima_mutex);
			auto v = split_string(key);
			int nlen = makeNeiMa(neima, v);
			int a = 0;
			int count = m_tcpSrv.broadcast(neima, nlen);
			std::string htmlJson;
			htmlJson =fmt::format("{{\"ret\":{} ,\"ledCount\":{} ,\"msg\":\"{}\"}}",a,count,"ok");
			return res->Json(htmlJson);
		}
		return res->Json(fmt::format("{{\"ret\":-1 ,\"ledCount\":0 ,\"msg\":\"miss key\"}}")); });

	printf("http:%d\n", httpPort);
	httpServer.registerHttpService(&router);
	httpServer.setPort(httpPort);
	httpServer.setThreadNum(4);
	httpServer.run();
	return 0;
}
std::string localaddr(hio_t *io_)
{
	if (io_ == NULL)
		return "";
	struct sockaddr *addr = hio_localaddr(io_);
	char buf[SOCKADDR_STRLEN] = {0};
	return SOCKADDR_STR(addr, buf);
}

std::string PEERADDR(hio_t *io_)
{
	if (io_ == NULL)
		return "";
	struct sockaddr *addr = hio_peeraddr(io_);
	char buf[SOCKADDR_STRLEN] = {0};
	return SOCKADDR_STR(addr, buf);
}

typedef std::shared_ptr<TcpClient> TcpClientPtr;

void handle_Client(const SocketChannelPtr &channel, int _backend_port)
{
}
inline int HV_serverImp::tcp_server(int _proxy_port, int _backend_port)
{
	hlog_set_level(LOG_LEVEL_DEBUG);
	int listenfd = m_tcpSrv.createsocket(_proxy_port);
	if (listenfd < 0)
	{
		return -20;
	}
	backend_port = _backend_port;
	printf("server listen on port %d, listenfd=%d ...\n", _proxy_port, listenfd);
	m_tcpSrv.onConnection = [this, _backend_port](const SocketChannelPtr &channel)
	{
		std::string peeraddr = channel->peeraddr();
		if (channel->isConnected())
		{

			printf("LED:%s connected! connfd=%d id=%d tid=%ld\n", peeraddr.c_str(), channel->fd(), channel->id(), currentThreadEventLoop->tid());

			TcpClientPtr target = std::make_shared<TcpClient>();
			int connfd = target->createsocket(_backend_port);
			if (connfd < 0)
			{
				printf("LED:%s connect to ledServer(%d) failed!\n", peeraddr.c_str(), _backend_port);
				return;
			}
			target->onConnection = [this, localChannel = channel, _backend_port](const SocketChannelPtr &targetchannel)
			{
				std::string peeraddr = targetchannel->peeraddr();
				if (targetchannel->isConnected())
				{
					printf("%s connect to ledServer(%d)! connfd=%d id=%d tid=%ld\n", peeraddr.c_str(), _backend_port, targetchannel->fd(), targetchannel->id(), currentThreadEventLoop->tid());

					localChannel->onread = [this, targetchannel](Buffer *buf)
					{
						// printf("%s read %d bytes\n", peeraddr.c_str(), buf->size());
						targetchannel->write(buf->data(), buf->size());
					};
					localChannel->onwrite = [this, targetchannel](Buffer *buf)
					{
						// printf("%s write %d bytes\n", peeraddr.c_str(), buf->size());
					};

					targetchannel->onread = [this, localChannel](Buffer *buf)
					{
						localChannel->write(buf->data(), buf->size());
					};
				}
				else
				{
					printf("disconnected to %s! connfd=%d\n", peeraddr.c_str(), targetchannel->fd());
				}
			};
			target->start();

			// hio_t * up1= hio_setup_tcp_upstream(channel->io(), "127.0.0.1", backend_port, false);
			// hio_t * upstream = hio_get_upstream(channel->io());

			// printf("upstream  %x %x\n",up1,upstream);
			// printf("CUR %x %s[%s] connected! \n", channel->io(),			PEERADDR(channel->io()).c_str(), 			localaddr(channel->io()).c_str()			);/**/

			// printf("UP %x %s[%s] connected! \n", upstream,			PEERADDR(upstream).c_str(), 			localaddr(upstream).c_str()			);/**/
		}
		else
		{
			printf("%s disconnected! connfd=%d id=%d tid=%ld\n", peeraddr.c_str(), channel->fd(), channel->id(), currentThreadEventLoop->tid());
		}
	};

	m_tcpSrv.setThreadNum(4);
	m_tcpSrv.setLoadBalance(LB_LeastConnections);
	m_tcpSrv.start();
	return 0;
}
