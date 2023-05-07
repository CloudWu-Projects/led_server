#pragma once
#include "httplib.h"

namespace httplib
{
	class HttpServerWrapper : public httplib::Server
	{
	public:
		HttpServerWrapper():HttpServerWrapper("unknown") {};

		static void makeString(const char* pattern, const char* Method, std::string& result)
		{
			result += R"(<li> <button onclick="return UserAction( ')" + std::string(pattern) + R"(' ); ">..</button>)" + std::string(Method) + " --->    <a href = '" + std::string(pattern) + "'> " + std::string(pattern) + "</a></li>";
		}

		HttpServerWrapper(const std::string& moduleName, int httpPort = 0, int webSocketPort = 0)
		{
			httplib::Server::Get("/", [&](const httplib::Request& req, httplib::Response& res)
				                     {
					                     res.set_header("Access-Control-Allow-Origin", "*");
					                     res.set_content(getHttpHtml(), "text/html");
				                     });

			if (httpPort != 0 && webSocketPort != 0)
			{
				char buff[1025];
				sprintf(buff, R"({"moduleName":"%s","httpPort":%d,"webSocketPort":%d })", moduleName.c_str(), httpPort, webSocketPort);
				m_getModuleInfo = buff;

				this->Get("/getModuleInfo", [&](const httplib::Request& req, httplib::Response& res)
					          {
						          res.set_header("Access-Control-Allow-Origin", "*");
						          res.set_content(m_getModuleInfo, "text/json");
					          });
			}
		}

#define MAKESTRING makeString(pattern,__FUNCTION__,m_httpHTML);

		HttpServerWrapper& Get(const char* pattern, Handler handler)
		{
			MAKESTRING;
			httplib::Server::Get(pattern, std::move(handler));
			return *this;
		}

		HttpServerWrapper& Post(const char* pattern, Handler handler)
		{
			MAKESTRING;
			httplib::Server::Post(pattern, std::move(handler));
			return *this;
		}

		std::string& getHttpHtml()
		{
			return m_httpHTML;
		}

	private:
		std::string m_getModuleInfo;
		std::string m_httpHTML = R"(<script>
function UserAction(url) {
console.log(url);
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {           
			 document.getElementById('result').innerHTML=this.responseText;         
		}
	};
    xhttp.open("GET",url);    
    xhttp.send();
}

window.onload = (event)=>{
	var childNode = document.createElement('p');
childNode.setAttribute('id',"result");
document.getElementsByTagName('body')[0].appendChild(childNode);
};
</script>
)";
	};
};
