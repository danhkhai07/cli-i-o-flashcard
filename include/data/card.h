#pragma once

#include "date/date.h"
// #include "../utils/date/date.h" // pre production

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum class Grade {Again, Hard, Good, Easy};
enum class CardState {New, Learn, Review, Lapse};

Grade inputToGrade(const std::string& input);

struct Card {
    public:
        Card(){
            calculateGradeTimeIntervals();
        }
        std::string front, back;
        std::unordered_map<Grade, double> gradeTimeIntervals;
        std::string lastRefresh;

        void review(const Grade& grade);
        bool due();

    private:
        CardState state = CardState::New;
        int currentStep = 0;
        double easeFactor = 2.5;
        double interval = 0;
        std::vector<double> learningStep = {1.0/1440, 10.0/1440, 15.0/1440};
        
        std::function<double(Grade)> mult = [this](Grade grade) {
            if (Grade::Hard == grade) return 1.2;
            if (Grade::Good == grade) return easeFactor;
            return easeFactor * 1.3; // Grade::Easy == grade
        };
        static const std::unordered_map<Grade, int> presetLearnStep;
        static const std::unordered_map<Grade, int> presetLapseStep;

        // Handlers
        void learnHandler(const Grade& grade);
        void reviewHandler(const Grade& grade);
        void lapseHandler(const Grade& grade);

        // Utilities
        void calculateGradeTimeIntervals();
        void printGradeTimeInt();
        void printNextReview();
        void adjustEaseFactor(const Grade& grade);
        void adjustInterval();
};