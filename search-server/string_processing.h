#pragma once
// string_processing.h

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>

template <typename StringContainer>
std::vector<std::string> SplitIntoWords(const StringContainer& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (std::isspace(static_cast<unsigned char>(c))) {
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

template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string> unique_strings;
    for (const auto& s : strings) {
        if (!s.empty()) {
            unique_strings.insert(s);
        }
    }
    return unique_strings;
}

std::vector<std::string> SplitIntoWordsNoStop(const std::string& text, const std::set<std::string>& stop_words);
