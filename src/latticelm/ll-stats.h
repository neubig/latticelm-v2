#pragma once

#include <cmath>

namespace latticelm {

class LLStats {

public:
    LLStats(const LLStats & rhs) : words_(rhs.words_), unk_(rhs.unk_), lik_(rhs.lik_) { }
    LLStats() : words_(0), unk_(0), correct_(0), lik_(0.0) { }

    LLStats & operator+=(const LLStats & rhs) {
        words_ += rhs.words_;
        unk_ += rhs.unk_;
        lik_ += rhs.lik_;
        correct_ += rhs.correct_;
        return *this;
    }

    float CalcPPL() {
        return exp(-lik_/words_);
    }

    int words_; // Number of words
    int unk_;   // Number of unknown words
    int correct_; // Number of correct predictions
    float lik_;  // Log likelihood

};

}
