#include "databasemanager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QCryptographicHash>
#include <QDebug>

QSqlDatabase DatabaseManager::db = QSqlDatabase();

bool DatabaseManager::connectDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("challenge30.db");
    }

    if (!db.open()) {
        qDebug() << "Database connection failed:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database connected successfully.";
    return true;
}
void DatabaseManager::seedFullQuestions()
{
    QSqlQuery query;

    auto insertQuestion = [&](QString question,
                              QString a, QString b, QString c, QString d,
                              QString correct,
                              QString category,
                              QString difficulty)
    {
        query.prepare("INSERT INTO questions "
                      "(question_text, option_a, option_b, option_c, option_d, correct_answer, category, difficulty) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

        query.addBindValue(question);
        query.addBindValue(a);
        query.addBindValue(b);
        query.addBindValue(c);
        query.addBindValue(d);
        query.addBindValue(correct);
        query.addBindValue(category);
        query.addBindValue(difficulty);

        if (!query.exec()) {
            qDebug() << "Insert error:" << query.lastError().text();
        }
    };

    // =========================
    // EXAMPLE: Science - Easy
    // =========================

    insertQuestion("What planet is known as the Red Planet?",
                   "Earth", "Mars", "Jupiter", "Venus",
                   "Mars", "Science", "Easy");

    insertQuestion("What gas do plants absorb?",
                   "Oxygen", "Nitrogen", "Carbon Dioxide", "Hydrogen",
                   "Carbon Dioxide", "Science", "Easy");

}
void DatabaseManager::createTables()
{
    QSqlQuery query;

    QString createUsersTable = R"(
       CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    salt TEXT NOT NULL,
    role TEXT NOT NULL DEFAULT 'user'
)

    )";

    if (!query.exec(createUsersTable)) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
    } else {
        qDebug() << "Users table ready.";
    }

    QString createQuestionsTable = R"(
        CREATE TABLE IF NOT EXISTS questions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            question_text TEXT NOT NULL,
            option_a TEXT NOT NULL,
            option_b TEXT NOT NULL,
            option_c TEXT NOT NULL,
            option_d TEXT NOT NULL,
            correct_answer TEXT NOT NULL,
            difficulty TEXT NOT NULL,
            category TEXT NOT NULL
        )
    )";

    if (!query.exec(createQuestionsTable)) {
        qDebug() << "Failed to create questions table:" << query.lastError().text();
    } else {
        qDebug() << "Questions table ready.";
    }

    QString createScoresTable = R"(
    CREATE TABLE IF NOT EXISTS scores (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL,
        score INTEGER NOT NULL,
        category TEXT NOT NULL,
        difficulty TEXT NOT NULL,
        played_at TEXT NOT NULL
    )
)";
    QString createPlayerProgressTable = R"(
    CREATE TABLE IF NOT EXISTS player_progress (
        username TEXT PRIMARY KEY,
        xp INTEGER NOT NULL DEFAULT 0,
        level INTEGER NOT NULL DEFAULT 1,
        games_played INTEGER NOT NULL DEFAULT 0
    )
)";

    if (!query.exec(createPlayerProgressTable)) {
        qDebug() << "Failed to create player_progress table:" << query.lastError().text();
    } else {
        qDebug() << "Player progress table ready.";
    }

    QString createUserAchievementsTable = R"(
    CREATE TABLE IF NOT EXISTS user_achievements (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL,
        achievement_key TEXT NOT NULL,
        unlocked_at TEXT NOT NULL,
        UNIQUE(username, achievement_key)
    )
)";

    if (!query.exec(createUserAchievementsTable)) {
        qDebug() << "Failed to create user_achievements table:" << query.lastError().text();
    } else {
        qDebug() << "User achievements table ready.";
    }


    if (!query.exec(createScoresTable)) {
        qDebug() << "Failed to create scores table:" << query.lastError().text();
    } else {
        qDebug() << "Scores table ready.";
    }
}


void DatabaseManager::insertTestQuestions()
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM questions");

    if (!checkQuery.exec()) {
        qDebug() << "Failed to check questions table:" << checkQuery.lastError().text();
        return;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "Questions already exist. Skipping seed.";
        return;
    }

    QSqlQuery query;

    auto insertQuestion = [&](const QString &q,
                              const QString &a,
                              const QString &b,
                              const QString &c,
                              const QString &d,
                              const QString &correct,
                              const QString &difficulty,
                              const QString &category)
    {
        query.prepare("INSERT INTO questions "
                      "(question_text, option_a, option_b, option_c, option_d, correct_answer, difficulty, category) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

        query.addBindValue(q);
        query.addBindValue(a);
        query.addBindValue(b);
        query.addBindValue(c);
        query.addBindValue(d);
        query.addBindValue(correct);   // A / B / C / D
        query.addBindValue(difficulty);
        query.addBindValue(category);

        if (!query.exec()) {
            qDebug() << "Insert question failed:" << query.lastError().text();
            qDebug() << q;
        }
    };

    // =========================================================
    // GENERAL KNOWLEDGE - EASY (15)
    // =========================================================
    insertQuestion("What is the capital of France?",
                   "Berlin", "Madrid", "Paris", "Rome",
                   "C", "Easy", "General Knowledge");

    insertQuestion("How many continents are there?",
                   "5", "6", "7", "8",
                   "C", "Easy", "General Knowledge");

    insertQuestion("Which color is the sky on a clear day?",
                   "Blue", "Green", "Red", "Black",
                   "A", "Easy", "General Knowledge");

    insertQuestion("How many days are there in a week?",
                   "5", "6", "7", "8",
                   "C", "Easy", "General Knowledge");

    insertQuestion("Which animal barks?",
                   "Cat", "Dog", "Cow", "Horse",
                   "B", "Easy", "General Knowledge");

    insertQuestion("Which is a fruit?",
                   "Potato", "Carrot", "Apple", "Onion",
                   "C", "Easy", "General Knowledge");

    insertQuestion("What is 5 + 3?",
                   "6", "7", "8", "9",
                   "C", "Easy", "General Knowledge");

    insertQuestion("Which ocean is the largest?",
                   "Atlantic", "Indian", "Pacific", "Arctic",
                   "C", "Easy", "General Knowledge");

    insertQuestion("What do bees make?",
                   "Milk", "Honey", "Water", "Bread",
                   "B", "Easy", "General Knowledge");

    insertQuestion("Which planet do we live on?",
                   "Mars", "Earth", "Venus", "Jupiter",
                   "B", "Easy", "General Knowledge");

    insertQuestion("How many legs does a spider have?",
                   "6", "7", "8", "10",
                   "C", "Easy", "General Knowledge");

    insertQuestion("What is the capital of Spain?",
                   "Barcelona", "Madrid", "Valencia", "Seville",
                   "B", "Easy", "General Knowledge");

    insertQuestion("Which month comes after March?",
                   "February", "April", "May", "June",
                   "B", "Easy", "General Knowledge");

    insertQuestion("Which is a primary color?",
                   "Green", "Purple", "Red", "Pink",
                   "C", "Easy", "General Knowledge");

    insertQuestion("How many hours are there in one day?",
                   "12", "18", "24", "48",
                   "C", "Easy", "General Knowledge");

    // =========================================================
    // GENERAL KNOWLEDGE - MEDIUM (15)
    // =========================================================
    insertQuestion("Which country is known as the Land of the Rising Sun?",
                   "China", "Japan", "Thailand", "Korea",
                   "B", "Medium", "General Knowledge");

    insertQuestion("What is the currency of the United Kingdom?",
                   "Euro", "Dollar", "Pound Sterling", "Yen",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Which is the smallest prime number?",
                   "0", "1", "2", "3",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Which metal is liquid at room temperature?",
                   "Iron", "Mercury", "Gold", "Silver",
                   "B", "Medium", "General Knowledge");

    insertQuestion("In which year are leap years repeated?",
                   "Every 2 years", "Every 3 years", "Every 4 years", "Every 5 years",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Which language has the most native speakers worldwide?",
                   "English", "Spanish", "Hindi", "Mandarin Chinese",
                   "D", "Medium", "General Knowledge");

    insertQuestion("How many sides does a hexagon have?",
                   "5", "6", "7", "8",
                   "B", "Medium", "General Knowledge");

    insertQuestion("What is the largest desert in the world?",
                   "Sahara", "Gobi", "Antarctic Desert", "Arabian Desert",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Which organ pumps blood through the body?",
                   "Liver", "Lung", "Heart", "Kidney",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Who painted the Mona Lisa?",
                   "Van Gogh", "Picasso", "Leonardo da Vinci", "Michelangelo",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Which country gifted the Statue of Liberty to the USA?",
                   "France", "Italy", "Germany", "Spain",
                   "A", "Medium", "General Knowledge");

    insertQuestion("Which instrument has 88 keys?",
                   "Violin", "Piano", "Flute", "Drum",
                   "B", "Medium", "General Knowledge");

    insertQuestion("What is the freezing point of water in Celsius?",
                   "0", "10", "32", "100",
                   "A", "Medium", "General Knowledge");

    insertQuestion("Which sea creature has three hearts?",
                   "Shark", "Whale", "Octopus", "Dolphin",
                   "C", "Medium", "General Knowledge");

    insertQuestion("Which country has the most pyramids?",
                   "Egypt", "Sudan", "Mexico", "Peru",
                   "B", "Medium", "General Knowledge");

    // =========================================================
    // GENERAL KNOWLEDGE - HARD (15)
    // =========================================================
    insertQuestion("What is the longest river in the world according to many modern sources?",
                   "Amazon", "Nile", "Yangtze", "Mississippi",
                   "A", "Hard", "General Knowledge");

    insertQuestion("Which country has the most time zones?",
                   "USA", "Russia", "France", "China",
                   "C", "Hard", "General Knowledge");

    insertQuestion("What is the only mammal capable of true flight?",
                   "Flying squirrel", "Bat", "Penguin", "Ostrich",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Which chemical element has the symbol W?",
                   "Tungsten", "Tin", "Titanium", "Tantalum",
                   "A", "Hard", "General Knowledge");

    insertQuestion("What is the capital of Kazakhstan?",
                   "Almaty", "Astana", "Tashkent", "Bishkek",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Which year did the Berlin Wall fall?",
                   "1987", "1988", "1989", "1990",
                   "C", "Hard", "General Knowledge");

    insertQuestion("How many bones are in an adult human body?",
                   "198", "206", "214", "220",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Which planet has the most moons currently known?",
                   "Jupiter", "Saturn", "Uranus", "Neptune",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Who wrote the novel '1984'?",
                   "George Orwell", "Aldous Huxley", "J.K. Rowling", "Mark Twain",
                   "A", "Hard", "General Knowledge");

    insertQuestion("Which country is home to Mount Kilimanjaro?",
                   "Kenya", "Uganda", "Tanzania", "Ethiopia",
                   "C", "Hard", "General Knowledge");

    insertQuestion("What is the rarest blood type among the major ABO/Rh groups?",
                   "O+", "AB-", "A+", "B-",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Which city hosted the 2016 Summer Olympics?",
                   "Tokyo", "Rio de Janeiro", "London", "Beijing",
                   "B", "Hard", "General Knowledge");

    insertQuestion("What is the largest internal organ in the human body?",
                   "Brain", "Liver", "Lung", "Kidney",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Which scientist proposed the three laws of motion?",
                   "Einstein", "Newton", "Galileo", "Tesla",
                   "B", "Hard", "General Knowledge");

    insertQuestion("Which strait separates Asia from North America?",
                   "Gibraltar", "Bosporus", "Bering Strait", "Hormuz",
                   "C", "Hard", "General Knowledge");

    // =========================================================
    // SCIENCE - EASY (15)
    // =========================================================
    insertQuestion("What planet is known as the Red Planet?",
                   "Earth", "Mars", "Jupiter", "Venus",
                   "B", "Easy", "Science");

    insertQuestion("What gas do plants absorb from the air?",
                   "Oxygen", "Nitrogen", "Carbon Dioxide", "Hydrogen",
                   "C", "Easy", "Science");

    insertQuestion("What is H2O commonly known as?",
                   "Salt", "Oxygen", "Water", "Hydrogen",
                   "C", "Easy", "Science");

    insertQuestion("What force pulls objects toward Earth?",
                   "Magnetism", "Gravity", "Light", "Speed",
                   "B", "Easy", "Science");

    insertQuestion("How many planets are in the solar system?",
                   "7", "8", "9", "10",
                   "B", "Easy", "Science");

    insertQuestion("Which organ helps humans breathe?",
                   "Heart", "Liver", "Lungs", "Brain",
                   "C", "Easy", "Science");

    insertQuestion("What do we call animals that eat only plants?",
                   "Carnivores", "Herbivores", "Omnivores", "Insects",
                   "B", "Easy", "Science");

    insertQuestion("Which star gives Earth light and heat?",
                   "Moon", "Mars", "Sun", "Venus",
                   "C", "Easy", "Science");

    insertQuestion("Which body part is used for seeing?",
                   "Ear", "Nose", "Eye", "Hand",
                   "C", "Easy", "Science");

    insertQuestion("Which state of matter has a fixed shape?",
                   "Gas", "Liquid", "Solid", "Steam",
                   "C", "Easy", "Science");

    insertQuestion("What is the boiling point of water in Celsius?",
                   "0", "50", "100", "200",
                   "C", "Easy", "Science");

    insertQuestion("What vitamin do we get from sunlight?",
                   "Vitamin A", "Vitamin B", "Vitamin C", "Vitamin D",
                   "D", "Easy", "Science");

    insertQuestion("Which blood cells help fight infection?",
                   "Red blood cells", "White blood cells", "Platelets", "Plasma",
                   "B", "Easy", "Science");

    insertQuestion("What is the center of an atom called?",
                   "Electron", "Nucleus", "Proton", "Shell",
                   "B", "Easy", "Science");

    insertQuestion("Which planet is closest to the Sun?",
                   "Mercury", "Venus", "Earth", "Mars",
                   "A", "Easy", "Science");

    // =========================================================
    // SCIENCE - MEDIUM (15)
    // =========================================================
    insertQuestion("What part of the cell contains genetic material?",
                   "Membrane", "Nucleus", "Cytoplasm", "Ribosome",
                   "B", "Medium", "Science");

    insertQuestion("Which gas is most abundant in Earth's atmosphere?",
                   "Oxygen", "Carbon Dioxide", "Nitrogen", "Hydrogen",
                   "C", "Medium", "Science");

    insertQuestion("What is the chemical symbol for sodium?",
                   "So", "Na", "S", "Sd",
                   "B", "Medium", "Science");

    insertQuestion("Which planet is famous for its rings?",
                   "Mars", "Jupiter", "Saturn", "Neptune",
                   "C", "Medium", "Science");

    insertQuestion("What do you call a change from liquid to gas?",
                   "Freezing", "Condensation", "Evaporation", "Melting",
                   "C", "Medium", "Science");

    insertQuestion("Which scientist developed the theory of evolution by natural selection?",
                   "Newton", "Darwin", "Einstein", "Galileo",
                   "B", "Medium", "Science");

    insertQuestion("What is the powerhouse of the cell?",
                   "Nucleus", "Mitochondria", "Ribosome", "Vacuole",
                   "B", "Medium", "Science");

    insertQuestion("Which type of energy is stored in food?",
                   "Kinetic", "Chemical", "Sound", "Light",
                   "B", "Medium", "Science");

    insertQuestion("What is measured with a seismometer?",
                   "Wind", "Rain", "Earthquakes", "Temperature",
                   "C", "Medium", "Science");

    insertQuestion("How many chambers does the human heart have?",
                   "2", "3", "4", "5",
                   "C", "Medium", "Science");

    insertQuestion("What is the hardest natural substance on Earth?",
                   "Iron", "Diamond", "Gold", "Quartz",
                   "B", "Medium", "Science");

    insertQuestion("Which organ filters blood in the human body?",
                   "Heart", "Lungs", "Kidneys", "Pancreas",
                   "C", "Medium", "Science");

    insertQuestion("What is the process by which plants make food?",
                   "Respiration", "Photosynthesis", "Digestion", "Fermentation",
                   "B", "Medium", "Science");

    insertQuestion("Which blood type is known as the universal donor?",
                   "AB+", "O-", "A+", "B+",
                   "B", "Medium", "Science");

    insertQuestion("What does DNA stand for?",
                   "Dynamic Nucleic Acid", "Deoxyribonucleic Acid", "Double Nitrogen Atom", "Digital Nucleic Array",
                   "B", "Medium", "Science");

    // =========================================================
    // SCIENCE - HARD (15)
    // =========================================================
    insertQuestion("Which particle has a negative charge?",
                   "Proton", "Neutron", "Electron", "Photon",
                   "C", "Hard", "Science");

    insertQuestion("What is the SI unit of force?",
                   "Watt", "Pascal", "Newton", "Joule",
                   "C", "Hard", "Science");

    insertQuestion("Which branch of science studies fossils?",
                   "Ecology", "Genetics", "Paleontology", "Meteorology",
                   "C", "Hard", "Science");

    insertQuestion("What is the approximate speed of light in vacuum?",
                   "300,000 km/s", "150,000 km/s", "30,000 km/s", "3,000 km/s",
                   "A", "Hard", "Science");

    insertQuestion("Which law explains the relationship between pressure and volume of a gas at constant temperature?",
                   "Newton's Law", "Boyle's Law", "Ohm's Law", "Kepler's Law",
                   "B", "Hard", "Science");

    insertQuestion("What is the pH of a neutral solution?",
                   "0", "5", "7", "14",
                   "C", "Hard", "Science");

    insertQuestion("Which organelle is responsible for protein synthesis?",
                   "Golgi apparatus", "Ribosome", "Chloroplast", "Lysosome",
                   "B", "Hard", "Science");

    insertQuestion("Which scientist is known for the uncertainty principle?",
                   "Bohr", "Heisenberg", "Planck", "Faraday",
                   "B", "Hard", "Science");

    insertQuestion("Which planet has the shortest year?",
                   "Mercury", "Venus", "Mars", "Jupiter",
                   "A", "Hard", "Science");

    insertQuestion("What is the atomic number of carbon?",
                   "4", "6", "8", "12",
                   "B", "Hard", "Science");

    insertQuestion("What type of bond involves sharing electrons?",
                   "Ionic", "Covalent", "Metallic", "Hydrogen",
                   "B", "Hard", "Science");

    insertQuestion("Which blood vessel carries oxygenated blood from the lungs to the heart?",
                   "Aorta", "Pulmonary vein", "Pulmonary artery", "Vena cava",
                   "B", "Hard", "Science");

    insertQuestion("What is the name of the galaxy containing our solar system?",
                   "Andromeda", "Milky Way", "Whirlpool", "Orion",
                   "B", "Hard", "Science");

    insertQuestion("Which scientist discovered penicillin?",
                   "Pasteur", "Fleming", "Curie", "Hooke",
                   "B", "Hard", "Science");

    insertQuestion("What is the smallest unit of life?",
                   "Atom", "Cell", "Molecule", "Tissue",
                   "B", "Hard", "Science");

    // =========================================================
    // HISTORY - EASY (15)
    // =========================================================
    insertQuestion("Who was the first President of the United States?",
                   "George Washington", "Abraham Lincoln", "Thomas Jefferson", "John Adams",
                   "A", "Easy", "History");

    insertQuestion("In which country were the pyramids built?",
                   "Mexico", "Egypt", "Iraq", "India",
                   "B", "Easy", "History");

    insertQuestion("Who discovered America in 1492?",
                   "Christopher Columbus", "Marco Polo", "Magellan", "Da Gama",
                   "A", "Easy", "History");

    insertQuestion("Which wall divided Berlin until 1989?",
                   "Great Wall", "Berlin Wall", "Roman Wall", "Iron Wall",
                   "B", "Easy", "History");

    insertQuestion("Who was known as the Maid of Orleans?",
                   "Cleopatra", "Joan of Arc", "Queen Victoria", "Marie Curie",
                   "B", "Easy", "History");

    insertQuestion("Which ship famously sank in 1912?",
                   "Titanic", "Britannic", "Mayflower", "Victory",
                   "A", "Easy", "History");

    insertQuestion("Which civilization built the Colosseum?",
                   "Greek", "Roman", "Egyptian", "Persian",
                   "B", "Easy", "History");

    insertQuestion("Who was the leader of Nazi Germany during World War II?",
                   "Hitler", "Stalin", "Churchill", "Roosevelt",
                   "A", "Easy", "History");

    insertQuestion("What war was fought between the North and South regions in the United States?",
                   "World War I", "Civil War", "Cold War", "Crimean War",
                   "B", "Easy", "History");

    insertQuestion("Which country gave the Statue of Liberty to the USA?",
                   "France", "Spain", "Italy", "Germany",
                   "A", "Easy", "History");

    insertQuestion("Who was the famous queen of ancient Egypt?",
                   "Victoria", "Cleopatra", "Elizabeth", "Mary",
                   "B", "Easy", "History");

    insertQuestion("What was the name of the trade route linking Europe and Asia?",
                   "Silk Road", "Salt Road", "King's Way", "Spice Trail",
                   "A", "Easy", "History");

    insertQuestion("Which war ended in 1945?",
                   "World War I", "World War II", "Cold War", "Vietnam War",
                   "B", "Easy", "History");

    insertQuestion("Who was assassinated in 1963 in Dallas, Texas?",
                   "JFK", "Lincoln", "Martin Luther King Jr.", "Nixon",
                   "A", "Easy", "History");

    insertQuestion("Which empire was ruled by Julius Caesar?",
                   "Ottoman Empire", "Roman Republic", "Mongol Empire", "British Empire",
                   "B", "Easy", "History");

    // =========================================================
    // HISTORY - MEDIUM (15)
    // =========================================================
    insertQuestion("In which year did World War I begin?",
                   "1912", "1914", "1916", "1918",
                   "B", "Medium", "History");

    insertQuestion("Who was the British Prime Minister during most of World War II?",
                   "Churchill", "Blair", "Thatcher", "Chamberlain",
                   "A", "Medium", "History");

    insertQuestion("Which empire fell in 1453 with the capture of Constantinople?",
                   "Roman Empire", "Byzantine Empire", "Mughal Empire", "Persian Empire",
                   "B", "Medium", "History");

    insertQuestion("Who wrote the Communist Manifesto with Friedrich Engels?",
                   "Stalin", "Lenin", "Karl Marx", "Trotsky",
                   "C", "Medium", "History");

    insertQuestion("What was the name of the period of intellectual revival in Europe from the 14th to 17th century?",
                   "Reformation", "Renaissance", "Enlightenment", "Industrial Revolution",
                   "B", "Medium", "History");

    insertQuestion("Which country was ruled by Tsars before the 1917 revolution?",
                   "France", "Russia", "Germany", "Austria",
                   "B", "Medium", "History");

    insertQuestion("Who was the first emperor of Rome?",
                   "Julius Caesar", "Augustus", "Nero", "Constantine",
                   "B", "Medium", "History");

    insertQuestion("What document signed in 1215 limited the powers of the English king?",
                   "Bill of Rights", "Magna Carta", "Treaty of Versailles", "Declaration of Rights",
                   "B", "Medium", "History");

    insertQuestion("Who led the Indian independence movement using nonviolent resistance?",
                   "Nehru", "Gandhi", "Patel", "Bose",
                   "B", "Medium", "History");

    insertQuestion("Which battle in 1815 marked Napoleon's final defeat?",
                   "Trafalgar", "Leipzig", "Austerlitz", "Waterloo",
                   "D", "Medium", "History");

    insertQuestion("Which dynasty built much of the Great Wall of China as seen today?",
                   "Qin", "Han", "Ming", "Tang",
                   "C", "Medium", "History");

    insertQuestion("Which event began on July 14, 1789 in France?",
                   "French Revolution", "Industrial Revolution", "Cold War", "American Civil War",
                   "A", "Medium", "History");

    insertQuestion("What was the name of the ship on which the Pilgrims traveled to America in 1620?",
                   "Victoria", "Mayflower", "Titanic", "Santa Maria",
                   "B", "Medium", "History");

    insertQuestion("Who was the South African leader imprisoned for 27 years and later became president?",
                   "Mandela", "Tutu", "Botha", "Mbeki",
                   "A", "Medium", "History");

    insertQuestion("Which city was once called Constantinople?",
                   "Athens", "Istanbul", "Rome", "Cairo",
                   "B", "Medium", "History");

    // =========================================================
    // HISTORY - HARD (15)
    // =========================================================
    insertQuestion("Which treaty ended World War I in 1919?",
                   "Treaty of Paris", "Treaty of Versailles", "Treaty of Utrecht", "Treaty of Vienna",
                   "B", "Hard", "History");

    insertQuestion("Who was the Carthaginian general who crossed the Alps with elephants?",
                   "Hannibal", "Scipio", "Alexander", "Caesar",
                   "A", "Hard", "History");

    insertQuestion("Which Chinese philosopher is associated with teachings on ethics and government?",
                   "Sun Tzu", "Laozi", "Confucius", "Mencius",
                   "C", "Hard", "History");

    insertQuestion("The Hundred Years' War was primarily fought between which two countries?",
                   "England and France", "Spain and Portugal", "Germany and Italy", "Russia and Poland",
                   "A", "Hard", "History");

    insertQuestion("Which civilization developed cuneiform writing?",
                   "Romans", "Sumerians", "Mayans", "Phoenicians",
                   "B", "Hard", "History");

    insertQuestion("Who was the last Tsar of Russia?",
                   "Peter I", "Nicholas II", "Alexander III", "Ivan IV",
                   "B", "Hard", "History");

    insertQuestion("Which battle in 1066 changed English history dramatically?",
                   "Agincourt", "Hastings", "Tours", "Crecy",
                   "B", "Hard", "History");

    insertQuestion("What was the main port city of the ancient Phoenicians in modern-day Lebanon?",
                   "Athens", "Babylon", "Tyre", "Memphis",
                   "C", "Hard", "History");

    insertQuestion("Who was the first woman to rule Egypt as pharaoh in her own right for a long period?",
                   "Nefertiti", "Hatshepsut", "Cleopatra", "Merneith",
                   "B", "Hard", "History");

    insertQuestion("Which empire was ruled by Suleiman the Magnificent?",
                   "Roman Empire", "Ottoman Empire", "Mongol Empire", "Persian Empire",
                   "B", "Hard", "History");

    insertQuestion("Which reformer famously posted the Ninety-five Theses in 1517?",
                   "John Calvin", "Martin Luther", "Erasmus", "Zwingli",
                   "B", "Hard", "History");

    insertQuestion("What was the capital of the Byzantine Empire?",
                   "Rome", "Constantinople", "Athens", "Alexandria",
                   "B", "Hard", "History");

    insertQuestion("Which ancient wonder was located in Babylon?",
                   "Colossus of Rhodes", "Hanging Gardens", "Temple of Artemis", "Lighthouse of Alexandria",
                   "B", "Hard", "History");

    insertQuestion("Who was the Mongol ruler who founded the largest contiguous land empire in history?",
                   "Kublai Khan", "Genghis Khan", "Tamerlane", "Attila",
                   "B", "Hard", "History");

    insertQuestion("Which conference in 1884–1885 regulated European colonization in Africa?",
                   "Yalta Conference", "Berlin Conference", "Vienna Congress", "Paris Peace Conference",
                   "B", "Hard", "History");

    // =========================================================
    // FOOTBALL - EASY (15)
    // =========================================================
    insertQuestion("How many players are on the field for one football team?",
                   "9", "10", "11", "12",
                   "C", "Easy", "Football");

    insertQuestion("How long is a regular football match?",
                   "80 minutes", "90 minutes", "100 minutes", "120 minutes",
                   "B", "Easy", "Football");

    insertQuestion("What card sends a player off?",
                   "Blue", "Green", "Yellow", "Red",
                   "D", "Easy", "Football");

    insertQuestion("Who protects the goal?",
                   "Defender", "Striker", "Goalkeeper", "Coach",
                   "C", "Easy", "Football");

    insertQuestion("What is given for a foul inside the penalty area?",
                   "Throw-in", "Penalty kick", "Corner kick", "Offside",
                   "B", "Easy", "Football");

    insertQuestion("Which country won the FIFA World Cup in 2022?",
                   "France", "Brazil", "Argentina", "Germany",
                   "C", "Easy", "Football");

    insertQuestion("What part of the body do field players generally not use?",
                   "Head", "Chest", "Foot", "Hand",
                   "D", "Easy", "Football");

    insertQuestion("What is it called when both teams have the same score?",
                   "Penalty", "Draw", "Offside", "Corner",
                   "B", "Easy", "Football");

    insertQuestion("How many halves are in a football match?",
                   "2", "3", "4", "1",
                   "A", "Easy", "Football");

    insertQuestion("Which player is famous for playing for Argentina and Barcelona?",
                   "Zidane", "Messi", "Ronaldo Nazario", "Salah",
                   "B", "Easy", "Football");

    insertQuestion("What restarts play when the ball goes out over the sideline?",
                   "Goal kick", "Throw-in", "Penalty", "Drop ball",
                   "B", "Easy", "Football");

    insertQuestion("Which competition is for national teams?",
                   "Champions League", "Premier League", "World Cup", "La Liga",
                   "C", "Easy", "Football");

    insertQuestion("How many points is a win worth in league football?",
                   "1", "2", "3", "4",
                   "C", "Easy", "Football");

    insertQuestion("What is the area called where the goalkeeper can use hands?",
                   "Center circle", "Penalty area", "Corner arc", "Technical area",
                   "B", "Easy", "Football");

    insertQuestion("Who starts and controls the match officially?",
                   "Captain", "Coach", "Referee", "Assistant manager",
                   "C", "Easy", "Football");

    // =========================================================
    // FOOTBALL - MEDIUM (15)
    // =========================================================
    insertQuestion("Which club has won the most UEFA Champions League titles?",
                   "Barcelona", "Liverpool", "Real Madrid", "Bayern Munich",
                   "C", "Medium", "Football");

    insertQuestion("Which country hosted the 2018 FIFA World Cup?",
                   "Brazil", "Russia", "Qatar", "Germany",
                   "B", "Medium", "Football");

    insertQuestion("What is awarded when the defending team last touches the ball over its own goal line, without a goal being scored?",
                   "Throw-in", "Penalty", "Corner kick", "Indirect free kick",
                   "C", "Medium", "Football");

    insertQuestion("Which player is nicknamed CR7?",
                   "Kylian Mbappe", "Cristiano Ronaldo", "Karim Benzema", "Ronaldinho",
                   "B", "Medium", "Football");

    insertQuestion("Which country has won the most FIFA World Cups?",
                   "Germany", "Italy", "Brazil", "Argentina",
                   "C", "Medium", "Football");

    insertQuestion("How many players are allowed in a standard starting lineup?",
                   "10", "11", "12", "13",
                   "B", "Medium", "Football");

    insertQuestion("Which English club is known as the Red Devils?",
                   "Liverpool", "Arsenal", "Manchester United", "Chelsea",
                   "C", "Medium", "Football");

    insertQuestion("What does VAR stand for?",
                   "Video Assistant Referee", "Visual Action Replay", "Verified Assistant Review", "Video Action Referee",
                   "A", "Medium", "Football");

    insertQuestion("Which footballer won the 2023 Ballon d'Or?",
                   "Haaland", "Mbappe", "Messi", "Benzema",
                   "C", "Medium", "Football");

    insertQuestion("Which position mainly scores goals?",
                   "Goalkeeper", "Striker", "Referee", "Full-back",
                   "B", "Medium", "Football");

    insertQuestion("Which tournament is played by clubs from Europe?",
                   "Copa America", "AFCON", "UEFA Champions League", "World Cup",
                   "C", "Medium", "Football");

    insertQuestion("How many minutes is extra time in a knockout match, not counting stoppage time?",
                   "20", "25", "30", "40",
                   "C", "Medium", "Football");

    insertQuestion("Which country won Euro 2016?",
                   "France", "Portugal", "Spain", "Italy",
                   "B", "Medium", "Football");

    insertQuestion("Which North African player is famous for playing for Liverpool and Morocco?",
                   "Mahrez", "Hakimi", "Salah", "Ziyech",
                   "C", "Medium", "Football");

    insertQuestion("What is given if a player is in an offside position and becomes involved in play?",
                   "Corner", "Penalty", "Offside free kick", "Throw-in",
                   "C", "Medium", "Football");

    // =========================================================
    // FOOTBALL - HARD (15)
    // =========================================================
    insertQuestion("Which country won the first FIFA World Cup in 1930?",
                   "Brazil", "Uruguay", "Italy", "Argentina",
                   "B", "Hard", "Football");

    insertQuestion("Which club is known as 'The Old Lady'?",
                   "AC Milan", "Inter Milan", "Juventus", "Roma",
                   "C", "Hard", "Football");

    insertQuestion("Who scored the infamous 'Hand of God' goal in 1986?",
                   "Pele", "Maradona", "Messi", "Romario",
                   "B", "Hard", "Football");

    insertQuestion("Which manager led Manchester City to a historic treble in 2023?",
                   "Klopp", "Guardiola", "Ancelotti", "Mourinho",
                   "B", "Hard", "Football");

    insertQuestion("Which country hosted the 2010 FIFA World Cup?",
                   "South Africa", "Germany", "Japan", "Brazil",
                   "A", "Hard", "Football");

    insertQuestion("Which player has won the most Ballon d'Or awards as of recent years?",
                   "Cristiano Ronaldo", "Messi", "Cruyff", "Platini",
                   "B", "Hard", "Football");

    insertQuestion("Which club won the UEFA Champions League in 2024?",
                   "Borussia Dortmund", "Manchester City", "Real Madrid", "Bayern Munich",
                   "C", "Hard", "Football");

    insertQuestion("What is the maximum number of yellow cards that results in a red card within a single match?",
                   "1", "2", "3", "4",
                   "B", "Hard", "Football");

    insertQuestion("Which African nation won the Africa Cup of Nations in 2024?",
                   "Morocco", "Egypt", "Nigeria", "Cote d'Ivoire",
                   "D", "Hard", "Football");

    insertQuestion("Which player is famous for the phrase 'I am not a galactico, I am Zidane'?",
                   "Ronaldo", "Zidane", "Figo", "Raul",
                   "B", "Hard", "Football");

    insertQuestion("Which club won the first Premier League season in 1992-93?",
                   "Arsenal", "Manchester United", "Liverpool", "Blackburn",
                   "B", "Hard", "Football");

    insertQuestion("Which goalkeeper won the Yashin Trophy in 2023?",
                   "Ederson", "Courtois", "Martinez", "Ter Stegen",
                   "C", "Hard", "Football");

    insertQuestion("Which country did Zinedine Zidane represent internationally?",
                   "Algeria", "France", "Morocco", "Spain",
                   "B", "Hard", "Football");

    insertQuestion("What is the name of Barcelona's stadium before its redevelopment project?",
                   "Bernabeu", "Camp Nou", "Wembley", "San Siro",
                   "B", "Hard", "Football");

    insertQuestion("Which striker broke the Premier League single-season scoring record in 2022-23?",
                   "Kane", "Haaland", "Salah", "Suarez",
                   "B", "Hard", "Football");

    // =========================================================
    // ISLAMIC - EASY (15)
    // =========================================================
    insertQuestion("How many pillars are there in Islam?",
                   "4", "5", "6", "7",
                   "B", "Easy", "Islamic");

    insertQuestion("What is the holy book of Islam?",
                   "Bible", "Torah", "Quran", "Gospel",
                   "C", "Easy", "Islamic");

    insertQuestion("Who is the final Prophet in Islam?",
                   "Musa", "Isa", "Muhammad (PBUH)", "Ibrahim",
                   "C", "Easy", "Islamic");

    insertQuestion("What is the name of the month in which Muslims fast?",
                   "Rajab", "Ramadan", "Muharram", "Shawwal",
                   "B", "Easy", "Islamic");

    insertQuestion("How many daily prayers do Muslims perform?",
                   "3", "4", "5", "6",
                   "C", "Easy", "Islamic");

    insertQuestion("Where is the Kaaba located?",
                   "Jerusalem", "Makkah", "Madinah", "Cairo",
                   "B", "Easy", "Islamic");

    insertQuestion("What is the first pillar of Islam?",
                   "Zakat", "Salah", "Shahada", "Hajj",
                   "C", "Easy", "Islamic");

    insertQuestion("What is the name of charity in Islam?",
                   "Sawm", "Zakat", "Hajj", "Dua",
                   "B", "Easy", "Islamic");

    insertQuestion("What is the pilgrimage to Makkah called?",
                   "Umrah", "Hajj", "Hijrah", "Ziyarah",
                   "B", "Easy", "Islamic");

    insertQuestion("What greeting do Muslims often use?",
                   "Hello", "As-salamu alaykum", "Good evening", "Welcome",
                   "B", "Easy", "Islamic");

    insertQuestion("Which angel brought revelation to Prophet Muhammad (PBUH)?",
                   "Mikail", "Israfil", "Jibril", "Malik",
                   "C", "Easy", "Islamic");

    insertQuestion("What direction do Muslims face when praying?",
                   "East", "West", "Qibla", "North",
                   "C", "Easy", "Islamic");

    insertQuestion("On which day is Jumu'ah prayer performed?",
                   "Monday", "Friday", "Saturday", "Sunday",
                   "B", "Easy", "Islamic");

    insertQuestion("How many surahs are in the Quran?",
                   "99", "100", "114", "120",
                   "C", "Easy", "Islamic");

    insertQuestion("Who was the first caliph after Prophet Muhammad (PBUH)?",
                   "Umar", "Abu Bakr", "Ali", "Uthman",
                   "B", "Easy", "Islamic");

    // =========================================================
    // ISLAMIC - MEDIUM (15)
    // =========================================================
    insertQuestion("Which surah is the first in the Quran?",
                   "Al-Ikhlas", "Al-Fatihah", "Al-Baqarah", "Yasin",
                   "B", "Medium", "Islamic");

    insertQuestion("How many rak'ahs are in Fajr prayer?",
                   "2", "3", "4", "5",
                   "A", "Medium", "Islamic");

    insertQuestion("What is the night called on which the Quran began to be revealed?",
                   "Laylat al-Qadr", "Isra and Mi'raj", "Ashura", "Arafah",
                   "A", "Medium", "Islamic");

    insertQuestion("What is the Arabic term for fasting?",
                   "Salah", "Zakat", "Sawm", "Hajj",
                   "C", "Medium", "Islamic");

    insertQuestion("Which city did the Prophet (PBUH) migrate to in the Hijrah?",
                   "Taif", "Jerusalem", "Madinah", "Damascus",
                   "C", "Medium", "Islamic");

    insertQuestion("Which battle was the first major battle in Islam?",
                   "Uhud", "Badr", "Khandaq", "Hunayn",
                   "B", "Medium", "Islamic");

    insertQuestion("How many months are there in the Islamic calendar?",
                   "10", "11", "12", "13",
                   "C", "Medium", "Islamic");

    insertQuestion("Which prophet built the Kaaba with his son Ismail?",
                   "Nuh", "Ibrahim", "Yusuf", "Musa",
                   "B", "Medium", "Islamic");

    insertQuestion("What is the Arabic word for mosque?",
                   "Masjid", "Madrasa", "Minbar", "Mihrab",
                   "A", "Medium", "Islamic");

    insertQuestion("What is the Islamic festival after Ramadan called?",
                   "Eid al-Adha", "Eid al-Fitr", "Ashura", "Mawlid",
                   "B", "Medium", "Islamic");

    insertQuestion("Which caliph was known as Al-Farooq?",
                   "Abu Bakr", "Umar ibn Al-Khattab", "Uthman", "Ali",
                   "B", "Medium", "Islamic");

    insertQuestion("What is the waiting period after divorce or widowhood called?",
                   "Wudu", "Iddah", "Sunnah", "Qada",
                   "B", "Medium", "Islamic");

    insertQuestion("What is the Islamic testimony of faith called?",
                   "Shahada", "Sunnah", "Qiyam", "Takbir",
                   "A", "Medium", "Islamic");

    insertQuestion("Which prophet is known for being swallowed by a great fish?",
                   "Yunus", "Yusuf", "Nuh", "Hud",
                   "A", "Medium", "Islamic");

    insertQuestion("What is the Arabic term for obligatory charity on wealth?",
                   "Sadaqah", "Zakat", "Waqf", "Khums",
                   "B", "Medium", "Islamic");

    // =========================================================
    // ISLAMIC - HARD (15)
    // =========================================================
    insertQuestion("Which surah is the longest in the Quran?",
                   "Al-Imran", "Al-Baqarah", "An-Nisa", "Yusuf",
                   "B", "Hard", "Islamic");

    insertQuestion("How many years approximately did the revelation of the Quran take?",
                   "10", "15", "23", "40",
                   "C", "Hard", "Islamic");

    insertQuestion("What is the term for consensus of Islamic scholars?",
                   "Qiyas", "Ijma", "Ijtihad", "Fiqh",
                   "B", "Hard", "Islamic");

    insertQuestion("Which companion was known as the Sword of Allah?",
                   "Ali", "Umar", "Khalid ibn al-Walid", "Bilal",
                   "C", "Hard", "Islamic");

    insertQuestion("What is the name of the treaty signed between the Muslims and Quraysh that led to peaceful relations before Makkah's conquest?",
                   "Treaty of Taif", "Treaty of Hudaybiyyah", "Treaty of Badr", "Treaty of Uhud",
                   "B", "Hard", "Islamic");

    insertQuestion("What is qiyas in Islamic jurisprudence?",
                   "Consensus", "Analogy", "Tradition", "Charity",
                   "B", "Hard", "Islamic");

    insertQuestion("Which wife of the Prophet (PBUH) was known for narrating many hadith?",
                   "Khadijah", "Aisha", "Hafsa", "Sawdah",
                   "B", "Hard", "Islamic");

    insertQuestion("What is the first month of the Islamic calendar?",
                   "Ramadan", "Muharram", "Safar", "Rajab",
                   "B", "Hard", "Islamic");

    insertQuestion("Which battle is known as the Battle of the Trench?",
                   "Badr", "Uhud", "Khandaq", "Hunayn",
                   "C", "Hard", "Islamic");

    insertQuestion("Who compiled the Quran into one mushaf during the caliphate of Abu Bakr?",
                   "Ali", "Zayd ibn Thabit", "Umar", "Bilal",
                   "B", "Hard", "Islamic");

    insertQuestion("Which prophet is called Kalimullah, the one who spoke directly with Allah?",
                   "Ibrahim", "Isa", "Musa", "Dawud",
                   "C", "Hard", "Islamic");

    insertQuestion("What does 'fiqh' refer to?",
                   "Quran recitation", "Islamic jurisprudence", "Pilgrimage", "Theology only",
                   "B", "Hard", "Islamic");

    insertQuestion("Which caliph standardized copies of the Quran and sent them to different regions?",
                   "Abu Bakr", "Umar", "Uthman", "Ali",
                   "C", "Hard", "Islamic");

    insertQuestion("What is the name of the cave where the first revelation occurred?",
                   "Cave of Hira", "Cave of Thawr", "Cave of Uhud", "Cave of Badr",
                   "A", "Hard", "Islamic");

    insertQuestion("What is the term for the chain of narration in hadith studies?",
                   "Matn", "Isnad", "Fiqh", "Qiyas",
                   "B", "Hard", "Islamic");

    // =========================================================
    // PROGRAMMING - EASY (15)
    // =========================================================
    insertQuestion("What does CPU stand for?",
                   "Central Process Unit", "Central Processing Unit", "Computer Personal Unit", "Control Program Unit",
                   "B", "Easy", "Programming");

    insertQuestion("Which language is mainly used to structure web pages?",
                   "Python", "HTML", "C++", "Java",
                   "B", "Easy", "Programming");

    insertQuestion("Which symbol ends a C++ statement?",
                   ".", ",", ";", ":",
                   "C", "Easy", "Programming");

    insertQuestion("Which data type stores whole numbers in C++?",
                   "float", "string", "int", "bool",
                   "C", "Easy", "Programming");

    insertQuestion("Which of these is an output object in C++?",
                   "cin", "cout", "main", "std",
                   "B", "Easy", "Programming");

    insertQuestion("Which keyword is used to define a constant in C++?",
                   "fixed", "final", "const", "static",
                   "C", "Easy", "Programming");

    insertQuestion("What is the correct extension for a C++ source file?",
                   ".cpp", ".html", ".py", ".java",
                   "A", "Easy", "Programming");

    insertQuestion("Which operator compares equality in C++?",
                   "=", "==", "!=", ":=",
                   "B", "Easy", "Programming");

    insertQuestion("Which loop repeats while a condition is true?",
                   "if", "for", "while", "switch",
                   "C", "Easy", "Programming");

    insertQuestion("Which symbols are used for single-line comments in C++?",
                   "/* */", "//", "#", "--",
                   "B", "Easy", "Programming");

    insertQuestion("Which keyword defines a class in C++?",
                   "object", "class", "define", "new",
                   "B", "Easy", "Programming");

    insertQuestion("Which header is commonly used for input and output in C++?",
                   "<vector>", "<iostream>", "<string>", "<cmath>",
                   "B", "Easy", "Programming");

    insertQuestion("What is debugging?",
                   "Writing code", "Finding and fixing errors", "Deleting files", "Designing UI",
                   "B", "Easy", "Programming");

    insertQuestion("Which keyword is used to create an object dynamically in C++?",
                   "make", "create", "malloc", "new",
                   "D", "Easy", "Programming");

    insertQuestion("Which symbol is used to access members of an object?",
                   ".", ":", ";", "#",
                   "A", "Easy", "Programming");

    // =========================================================
    // PROGRAMMING - MEDIUM (15)
    // =========================================================
    insertQuestion("What is the return type of main in standard C++?",
                   "void", "int", "float", "string",
                   "B", "Medium", "Programming");

    insertQuestion("Which data structure follows FIFO order?",
                   "Stack", "Queue", "Tree", "Graph",
                   "B", "Medium", "Programming");

    insertQuestion("Which data structure follows LIFO order?",
                   "Queue", "Array", "Stack", "List",
                   "C", "Medium", "Programming");

    insertQuestion("Which keyword is used for inheritance in C++ class declaration?",
                   "inherits", "extends", ":", "->",
                   "C", "Medium", "Programming");

    insertQuestion("Which OOP principle hides internal implementation details?",
                   "Inheritance", "Encapsulation", "Polymorphism", "Recursion",
                   "B", "Medium", "Programming");

    insertQuestion("What is the result type of 5 / 2 in integer division in C++?",
                   "2", "2.5", "3", "1",
                   "A", "Medium", "Programming");

    insertQuestion("Which header is needed for std::vector?",
                   "<array>", "<list>", "<vector>", "<map>",
                   "C", "Medium", "Programming");

    insertQuestion("What does IDE stand for?",
                   "Integrated Development Environment", "Internal Data Engine", "Indexed Debug Editor", "Intelligent Device Emulator",
                   "A", "Medium", "Programming");

    insertQuestion("What is recursion?",
                   "A loop with break", "A function calling itself", "An error message", "A data type",
                   "B", "Medium", "Programming");

    insertQuestion("Which statement is used to make a decision among multiple choices in C++?",
                   "switch", "loop", "struct", "using",
                   "A", "Medium", "Programming");

    insertQuestion("What does the bool data type store?",
                   "Characters", "Whole numbers", "True/False values", "Decimal values",
                   "C", "Medium", "Programming");

    insertQuestion("Which operator gives the remainder of division?",
                   "/", "%", "*", "+",
                   "B", "Medium", "Programming");

    insertQuestion("Which standard namespace is commonly used in C++ examples?",
                   "global", "core", "std", "main",
                   "C", "Medium", "Programming");

    insertQuestion("What is a function parameter?",
                   "A file type", "An input to a function", "A class member only", "A compiler",
                   "B", "Medium", "Programming");

    insertQuestion("Which keyword can prevent a function from changing a passed value reference target when used appropriately with parameters?",
                   "const", "static", "new", "friend",
                   "A", "Medium", "Programming");

    // =========================================================
    // PROGRAMMING - HARD (15)
    // =========================================================
    insertQuestion("What is polymorphism in OOP?",
                   "Using many files", "One interface, many implementations", "Hiding data only", "Fixing syntax errors",
                   "B", "Hard", "Programming");

    insertQuestion("Which access specifier makes class members accessible only within the class and its friends?",
                   "public", "protected", "private", "global",
                   "C", "Hard", "Programming");

    insertQuestion("What is the purpose of a constructor in C++?",
                   "Destroy objects", "Initialize objects", "Compare objects", "Print output",
                   "B", "Hard", "Programming");

    insertQuestion("Which keyword is used for runtime polymorphism with member functions?",
                   "virtual", "inline", "static", "const",
                   "A", "Hard", "Programming");

    insertQuestion("What is a pointer in C++?",
                   "A loop", "A variable storing an address", "A class type only", "A namespace",
                   "B", "Hard", "Programming");

    insertQuestion("What does nullptr represent?",
                   "A text value", "A null pointer constant", "Zero integer only", "A library",
                   "B", "Hard", "Programming");

    insertQuestion("Which container automatically stores key-value pairs in sorted key order by default?",
                   "vector", "queue", "map", "stack",
                   "C", "Hard", "Programming");

    insertQuestion("What is function overloading?",
                   "Having too many variables", "Using same function name with different parameter lists", "Calling a function twice", "A function returning many values",
                   "B", "Hard", "Programming");

    insertQuestion("What is the time complexity of binary search on a sorted array?",
                   "O(n)", "O(log n)", "O(n log n)", "O(1)",
                   "B", "Hard", "Programming");

    insertQuestion("Which concept allows one class to use features of another class?",
                   "Compilation", "Inheritance", "Tokenizing", "Linking",
                   "B", "Hard", "Programming");

    insertQuestion("What is an abstract class in C++?",
                   "A class with only variables", "A class that cannot be instantiated directly and may contain pure virtual functions", "A hidden library", "A class with no methods",
                   "B", "Hard", "Programming");

    insertQuestion("What is the purpose of a destructor?",
                   "Create an object", "Initialize an array", "Clean up when an object is destroyed", "Restart the program",
                   "C", "Hard", "Programming");

    insertQuestion("Which keyword is used to derive one class from another in C++ syntax?",
                   "using", "extends", ":", "inherits",
                   "C", "Hard", "Programming");

    insertQuestion("Which smart pointer owns an object exclusively in modern C++?",
                   "shared_ptr", "unique_ptr", "weak_ptr", "raw_ptr",
                   "B", "Hard", "Programming");

    insertQuestion("What is the main purpose of encapsulation?",
                   "To duplicate code", "To bundle data and methods and control access", "To increase file size", "To remove classes",
                   "B", "Hard", "Programming");

    qDebug() << "Bulk questions inserted successfully.";
}

bool DatabaseManager::saveScore(const QString &username,
                                int score,
                                const QString &category,
                                const QString &difficulty)
{
    QSqlQuery query;

    query.prepare("INSERT INTO scores (username, score, category, difficulty, played_at) "
                  "VALUES (?, ?, ?, ?, datetime('now'))");
    query.addBindValue(username);
    query.addBindValue(score);
    query.addBindValue(category);
    query.addBindValue(difficulty);

    if (!query.exec()) {
        qDebug() << "Failed to save score:" << query.lastError().text();
        return false;
    }

    qDebug() << "Score saved successfully.";
    return true;
}

bool DatabaseManager::addQuestion(const QString &questionText,
                                  const QString &optionA,
                                  const QString &optionB,
                                  const QString &optionC,
                                  const QString &optionD,
                                  const QString &correctAnswer,
                                  const QString &difficulty,
                                  const QString &category)
{
    QSqlQuery query;
    query.prepare("INSERT INTO questions "
                  "(question_text, option_a, option_b, option_c, option_d, correct_answer, difficulty, category) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(questionText);
    query.addBindValue(optionA);
    query.addBindValue(optionB);
    query.addBindValue(optionC);
    query.addBindValue(optionD);
    query.addBindValue(correctAnswer);
    query.addBindValue(difficulty);
    query.addBindValue(category);

    if (!query.exec()) {
        qDebug() << "Failed to add question:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteQuestion(int questionId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM questions WHERE id = ?");
    query.addBindValue(questionId);

    if (!query.exec()) {
        qDebug() << "Failed to delete question:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::updateQuestion(int questionId,
                                     const QString &questionText,
                                     const QString &optionA,
                                     const QString &optionB,
                                     const QString &optionC,
                                     const QString &optionD,
                                     const QString &correctAnswer,
                                     const QString &difficulty,
                                     const QString &category)
{
    QSqlQuery query;
    query.prepare("UPDATE questions SET "
                  "question_text = ?, "
                  "option_a = ?, "
                  "option_b = ?, "
                  "option_c = ?, "
                  "option_d = ?, "
                  "correct_answer = ?, "
                  "difficulty = ?, "
                  "category = ? "
                  "WHERE id = ?");

    query.addBindValue(questionText);
    query.addBindValue(optionA);
    query.addBindValue(optionB);
    query.addBindValue(optionC);
    query.addBindValue(optionD);
    query.addBindValue(correctAnswer);
    query.addBindValue(difficulty);
    query.addBindValue(category);
    query.addBindValue(questionId);

    if (!query.exec()) {
        qDebug() << "Failed to update question:" << query.lastError().text();
        return false;
    }

    return true;
}

QString DatabaseManager::generateSalt()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString DatabaseManager::hashPassword(const QString &password, const QString &salt)
{
    QByteArray hashed = QCryptographicHash::hash(
        (salt + password).toUtf8(),
        QCryptographicHash::Sha256
        );
    return QString(hashed.toHex());
}
bool DatabaseManager::updateUserRole(const QString &username, const QString &role)
{
    QSqlQuery query;
    query.prepare("UPDATE users SET role = ? WHERE username = ?");
    query.addBindValue(role);
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Failed to update user role:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::deleteUser(const QString &username)
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Failed to delete user:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
bool DatabaseManager::ensurePlayerProgressExists(const QString &username)
{
    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO player_progress (username, xp, level, games_played) VALUES (?, 0, 1, 0)");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Failed to ensure player progress exists:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::addXp(const QString &username, int xpGained)
{
    if (!ensurePlayerProgressExists(username)) {
        return false;
    }

    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT xp FROM player_progress WHERE username = ?");
    selectQuery.addBindValue(username);

    if (!selectQuery.exec() || !selectQuery.next()) {
        qDebug() << "Failed to load current XP:" << selectQuery.lastError().text();
        return false;
    }

    int currentXp = selectQuery.value(0).toInt();
    int newXp = currentXp + xpGained;
    int newLevel = (newXp / 100) + 1;

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE player_progress SET xp = ?, level = ? WHERE username = ?");
    updateQuery.addBindValue(newXp);
    updateQuery.addBindValue(newLevel);
    updateQuery.addBindValue(username);

    if (!updateQuery.exec()) {
        qDebug() << "Failed to update XP:" << updateQuery.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::incrementGamesPlayed(const QString &username)
{
    if (!ensurePlayerProgressExists(username)) {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE player_progress SET games_played = games_played + 1 WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Failed to increment games played:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::hasAchievement(const QString &username, const QString &achievementKey)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM user_achievements WHERE username = ? AND achievement_key = ?");
    query.addBindValue(username);
    query.addBindValue(achievementKey);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to check achievement:" << query.lastError().text();
        return false;
    }

    return query.value(0).toInt() > 0;
}

bool DatabaseManager::unlockAchievement(const QString &username, const QString &achievementKey)
{
    if (hasAchievement(username, achievementKey)) {
        return true;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO user_achievements (username, achievement_key, unlocked_at) VALUES (?, ?, datetime('now'))");
    query.addBindValue(username);
    query.addBindValue(achievementKey);

    if (!query.exec()) {
        qDebug() << "Failed to unlock achievement:" << query.lastError().text();
        return false;
    }

    return true;
}

int DatabaseManager::getPlayerLevel(const QString &username)
{
    if (!ensurePlayerProgressExists(username)) {
        return 1;
    }

    QSqlQuery query;
    query.prepare("SELECT level FROM player_progress WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get player level:" << query.lastError().text();
        return 1;
    }

    return query.value(0).toInt();
}

int DatabaseManager::getPlayerXp(const QString &username)
{
    if (!ensurePlayerProgressExists(username)) {
        return 0;
    }

    QSqlQuery query;
    query.prepare("SELECT xp FROM player_progress WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get player XP:" << query.lastError().text();
        return 0;
    }

    return query.value(0).toInt();
}

int DatabaseManager::getGamesPlayed(const QString &username)
{
    if (!ensurePlayerProgressExists(username)) {
        return 0;
    }

    QSqlQuery query;
    query.prepare("SELECT games_played FROM player_progress WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get games played:" << query.lastError().text();
        return 0;
    }

    return query.value(0).toInt();
}

int DatabaseManager::getAchievementCount(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM user_achievements WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get achievement count:" << query.lastError().text();
        return 0;
    }

    return query.value(0).toInt();
}
QStringList DatabaseManager::getUnlockedAchievements(const QString &username)
{
    QStringList achievements;

    QSqlQuery query;
    query.prepare("SELECT achievement_key FROM user_achievements WHERE username = ? ORDER BY unlocked_at ASC");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Failed to load unlocked achievements:" << query.lastError().text();
        return achievements;
    }

    while (query.next()) {
        QString key = query.value(0).toString();

        if (key == "first_game") {
            achievements << "First Game";
        } else if (key == "first_win") {
            achievements << "First Win";
        } else if (key == "perfect_score") {
            achievements << "Perfect Score";
        } else if (key == "five_games") {
            achievements << "Five Games Played";
        } else {
            achievements << key;
        }
    }

    return achievements;
}
QString DatabaseManager::getAchievementUnlockedAt(const QString &username, const QString &achievementKey)
{
    QSqlQuery query;
    query.prepare("SELECT unlocked_at FROM user_achievements WHERE username = ? AND achievement_key = ?");
    query.addBindValue(username);
    query.addBindValue(achievementKey);

    if (!query.exec() || !query.next()) {
        return "";
    }

    return query.value(0).toString();
}


