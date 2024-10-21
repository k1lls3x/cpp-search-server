#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <numeric>
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

// int ReadLineRating(){
//     int rating =0;
//     cin >> rating;
//     ReadLine();
//     return rating;
// }

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
    int rating;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

   void AddDocument(int document_id, const string& document, const vector<int>& ratings) { // добавление док-ов и вычисление tf
        const vector<string> words = SplitIntoWordsNoStop(document);
        document_count_++;
        double inv_word_count = 1.0 / words.size(); // -факап

        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
       
        rating_[document_id] = ComputeAverageRating(ratings);
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        for (auto& doc : matched_documents) {
        if (rating_.count(doc.id)) {
            doc.rating = rating_.at(doc.id);
        } else {
            doc.rating = 0; 
        }
    }

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
    map<int,int> rating_;

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

    vector<Document> FindAllDocuments(const Query& query) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_documents) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }

        for (const string& word : query.minus_documents) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({document_id, relevance, rating_.count(document_id) ? rating_.at(document_id) : 0});
        }
        return matched_documents;
    }

    int ComputeAverageRating(const vector<int>& ratings) {
      
        if(ratings.empty()){return 0; }

    int sum = accumulate(ratings.begin(),ratings.end(),0);
    return sum / static_cast<int>(ratings.size());
}


};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();

    for (int document_id = 0; document_id < document_count; ++document_id) {
        string document = ReadLine();
        
        // Читаем оценки
        int num_ratings = 0;
        cin >> num_ratings;
        vector<int> ratings(num_ratings);
        for (int i = 0; i < num_ratings; ++i) {
            cin >> ratings[i];
        }
        ReadLine(); // Чтобы убрать оставшийся перевод строки после чтения оценок

        search_server.AddDocument(document_id, document, ratings);
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    
    for (const auto& document : search_server.FindTopDocuments(query)) {
    cout << "{ document_id = "s << document.id << ", "
         << "relevance = "s << document.relevance << ", "
         << "rating = "s << document.rating << " }"s << endl;
}
}
