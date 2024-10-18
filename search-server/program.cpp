#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) { // разбиение строки на слова
    vector<string> words;
    string word;

    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) { // добавление док-ов и вычисление tf
        const vector<string> words = SplitIntoWordsNoStop(document);
        document_count_++;

        for (const string& word : words) {
            double inv_word_count = 1.0 / words.size();
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });

        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    map<string, map<int, double>> word_to_document_freqs_;
    int document_count_ = 0;
    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;

        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    struct Query {
        set<string> plus_documents;
        set<string> minus_documents;
    };

    Query ParseQuery(const string& text) const {
        Query query_words;

        for (const string& word : SplitIntoWordsNoStop(text)) { // обработка минус и плюс слов
            if (word[0] == '-') {
                string new_word = word.substr(1);
                query_words.minus_documents.insert(new_word);
            } else {
                query_words.plus_documents.insert(word);
            }
        }

        return query_words;
    }

    double ComputeWordInverseDocumentFreq(const string& word) const { // вычисление idf
        return log(document_count_ * 1.0 / word_to_document_freqs_.at(word).size());
    }

    vector<Document> FindAllDocuments(const Query& query_words) const { // считаем релевантость 
        vector<Document> matched_documents;
        map<int, double> id_relevance;

        if (query_words.plus_documents.empty()) {
            return {};
        }

        // обработка минус документов
        for (const auto& [word, word_id] : word_to_document_freqs_) {
            if (query_words.minus_documents.count(word) != 0) {
                for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    id_relevance.erase(document_id);
                }
                continue;
            }

            // обработка плюс документов
            if (query_words.plus_documents.count(word) != 0) {
                for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
                    id_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        // проверка документов на минус слова и исключение док-ов
        for (auto it = id_relevance.begin(); it != id_relevance.end();) {
            bool contains_minus_word = false;

            for (const auto& [word, word_id] : word_to_document_freqs_) {
                if (query_words.minus_documents.count(word) != 0) {
                    for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                        if (document_id == it->first) {
                            contains_minus_word = true;
                            break;
                        }
                    }
                    if (contains_minus_word) break;
                }
            }

            if (!contains_minus_word) {
                matched_documents.push_back({it->first, it->second});
                it = id_relevance.erase(it);
            } else {
                ++it;
            }
        }

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}
