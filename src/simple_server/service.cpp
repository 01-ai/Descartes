#include "service.h"

#define RETURN_IF_NOT_HTTP_METHOD(CONTROLLER, METHOD, RSP, STATUS)   \
    if (CONTROLLER->http_request().method() != METHOD) {             \
        LOG(WARNING) << "not support method " << METHOD;             \
        UnknownMethod(CONTROLLER, METHOD, RSP, STATUS);              \
        return;                                                      \
    }

namespace descartes {

using descartes::server::Status;
using descartes::server::Response;

enum class StatusCode {
    OK = 0,
    ERROR = 10001,
    UNKNOWN_METHOD = 10002,
    INVALID_REQUEST = 10003,
    INDEX_ERROR = 10004
};

static void inline SetStatus(StatusCode ret, Status *status)
{
    status->set_code(static_cast<int>(ret));
    if (ret == StatusCode::OK) {
        status->set_reason("success");
    } else if (status->reason().empty()) {
        status->set_reason("error");
    }
}

static inline void SerializeResponse(const google::protobuf::Message &response,
                                     brpc::Controller *brpcController)
{
    brpcController->http_response().set_content_type("application/json");
    std::string jsonResp;
    google::protobuf::util::JsonOptions options;
    options.always_print_primitive_fields = true;
    auto stat = google::protobuf::util::MessageToJsonString(response, &jsonResp, options);
    if (!stat.ok()) {
        LOG(ERROR) << "Can't serialize PB response to json. message[" << response.ShortDebugString() << "].";
    } else {
        brpcController->response_attachment().append(jsonResp);
    }
}

static inline void UnknownMethod(brpc::Controller *controller,
                                 int allowed_method,
                                 google::protobuf::Message *rsp,
                                 Status *status)
{
    SetStatus(StatusCode::UNKNOWN_METHOD, status);
    controller->http_response().set_status_code(brpc::HTTP_STATUS_METHOD_NOT_ALLOWED);
    status->mutable_reason()->append(": invalid http method");
    const char *allowed = nullptr;
    switch (allowed_method) {
    case brpc::HTTP_METHOD_POST:
        allowed = "POST";
        break;
    case brpc::HTTP_METHOD_GET:
        allowed = "GET";
        break;
    case brpc::HTTP_METHOD_PUT:
        allowed = "PUT";
        break;
    case brpc::HTTP_METHOD_DELETE:
        allowed = "DELETE";
        break;
        // default ignore
    }
    if (allowed) {
        controller->http_response().SetHeader("Allowed", allowed);
    }
    SerializeResponse(*rsp, controller);
}

void DescartesService::add_vector(::google::protobuf::RpcController *controller,
        const HttpRequest *request,
        HttpResponse *response,
        ::google::protobuf::Closure *done)
{
    brpc::ClosureGuard doneGuard(done);

    auto *brpcController = dynamic_cast<brpc::Controller *>(controller);
    Status status;
    RETURN_IF_NOT_HTTP_METHOD(brpcController, brpc::HTTP_METHOD_POST, &status, &status);
    const std::string &httpBody = brpcController->request_attachment().to_string();
    descartes::server::AddVectorRequest pbRequest;
    auto stat = google::protobuf::util::JsonStringToMessage(httpBody, &pbRequest);
    StatusCode code = StatusCode::OK;
    if (!stat.ok()) {
        code = StatusCode::INVALID_REQUEST;
        status.set_reason(stat.ToString());
    } else {
        for (const auto &entity : pbRequest.rows()) {
            const float *vec = entity.vector().data();
            if (vec != nullptr) {
                int ret = _index->AddVector(vec, entity.vector_size() * sizeof(float), entity.key());
                if (ret != 0) {
                    code = StatusCode::INDEX_ERROR;
                    status.set_reason("index add vector failed");
                    break;
                }
            } else {
                code = StatusCode::INVALID_REQUEST;
                status.set_reason("vector is empty");
                break;
            }
        }
    }
    SetStatus(code, &status);
    SerializeResponse(status, brpcController);
}

void DescartesService::search(::google::protobuf::RpcController *controller,
        const HttpRequest *request,
        HttpResponse *response,
        ::google::protobuf::Closure *done)
{
    brpc::ClosureGuard doneGuard(done);
    auto *brpcController = dynamic_cast<brpc::Controller *>(controller);
    descartes::server::SearchRequest pbRequest;
    descartes::server::SearchResponse pbResponse;
    Status &status = *pbResponse.mutable_status();
    RETURN_IF_NOT_HTTP_METHOD(brpcController, brpc::HTTP_METHOD_POST, &status, &status);
    const std::string &httpBody = brpcController->request_attachment().to_string();
    auto stat = google::protobuf::util::JsonStringToMessage(httpBody, &pbRequest);
    StatusCode code = StatusCode::OK;
    if (!stat.ok()) {
        code = StatusCode::INVALID_REQUEST;
        status.set_reason(stat.ToString());
    } else {
        SearchContext ctx;
        ctx.topk = pbRequest.topk();
        ctx.searchResCnt = pbRequest.search_res_cnt();
        const float *vec = pbRequest.vector().data();
        if (vec != nullptr) {
            int ret = _index->Search(vec, pbRequest.vector().size() * sizeof(float), ctx);
            if (ret != 0) {
                code = StatusCode::INDEX_ERROR;
                status.set_reason("index search failed, ret: " + std::to_string(ret));
            } else {
                for (const auto &i : ctx.results) {
                    auto entity = pbResponse.mutable_entities()->Add();
                    entity->set_key(i.first);
                    entity->set_score(i.second);
                }
            }
        } else {
            code = StatusCode::INVALID_REQUEST;
            status.set_reason("vector is empty");
        }
    }
    SetStatus(code, &status);
    SerializeResponse(pbResponse, brpcController);
}

void DescartesService::dump(::google::protobuf::RpcController *controller,
        const HttpRequest *request,
        HttpResponse *response,
        ::google::protobuf::Closure *done)
{
    brpc::ClosureGuard doneGuard(done);
    auto *brpcController = dynamic_cast<brpc::Controller *>(controller);
    Status status;
    RETURN_IF_NOT_HTTP_METHOD(brpcController, brpc::HTTP_METHOD_PUT, &status, &status);
    StatusCode code = StatusCode::OK;
    if (_index->Dump() != 0) {
        code = StatusCode::INDEX_ERROR;
        status.set_reason("index dump failed");
    }
    SetStatus(code, &status);
    SerializeResponse(status, brpcController);
}

void DescartesService::refine_index(::google::protobuf::RpcController *controller,
        const HttpRequest *request,
        HttpResponse *response,
        ::google::protobuf::Closure *done)
{
    brpc::ClosureGuard doneGuard(done);
    auto *brpcController = dynamic_cast<brpc::Controller *>(controller);
    Status status;
    RETURN_IF_NOT_HTTP_METHOD(brpcController, brpc::HTTP_METHOD_PUT, &status, &status);
    StatusCode code = StatusCode::OK;
    descartes::server::RefineRequest pbRequest;
    const std::string &httpBody = brpcController->request_attachment().to_string();
    auto stat = google::protobuf::util::JsonStringToMessage(httpBody, &pbRequest);
    if (!stat.ok()) {
        code = StatusCode::INVALID_REQUEST;
        status.set_reason(stat.ToString());
    } else {
        int ret = _index->RefineIndex();
        if (ret != 0) {
            code = StatusCode::INDEX_ERROR;
            status.set_reason("index refine failed, ret: " + std::to_string(ret));
        }
    }
    SetStatus(code, &status);
    SerializeResponse(status, brpcController);
}

void DescartesService::current_doc_cnt(::google::protobuf::RpcController *controller,
        const HttpRequest *request,
        HttpResponse *response,
        ::google::protobuf::Closure *done)
{
    brpc::ClosureGuard doneGuard(done);
    auto *brpcController = dynamic_cast<brpc::Controller *>(controller);
    Response pbResponse;
    Status &status = *pbResponse.mutable_status();
    RETURN_IF_NOT_HTTP_METHOD(brpcController, brpc::HTTP_METHOD_GET, &status, &status);
    pbResponse.set_current_doc_cnt(_index->GetCurrentDocCnt());
    SetStatus(StatusCode::OK, &status);
    SerializeResponse(pbResponse, brpcController);
}

int DescartesService::Init(const std::string &path)
{
    _index = CreateGraphIndex();
    if (_index == nullptr) {
        return -1;
    }
    return _index->Init(path);
}

}