

#include <chrono>
#include <memory>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class my_formatter_flag : public spdlog::custom_flag_formatter
{
public:
    void format(const spdlog::details::log_msg &, const std::tm &, spdlog::memory_buf_t &dest) override
    {
        std::string some_txt = "mark-flag";
        dest.append(some_txt.data(), some_txt.data() + some_txt.size());
    }
    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<my_formatter_flag>();
    }
};

int main() {

    spdlog::info("hello world");

    auto logger = spdlog::stdout_color_mt<spdlog::async_factory>("console");
    logger->info("hello world too!!");
    spdlog::get("console")->info("hello world too too !!!");

    auto sink1 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto sink2 = std::make_shared<spdlog::sinks::basic_file_sink_mt>("mark.txt");


    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<my_formatter_flag>('*').set_pattern("[%n] [%*] [%^%l%$] %v");
    sink1->set_formatter(std::move(formatter));

    sink2->set_pattern("[%^%l%$] %v");

    auto logger1 = std::make_shared<spdlog::logger>(std::string("console1"));

    logger1->sinks().push_back(sink1);
    logger1->sinks().push_back(sink2);
    spdlog::register_logger(logger1);

    spdlog::get("console1")->info("hello world too too too!!!!");

    SPDLOG_INFO("hello mark too!");
    SPDLOG_LOGGER_INFO(spdlog::get("console1"), "hello mark too too !!!");


    auto logger_mark = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("mark", "mark1.txt");
    logger_mark->info("hello logger_mark");

    spdlog::init_thread_pool(8292, 8); //队列大小,线程个数

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(sink1);
    sinks.push_back(sink2);
    auto logger_tp = std::make_shared<spdlog::async_logger>("tp", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);

    logger_tp->info("hello thread pool");

    logger_tp->flush();

    logger_tp->flush_on(spdlog::level::err);

    spdlog::flush_every(std::chrono::seconds(5));

    return 0;
}