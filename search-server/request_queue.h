#pragma once
// request_queue.h

#include "search_server.h"
#include <deque>
#include <algorithm>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
   
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) ;
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    void AddToDeq(bool is_empty);

    struct QueryResult {
        bool is_empty;
    };

    std::deque<QueryResult> requests_;
    static constexpr int min_in_day_ = 1440;
    const SearchServer& search_server_;
};
 template <typename DocumentPredicate>
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate){
        std::vector<Document> result = search_server_.FindTopDocuments(raw_query, document_predicate);
        AddToDeq(result.empty());
        return result;
    }
