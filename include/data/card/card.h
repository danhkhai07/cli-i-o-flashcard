#pragma once

#include "card_export.h"

#include "utils/nlohmann/json.hpp"
// #include "../utils/nlohmann/json.hpp"  // pre production

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

/**
 * @brief Possible user grades when reviewing a card.
 */
enum class CARD_SHARED Grade {Again, Hard, Good, Easy};

/**
 * @brief State of the card in the spaced repetition process.
 */
enum class CARD_SHARED CardState {New, Learn, Review, Lapse};

/**
 * @brief Converts a string representation of grade to Grade enum.
 * @param input The grade string ("Again", "Hard", "Good", "Easy").
 * @return Corresponding Grade enum value.
 */
Grade CARD_SHARED inputToGrade(std::string_view input);

/**
 * @brief Converts a string representation of state to CardState enum.
 * @param input The state string ("New", "Learn", "Review", "Lapse").
 * @return Corresponding CardState enum value.
 */
CardState CARD_SHARED inputToState(std::string_view input);

/**
 * @brief Represents a single flashcard with spaced repetition metadata.
 */
struct CARD_SHARED Card {
public:
    /**
     * @brief Constructs a card with default state and initializes grade intervals.
     */
    Card(){
        calculateGradeTimeIntervals();
    }

    /// Front text of the card.
    std::string front;

    /// Back text of the card.
    std::string back;

    /// Time intervals for each possible grade.
    std::unordered_map<Grade, double> gradeTimeIntervals;

    /// Last refresh time as a string (format: "%F %T").
    std::string lastRefresh;

    /**
     * @brief Reviews the card given a grade and updates scheduling data.
     * @param grade The user-assigned grade.
     */
    void review(const Grade& grade);

    /**
     * @brief Reads card data from a JSON object.
     * @param cardView JSON object containing card fields.
     */
    void read(const nlohmann::json& cardView);

    /**
     * @brief Checks if the card is due for review.
     * @return true if due, false otherwise.
     */
    bool due();

private:
    /// Current state of the card.
    CardState state = CardState::New;

    /// Current learning step index.
    int currentStep = 0;

    /// Current ease factor (difficulty multiplier).
    double easeFactor = 2.5;

    /// Current review interval in days (or fraction of day for minutes).
    double interval = 0;

    /// Predefined learning step intervals (in days, fractional for minutes).
    const std::vector<double> learningStep = {1.0/1440, 10.0/1440, 15.0/1440};
    
    /// Interval multiplier based on grade.
    std::function<double(Grade)> mult = [this](Grade grade) {
        if (Grade::Hard == grade) return 1.2;
        if (Grade::Good == grade) return easeFactor;
        return easeFactor * 1.3; // Grade::Easy == grade
    };

    /// Mapping of grades to learning step indexes for learning state.
    static const std::unordered_map<Grade, int> presetLearnStep;

    /// Mapping of grades to learning step indexes for lapse state.
    static const std::unordered_map<Grade, int> presetLapseStep;

    // Handlers

    /**
     * @brief Handles review logic for learning/new states.
     * @param grade The grade assigned by the user.
     */
    void learnHandler(const Grade& grade);

    /**
     * @brief Handles review logic for review state.
     * @param grade The grade assigned by the user.
     */
    void reviewHandler(const Grade& grade);

    /**
     * @brief Handles review logic for lapse state.
     * @param grade The grade assigned by the user.
     */
    void lapseHandler(const Grade& grade);

    // Utilities

    /**
     * @brief Calculates review time intervals for each grade based on current state.
     */
    void calculateGradeTimeIntervals();

    /**
     * @brief Prints grade time intervals (for debugging).
     */
    void printGradeTimeInt();

    /**
     * @brief Prints the time remaining until the next review (for debugging).
     */
    void printNextReview();

    /**
     * @brief Adjusts the ease factor based on the grade.
     * @param grade The grade assigned by the user.
     */
    void adjustEaseFactor(const Grade& grade);

    /**
     * @brief Adjusts the interval value to be integer days if >= 1.
     */
    void adjustInterval();
};