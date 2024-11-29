// string_processing.cpp
#include "string_processing.h"

std::vector<std::string> SplitIntoWordsNoStop(const std::string& text, const std::set<std::string>& stop_words) {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!word.empty() && stop_words.find(word) == stop_words.end()) {
            words.push_back(word);
        }
    }
    return words;
}
