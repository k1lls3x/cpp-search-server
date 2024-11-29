#pragma once
#include <iterator>
#include <stdexcept>
#include <vector>
#include <iostream>

template <typename Iterator>
class IteratorRange{
    public:
    explicit IteratorRange(Iterator b, Iterator e ,size_t page_size ):beg_it(b),end_it(e),page_size(page_size){}

    auto begin()  const {
        return   beg_it;
    }
    auto end()  const {
        return end_it;
    }
    size_t size()  const {
        return page_size;
    }

    private:
    Iterator beg_it,end_it;
    size_t page_size;
};


template <typename Iterator>
class Paginator {
    public:
       explicit  Paginator(Iterator beg, Iterator en, size_t page_size) {
        if (page_size == 0) {
            throw std::invalid_argument("Page size must be greater than 0");
        }

 
         for (auto current = beg; current != en;) {

            auto remaining_distance = std::distance(current, en);
            auto next = current;

            std::advance(next, std::min(static_cast<size_t>(remaining_distance), page_size));
            pages_.emplace_back(current, next, std::distance(current, next));

            current = next;
        }   

    }
          
    auto begin()  const {
        return   pages_.begin();
    }
    auto end()  const {
        return pages_.end();
    }
    size_t size() const  {
        return pages_.size();
    }

 private:
    std::vector<IteratorRange<Iterator>> pages_;
};
template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const IteratorRange<Iterator>& range) {
    bool first = true; 
    for (auto it = range.begin(); it != range.end(); ++it) {
        if (!first) {
            os << ""; 
        }
        os << *it; 
        first = false; 
    }
    return os;
}
template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}