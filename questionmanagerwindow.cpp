#include "questionmanagerwindow.h"
#include "ui_questionmanagerwindow.h"
#include "databasemanager.h"
#include "addquestiondialog.h"
#include "dashboardwindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QAbstractAnimation>

QuestionManagerWindow::QuestionManagerWindow(const QString &username,
                                             const QString &role,
                                             QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QuestionManagerWindow)
    , backgroundLabel(nullptr)
    , currentUsername(username)
    , currentRole(role)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    connect(ui->lineEditSearch, &QLineEdit::textChanged,
            this, &QuestionManagerWindow::on_lineEditSearch_textChanged);

    connect(ui->comboBoxCategoryFilter, &QComboBox::currentTextChanged,
            this, &QuestionManagerWindow::on_comboBoxCategoryFilter_currentTextChanged);

    setWindowTitle("Challenge 30 - Question Manager");

    // Background
    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    // Main frame
    ui->frameQuestionManager->setStyleSheet(
        "#frameQuestionManager {"
        "background-color: rgba(0, 0, 0, 95);"
        "border-radius: 18px;"
        "}"
        );

    // Title
    ui->labelTitle->setText("Question Manager");
    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    // Search + filter style
    ui->lineEditSearch->setPlaceholderText("Search question...");
    ui->lineEditSearch->setStyleSheet(
        "QLineEdit {"
        "background-color: rgba(255, 255, 255, 230);"
        "color: black;"
        "border: 2px solid #2563eb;"
        "border-radius: 10px;"
        "padding: 8px;"
        "font-size: 14px;"
        "}"
        );

    ui->comboBoxCategoryFilter->setStyleSheet(
        "QComboBox {"
        "background-color: rgba(255, 255, 255, 230);"
        "color: black;"
        "border: 2px solid #2563eb;"
        "border-radius: 10px;"
        "padding: 6px 10px;"
        "font-size: 14px;"
        "}"
        "QComboBox QAbstractItemView {"
        "background-color: white;"
        "selection-background-color: #2563eb;"
        "selection-color: white;"
        "}"
        );

    // Table style
    ui->tableWidgetQuestions->setColumnCount(5);
    ui->tableWidgetQuestions->setHorizontalHeaderLabels(
        QStringList() << "ID" << "Question" << "Difficulty" << "Category" << "Correct Answer"
        );

    ui->tableWidgetQuestions->setStyleSheet(
        "QTableWidget {"
        "background-color: rgba(255, 255, 255, 230);"
        "color: #0f172a;"
        "border: none;"
        "border-radius: 10px;"
        "gridline-color: #cbd5e1;"
        "font-size: 14px;"
        "}"
        "QHeaderView::section {"
        "background-color: #2563eb;"
        "color: white;"
        "padding: 8px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    ui->tableWidgetQuestions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetQuestions->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidgetQuestions->verticalHeader()->setVisible(false);
    ui->tableWidgetQuestions->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetQuestions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetQuestions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetQuestions->setAlternatingRowColors(true);

    // Buttons style
    QString blueButtonStyle =
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}";

    QString darkButtonStyle =
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}";

    ui->pushButtonAddQuestion->setStyleSheet(blueButtonStyle);
    ui->pushButtonEditQuestion->setStyleSheet(blueButtonStyle);
    ui->pushButtonDeleteQuestion->setStyleSheet(darkButtonStyle);
    ui->pushButtonBack->setStyleSheet(darkButtonStyle);

    loadCategories();
    loadQuestions();

    // Fade-in animation
    this->setWindowOpacity(0.0);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(400);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    this->showMaximized();
}

QuestionManagerWindow::~QuestionManagerWindow()
{
    delete ui;
}

void QuestionManagerWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

bool QuestionManagerWindow::isValidQuestionRow(int row) const
{
    if (row < 0) {
        return false;
    }

    QTableWidgetItem *idItem = ui->tableWidgetQuestions->item(row, 0);
    if (!idItem) {
        return false;
    }

    bool ok = false;
    idItem->text().toInt(&ok);
    return ok;
}

void QuestionManagerWindow::loadCategories()
{
    ui->comboBoxCategoryFilter->clear();
    ui->comboBoxCategoryFilter->addItem("All Categories");

    QSqlQuery query("SELECT DISTINCT category FROM questions ORDER BY category");
    while (query.next()) {
        ui->comboBoxCategoryFilter->addItem(query.value(0).toString());
    }
}

void QuestionManagerWindow::loadQuestions(const QString &searchText, const QString &category)
{
    ui->tableWidgetQuestions->clearContents();
    ui->tableWidgetQuestions->setRowCount(0);

    QSqlQuery query;
    QString sql =
        "SELECT id, question_text, difficulty, category, correct_answer "
        "FROM questions WHERE 1=1";

    if (!searchText.trimmed().isEmpty()) {
        sql += " AND question_text LIKE :search";
    }

    if (category != "All Categories") {
        sql += " AND category = :category";
    }

    sql += " ORDER BY id DESC";

    query.prepare(sql);

    if (!searchText.trimmed().isEmpty()) {
        query.bindValue(":search", "%" + searchText.trimmed() + "%");
    }

    if (category != "All Categories") {
        query.bindValue(":category", category);
    }

    if (!query.exec()) {
        qDebug() << "Error loading questions:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->tableWidgetQuestions->insertRow(row);

        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetQuestions->setItem(row, col, item);
        }

        row++;
    }

    if (row == 0) {
        ui->tableWidgetQuestions->setRowCount(1);
        ui->tableWidgetQuestions->setSpan(0, 0, 1, ui->tableWidgetQuestions->columnCount());

        QTableWidgetItem *item = new QTableWidgetItem("No questions found");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetQuestions->setItem(0, 0, item);
    }
}

void QuestionManagerWindow::on_lineEditSearch_textChanged(const QString &text)
{
    loadQuestions(text, ui->comboBoxCategoryFilter->currentText());
}

void QuestionManagerWindow::on_comboBoxCategoryFilter_currentTextChanged(const QString &text)
{
    loadQuestions(ui->lineEditSearch->text(), text);
}

void QuestionManagerWindow::on_pushButtonAddQuestion_clicked()
{
    AddQuestionDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        loadCategories();
        loadQuestions(ui->lineEditSearch->text(), ui->comboBoxCategoryFilter->currentText());
        QMessageBox::information(this, "Add Question", "Question added successfully.");
    }
}

void QuestionManagerWindow::on_pushButtonEditQuestion_clicked()
{
    int row = ui->tableWidgetQuestions->currentRow();

    if (!isValidQuestionRow(row)) {
        QMessageBox::warning(this, "Edit Question", "Please select a valid question first.");
        return;
    }

    int questionId = ui->tableWidgetQuestions->item(row, 0)->text().toInt();

    QSqlQuery query;
    query.prepare(
        "SELECT question_text, option_a, option_b, option_c, option_d, correct_answer, difficulty, category "
        "FROM questions WHERE id = ?"
        );
    query.addBindValue(questionId);

    if (!query.exec()) {
        QMessageBox::warning(this, "Edit Question", "Failed to load question data.");
        return;
    }

    if (query.next()) {
        AddQuestionDialog dialog(this);
        dialog.setEditMode(questionId,
                           query.value(0).toString(),
                           query.value(1).toString(),
                           query.value(2).toString(),
                           query.value(3).toString(),
                           query.value(4).toString(),
                           query.value(5).toString(),
                           query.value(6).toString(),
                           query.value(7).toString());

        if (dialog.exec() == QDialog::Accepted) {
            loadCategories();
            loadQuestions(ui->lineEditSearch->text(), ui->comboBoxCategoryFilter->currentText());
            QMessageBox::information(this, "Edit Question", "Question updated successfully.");
        }
    }
}

void QuestionManagerWindow::on_pushButtonDeleteQuestion_clicked()
{
    int row = ui->tableWidgetQuestions->currentRow();

    if (!isValidQuestionRow(row)) {
        QMessageBox::warning(this, "Delete Question", "Please select a valid question first.");
        return;
    }

    int questionId = ui->tableWidgetQuestions->item(row, 0)->text().toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Delete Question",
        "Are you sure you want to delete this question?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::No) {
        return;
    }

    if (DatabaseManager::deleteQuestion(questionId)) {
        loadCategories();
        loadQuestions(ui->lineEditSearch->text(), ui->comboBoxCategoryFilter->currentText());
        QMessageBox::information(this, "Delete Question", "Question deleted successfully.");
    } else {
        QMessageBox::critical(this, "Delete Question", "Failed to delete question.");
    }
}

void QuestionManagerWindow::on_pushButtonBack_clicked()
{
    DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
    dashboard->show();
    this->close();
}