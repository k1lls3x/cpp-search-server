// request_queue.cpp
#include "request_queue.h"

#include <algorithm>

RequestQueue::RequestQueue(const SearchServer& search_server) 
    : search_server_(search_server) {}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    std::vector<Document> result = search_server_.FindTopDocuments(raw_query, status);
    AddToDeq(result.empty());
    return result;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    std::vector<Document> result = search_server_.FindTopDocuments(raw_query);
    AddToDeq(result.empty());
    return result;
}

int RequestQueue::GetNoResultRequests() const {
    return std::count_if(begin(requests_), end(requests_), [](const QueryResult& result) {
        return result.is_empty;
    });
}

void RequestQueue::AddToDeq(bool is_empty) {
    requests_.push_back(QueryResult{is_empty});
    if (requests_.size() > min_in_day_) {
        requests_.pop_front();
    }
}
