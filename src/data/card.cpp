#include "data/card.h"
// #include "../../include/data/card.h" // pre production

#include "date/date.h"
// #include "../utils/date/date.h" // pre production

#include <algorithm>
#include <iostream>
#include <iomanip>

const std::unordered_map<Grade, int> Card::presetLearnStep = {
    {Grade::Again, 0},
    {Grade::Hard, 1},
    {Grade::Good, 2}
};

const std::unordered_map<Grade, int> Card::presetLapseStep = {
    {Grade::Again, 1},
    {Grade::Hard, 2}
};

Grade inputToGrade(std::string_view input) {
    static std::unordered_map<std::string, Grade> gradeMap = {
        { "Again", Grade::Again }, { "Hard", Grade::Hard }, { "Good", Grade::Good }, { "Easy", Grade::Easy }
    };
    return gradeMap[std::string(input)];
}

CardState inputToState(std::string_view input) {
    static std::unordered_map<std::string, CardState> stateMap = {
        { "New", CardState::New }, { "Learn", CardState::Learn }, { "Review", CardState::Review }, { "Lapse", CardState::Lapse }
    };
    return stateMap[std::string(input)];
}

void Card::printGradeTimeInt(){
    std::cout << "Again: " << (Card::gradeTimeIntervals[Grade::Again] < 1 ? Card::gradeTimeIntervals[Grade::Again] * 1440 : Card::gradeTimeIntervals[Grade::Again]);
    std::cout << " | Hard: " << (Card::gradeTimeIntervals[Grade::Hard] < 1 ? Card::gradeTimeIntervals[Grade::Hard] * 1440 : Card::gradeTimeIntervals[Grade::Hard]);
    std::cout << " | Good: " << (Card::gradeTimeIntervals[Grade::Good] < 1 ? Card::gradeTimeIntervals[Grade::Good] * 1440 : Card::gradeTimeIntervals[Grade::Good]);
    std::cout << " | Easy: " << Card::gradeTimeIntervals[Grade::Easy] << "\n";
}

void Card::review(const Grade& grade){
    static std::unordered_map<CardState, std::function<void(const Grade& grade)>> stateHandler = {
        {CardState::New, [this](const Grade& grade){ learnHandler(grade); } },
        {CardState::Learn, [this](const Grade& grade){ learnHandler(grade); } },
        {CardState::Review, [this](const Grade& grade){ reviewHandler(grade); } },
        {CardState::Lapse, [this](const Grade& grade){ lapseHandler(grade); } }
    };
    // printGradeTimeInt(); // pre production
    stateHandler[state](grade);
    calculateGradeTimeIntervals();
    adjustInterval();
    // printNextReview(); // pre production
}

void Card::read(const nlohmann::json& cardView){
    std::string tmp;
    front = cardView["front"].get<std::string>();
    back = cardView["back"].get<std::string>();
    lastRefresh = cardView["lastRefresh"].get<std::string>();
    state = inputToState(cardView["state"].get<std::string>());
    currentStep = std::stoi(cardView["currentStep"].get<std::string>());
    tmp = cardView["easeFactor"].get<std::string>();
    easeFactor = std::atof(tmp.c_str());
    tmp = cardView["interval"].get<std::string>();
    interval = std::atof(tmp.c_str());
}

bool Card::due(){
    using namespace std::chrono;

    std::istringstream in(lastRefresh);
    date::sys_time<seconds> last;
    in >> date::parse("%F %T", last);
    if (in.fail()) return false;
    auto now = system_clock::now() + hours(7);
    
    auto elapsed = now - last;
    return elapsed.count() >= (interval * 86400.0);
}

void Card::printNextReview(){
    if (interval < 1.0)
        std::cout << "Next review in " << interval * 1440 << " minutes\n";
    else
        std::cout << "Next review in " << interval << " days\n";
}

void Card::calculateGradeTimeIntervals(){
    switch (state){
        case CardState::Learn:
        case CardState::New:
            // Again
            gradeTimeIntervals[Grade::Again] = learningStep[presetLearnStep.at(Grade::Again)];

            // Hard
            gradeTimeIntervals[Grade::Hard] = learningStep[presetLearnStep.at(Grade::Hard)];

            // Good
            if (currentStep > 1) gradeTimeIntervals[Grade::Good] = 1;
            else gradeTimeIntervals[Grade::Good] = learningStep[presetLearnStep.at(Grade::Good)];

            // Easy
            gradeTimeIntervals[Grade::Easy] = 2;
            break;

        case CardState::Review:
            // Again
            gradeTimeIntervals[Grade::Again] = learningStep[presetLapseStep.at(Grade::Again)];
            
            // Hard
            gradeTimeIntervals[Grade::Hard] = interval * mult(Grade::Hard);

            // Good
            gradeTimeIntervals[Grade::Good] = interval * mult(Grade::Good);

            // Easy
            gradeTimeIntervals[Grade::Easy] = interval * mult(Grade::Easy);
            break;

        case CardState::Lapse:
            // Again
            gradeTimeIntervals[Grade::Again] = learningStep[presetLapseStep.at(Grade::Again)];

            // Hard
            gradeTimeIntervals[Grade::Hard] = learningStep[presetLapseStep.at(Grade::Hard)];

            // Good
            gradeTimeIntervals[Grade::Good] = 1;

            // Easy
            gradeTimeIntervals[Grade::Easy] = 4;
            break;

    }
}

void Card::learnHandler(const Grade& grade){
    state = CardState::Learn;
    interval = gradeTimeIntervals[grade];
    switch (grade)
    {
        case Grade::Again:
            currentStep = 0;
            break;
        case Grade::Hard:
            currentStep = 1;
            break;
        case Grade::Good:
            if (currentStep > 1) {
                state = CardState::Review;
                break;
            }
            currentStep = 2;
            break;
        case Grade::Easy:
            interval = 2;
            break;
    }
}

void Card::reviewHandler(const Grade& grade){
    interval = gradeTimeIntervals[grade];
    if (grade == Grade::Again){
        state = CardState::Lapse;
    } else {
        adjustEaseFactor(grade);
    }
}

void Card::lapseHandler(const Grade& grade){
    interval = gradeTimeIntervals[grade];
    switch (grade)
    {
        case Grade::Good:
        case Grade::Easy:
            state = CardState::Review;
            break;
    }
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