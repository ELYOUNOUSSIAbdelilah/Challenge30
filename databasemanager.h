#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static bool connectDatabase();
    static void seedFullQuestions();
    static void createTables();
    static bool updateUserRole(const QString &username, const QString &role);
    static bool deleteUser(const QString &username);
    static QString generateSalt();
    static QString hashPassword(const QString &password, const QString &salt);
    static void insertTestQuestions();
    static bool saveScore(const QString &username,
                          int score,
                          const QString &category,
                          const QString &difficulty);
    static bool addQuestion(const QString &questionText,
                            const QString &optionA,
                            const QString &optionB,
                            const QString &optionC,
                            const QString &optionD,
                            const QString &correctAnswer,
                            const QString &difficulty,
                            const QString &category);

    static bool updateQuestion(int questionId,
                               const QString &questionText,
                               const QString &optionA,
                               const QString &optionB,
                               const QString &optionC,
                               const QString &optionD,
                               const QString &correctAnswer,
                               const QString &difficulty,
                               const QString &category);

    static bool deleteQuestion(int questionId);
    static bool ensurePlayerProgressExists(const QString &username);
    static bool addXp(const QString &username, int xpGained);
    static bool incrementGamesPlayed(const QString &username);
    static bool hasAchievement(const QString &username, const QString &achievementKey);
    static bool unlockAchievement(const QString &username, const QString &achievementKey);
    static int getPlayerLevel(const QString &username);
    static int getPlayerXp(const QString &username);
    static int getGamesPlayed(const QString &username);
    static int getAchievementCount(const QString &username);
    static QStringList getUnlockedAchievements(const QString &username);
    static QString getAchievementUnlockedAt(const QString &username, const QString &achievementKey);


private:
    static QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
