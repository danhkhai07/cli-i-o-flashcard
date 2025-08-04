#pragma once

#include "data/card.h"

#include <string>
#include <iostream>

class Data {
    public:
        void test(){
            Card card;

            std::vector<std::string> testReviews = { "Good",
                "Good", "Good", "Hard", "Good", "Easy", 
                "Good", "Hard", "Again", "Good", "Easy", 
                "Good", "Again", "Good", "Good", "Easy", 
                "Hard", "Good", "Again", "Hard", "Easy" 
            };

            for (const auto& review : testReviews) {
                std::cout << "Review result: " << review << "\n";
                card.review(inputToGrade(review));
            }

            return;
        }
    private:
        
};