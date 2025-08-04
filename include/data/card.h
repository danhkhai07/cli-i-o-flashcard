#pragma once

#include <string>
#include <vector>

enum class Grade {Again, Hard, Good, Easy};
enum class CardState {New, Learn, Review, Lapse};

Grade inputToGrade(const std::string& input);

struct Card {
    public:
        std::string front, back;

        CardState state = CardState::New;
        double easeFactor = 2.5;
        double interval = 0;
        int currentStep = 0;
        std::vector<double> learningStep = {1.0/1440, 10.0/1440, 15.0/1440};

        void review(const Grade& grade);

    private:
        void printNextReview();
        double learnHandler(const Grade& grade);
        double reviewHandler(const Grade& grade);
        double lapseHandler(const Grade& grade);
        void adjustEaseFactor(const Grade& grade);
        void adjustInterval();
};