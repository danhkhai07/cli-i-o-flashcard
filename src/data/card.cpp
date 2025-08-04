#include "data/card.h"

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <iomanip>

Grade inputToGrade(const std::string& input) {
    static std::unordered_map<std::string, Grade> gradeMap = {
        { "Again", Grade::Again }, { "Hard", Grade::Hard }, { "Good", Grade::Good }, { "Easy", Grade::Easy }
    };
    return gradeMap[input];
}

void Card::review(const Grade& grade){
    static std::unordered_map<CardState, std::function<void(const Grade& grade)>> stateHandler = {
        {CardState::New, [this](const Grade& grade){ learnHandler(grade); } },
        {CardState::Learn, [this](const Grade& grade){ learnHandler(grade); } },
        {CardState::Review, [this](const Grade& grade){ reviewHandler(grade); } },
        {CardState::Lapse, [this](const Grade& grade){ lapseHandler(grade); } }
    };
    stateHandler[state](grade);
    adjustInterval();
    printNextReview();
}

void Card::printNextReview() {
    if (interval < 1.0)
        std::cout << "Next review in " << interval * 1440 << " minutes\n";
    else
        std::cout << "Next review in " << interval << " days\n";
}

double Card::learnHandler(const Grade& grade){
    static std::unordered_map<Grade, int> presetStep = {
        {Grade::Again, 0},
        {Grade::Hard, 1},
        {Grade::Good, 2}
    };

    switch (grade)
    {
        case Grade::Again:
            interval = learningStep[presetStep[grade]];
            currentStep = 0;
            break;
        case Grade::Hard:
            interval = learningStep[presetStep[grade]];
            currentStep = 1;
            break;
        case Grade::Good:
            if (currentStep > 1) {
                interval = 1;
                state = CardState::Review;
                break;
            }
            interval = learningStep[presetStep[grade]];
            currentStep = 2;
            break;
        case Grade::Easy:
            interval = 2;
            break;
    }
    return interval;
}

double Card::reviewHandler(const Grade& grade){
    if (grade == Grade::Again){
        state = CardState::Lapse;
        currentStep = 1;
        interval = learningStep[currentStep];
    } else {
        auto mult = [this, grade](){
            if (grade == Grade::Hard) return 1.2;
            if (grade == Grade::Good) return easeFactor;
            return easeFactor * 1.3; // grade == Grade::Easy
        };
        interval *= mult();
        adjustEaseFactor(grade);
    }
    return interval;
}

double Card::lapseHandler(const Grade& grade){
    static std::unordered_map<Grade, int> presetStep = {
        {Grade::Again, 1},
        {Grade::Hard, 2}
    };

    switch (grade)
    {
        case Grade::Again:
            interval = learningStep[presetStep[grade]];
            break;
        case Grade::Hard:
            interval = learningStep[presetStep[grade]];
            break;
        case Grade::Good:
            interval = 1;
            state = CardState::Review;
            break;
        case Grade::Easy:
            interval = 2;
            state = CardState::Review;
            break;
    }
    return interval;
}

void Card::adjustEaseFactor(const Grade& grade){
    if (grade == Grade::Hard) easeFactor -= 0.15;
    if (grade == Grade::Easy){
        easeFactor += 0.15;
        return;
    }    
    easeFactor = std::max(easeFactor, 1.3);
}

void Card::adjustInterval(){
    if (interval >= 1) interval = (int) interval;
}