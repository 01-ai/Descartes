#include <filesystem>
#include <iostream>
#include <signal.h>

#include "service.h"

#include <brpc/server.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

namespace descartes {
static brpc::Server descartes_server;
}

DEFINE_string(config, "", "descartes index config file");
DEFINE_int32(listening_port, 8080, "descartes server listening port");

static bool ValidateConfig(const char *flagname, const std::string &config)
{
    return !config.empty();
}

DEFINE_validator(config, ValidateConfig);

int main(int argc, char *argv[])
{
    using namespace descartes;
    google::InitGoogleLogging("descartes");
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    descartes::DescartesService *service = new descartes::DescartesService();
    int ret = service->Init(FLAGS_config);
    if (ret != 0) {
        LOG(ERROR) << "Init failed.";
        exit(1);
    }
    ret = descartes_server.AddService(service, brpc::SERVER_OWNS_SERVICE,
        "/add_vector => add_vector,"
        "/search => search,"
        "/dump => dump,"
        "/refine_index => refine_index,"
        "/current_doc_cnt => current_doc_cnt,");

    if (ret != 0) {
        LOG(ERROR) << "Http server add service failed.";
        exit(1);
    }

    ret = descartes_server.Start(FLAGS_listening_port, nullptr);
    if (ret != 0) {
        LOG(ERROR) << "Http server start failed.";
        exit(1);
    }
    google::FlushLogFiles(0);
    descartes_server.RunUntilAskedToQuit();

    LOG(INFO) << "Server exited.";
    google::FlushLogFiles(0);
    return 0;
}
