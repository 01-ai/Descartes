#pragma once

#include <brpc/server.h>
#include <google/protobuf/util/json_util.h>
#include "server.pb.h"

#include "descartes_index.h"

namespace descartes {

using descartes::server::HttpRequest;
using descartes::server::HttpResponse;

class DescartesService : public descartes::server::Service {
public:
    DescartesService() = default;

    void add_vector(::google::protobuf::RpcController *controller,
                    const HttpRequest *request,
                    HttpResponse *response,
                    ::google::protobuf::Closure *done) override;

    void search(::google::protobuf::RpcController *controller,
                const HttpRequest *request,
                HttpResponse *response,
                ::google::protobuf::Closure *done) override;

    void dump(::google::protobuf::RpcController *controller,
              const HttpRequest *request,
              HttpResponse *response,
              ::google::protobuf::Closure *done) override;

    void refine_index(::google::protobuf::RpcController *controller,
                      const HttpRequest *request,
                      HttpResponse *response,
                      ::google::protobuf::Closure *done) override;

    void current_doc_cnt(::google::protobuf::RpcController *controller,
                         const HttpRequest *request,
                         HttpResponse *response,
                         ::google::protobuf::Closure *done) override;

    int Init(const std::string &path);

private:
    GraphIndexPtr _index;
};

}