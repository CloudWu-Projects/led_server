#pragma once

#include <fmt/fmt.h>
#define SPDLOG_ACTIVE_LEVEL 0
#ifdef WIN32
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#endif
#include <spdlog/spdlog.h>
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <filesystem>
#include <string>

inline void initLogger(const char *folderPath)
{
	if (folderPath == nullptr)
		folderPath = ".\\";
	auto pCreateLogger = [&](std::string loggerName, bool bDefault = false)
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);

		std::string fileName = std::string(folderPath) + "/logs/" + loggerName + ".txt";
		auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(fileName, 0, 0, false, 10);
		file_sink->set_level(spdlog::level::trace);

		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(console_sink);
		sinks.push_back(file_sink);

		auto logger = std::make_shared<spdlog::logger>(loggerName, sinks.begin(), sinks.end());
		//{ console_sink, file_sink }
		//logger->flush_on(spdlog::level::info);
		logger->set_level(spdlog::level::debug);
		logger->set_pattern("[%x %X] %L %t [%s:%#-%!]>> %v");
		logger->info("########################STARt##############################");

		spdlog::register_logger(logger);
		if (bDefault)
			spdlog::set_default_logger(logger);
		//logger->flush();
	};
	/**/

	pCreateLogger("mainLog", true);
	spdlog::set_pattern("[%x %X] %L %t [%s:%#-%!]>> %v");
	spdlog::flush_every(std::chrono::seconds(5));
	spdlog::flush_on(spdlog::level::info);
}
