#include "controller.h"

Controller::DifficultyLevel Controller::GetDifficulty() const {
    return difficulty_;
}

void Controller::SetDifficulty(DifficultyLevel difficulty) {
    difficulty_ = difficulty;
}
