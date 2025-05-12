#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
public:
    enum class DifficultyLevel {
        Easy,
        Hard
    };

    explicit Controller() = default;

    [[nodiscard]] DifficultyLevel GetDifficulty() const;

    void SetDifficulty(DifficultyLevel difficulty);

private:
    DifficultyLevel difficulty_{DifficultyLevel::Easy};
};

#endif // CONTROLLER_H
