//#include "keypaddialog.h"
//#include "ui_keypaddialog.h"
//#include <QPushButton>

//KeypadDialog::KeypadDialog(QWidget *parent)
//    : QDialog(parent), ui(new Ui::KeypadDialog), inputValue("") {
//    ui->setupUi(this);

//    // Connect all buttons to same slot
//    QList<QPushButton *> buttons = findChildren<QPushButton *>();
//    for (auto *button : buttons)
//        connect(button, &QPushButton::clicked, this, &KeypadDialog::onButtonClicked);
//}

//KeypadDialog::~KeypadDialog() {
//    delete ui;
//}

//void KeypadDialog::onButtonClicked() {
//    QPushButton *btn = qobject_cast<QPushButton *>(sender());
//    if (!btn) return;

//    QString text = btn->text();
//    if (text == "Enter") {
//        accept();  // close dialog
//    } else if (text == "Clear") {
//        inputValue.clear();
//        ui->display->setText("");
//    } else {
//        inputValue += text;
//        ui->display->setText(inputValue);
//    }
//}

//QString KeypadDialog::getValue() const {
//    return inputValue;
//}

#include "keypaddialog.h"
#include <QFont>

// ─────────────────────────────────────────────────────────────────────────────
static const QString BTN_STYLE =
    "QPushButton {"
    "  background-color: #3a3a3a;"
    "  color: white;"
    "  border: 1px solid #666;"
    "  border-radius: 6px;"
    "  font-size: 18px;"
    "  font-weight: bold;"
    "}"
    "QPushButton:pressed { background-color: #0078d7; }";

static const QString OK_STYLE =
    "QPushButton {"
    "  background-color: #007a3d;"
    "  color: white;"
    "  border-radius: 6px;"
    "  font-size: 18px;"
    "  font-weight: bold;"
    "}"
    "QPushButton:pressed { background-color: #005a2d; }";

static const QString CANCEL_STYLE =
    "QPushButton {"
    "  background-color: #b00000;"
    "  color: white;"
    "  border-radius: 6px;"
    "  font-size: 18px;"
    "  font-weight: bold;"
    "}"
    "QPushButton:pressed { background-color: #800000; }";

// ─────────────────────────────────────────────────────────────────────────────
KeypadDialog::KeypadDialog(const QString &title,
                           const QString &currentValue,
                           Mode           mode,
                           QWidget       *parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);
    setStyleSheet("background-color: #1e1e1e;");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setSpacing(6);
    root->setContentsMargins(12, 12, 12, 12);

    // ── Title label ──────────────────────────────────────────────────────────
    QLabel *lbl = new QLabel(title, this);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("color: #cccccc; font-size: 16px; font-weight: bold;");
    root->addWidget(lbl);

    // ── Display field ────────────────────────────────────────────────────────
    m_display = new QLineEdit(currentValue, this);
    m_display->setReadOnly(true);
    m_display->setAlignment(Qt::AlignRight);
    m_display->setMinimumHeight(52);
    m_display->setStyleSheet(
        "QLineEdit {"
        "  background: #111;"
        "  color: #00ff99;"
        "  border: 1px solid #444;"
        "  border-radius: 4px;"
        "  font-size: 24px;"
        "  padding-right: 8px;"
        "}");
    root->addWidget(m_display);

    // ── Key grid ─────────────────────────────────────────────────────────────
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(6);

    if (mode == Numeric)
        buildNumericPad(grid);
    else
        buildAlphaPad(grid);

    root->addLayout(grid);

    // ── OK / Cancel ──────────────────────────────────────────────────────────
    QHBoxLayout *hb = new QHBoxLayout();
    hb->setSpacing(8);

    QPushButton *btnOK     = makeBtn("OK",     56);
    QPushButton *btnCancel = makeBtn("Cancel", 56);
    btnOK    ->setStyleSheet(OK_STYLE);
    btnCancel->setStyleSheet(CANCEL_STYLE);
    btnOK    ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(btnOK,     &QPushButton::clicked, this, &KeypadDialog::onOK);
    connect(btnCancel, &QPushButton::clicked, this, &KeypadDialog::onCancel);

    hb->addWidget(btnCancel);
    hb->addWidget(btnOK);
    root->addLayout(hb);

    adjustSize();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Numeric pad  (3×4 digits + decimal + backspace + clear)
//
//   [ 7 ] [ 8 ] [ 9 ]
//   [ 4 ] [ 5 ] [ 6 ]
//   [ 1 ] [ 2 ] [ 3 ]
//   [ . ] [ 0 ] [BCK]
//   [      CLR       ]
// ─────────────────────────────────────────────────────────────────────────────
void KeypadDialog::buildNumericPad(QGridLayout *grid)
{
    const QStringList keys = { "7","8","9",
                                "4","5","6",
                                "1","2","3",
                                ".","0","⌫" };
    int row = 0, col = 0;
    for (const QString &k : keys) {
        QPushButton *btn = makeBtn(k);
        btn->setProperty("keyValue", k);
        if (k == "⌫")
            connect(btn, &QPushButton::clicked, this, &KeypadDialog::onBackspace);
        else
            connect(btn, &QPushButton::clicked, this, &KeypadDialog::onKeyPressed);
        grid->addWidget(btn, row, col);
        col++;
        if (col == 3) { col = 0; row++; }
    }

    // Full-width CLR button
    QPushButton *btnClr = makeBtn("CLR");
    btnClr->setStyleSheet(
        "QPushButton { background-color:#555; color:white; border-radius:6px;"
        " font-size:18px; font-weight:bold; }"
        "QPushButton:pressed { background-color:#333; }");
    connect(btnClr, &QPushButton::clicked, this, &KeypadDialog::onClear);
    grid->addWidget(btnClr, row, 0, 1, 3);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Alphanumeric pad  (letters A-Z + digits 0-9 + backspace + clear)
//  Layout: 7 rows of keys
// ─────────────────────────────────────────────────────────────────────────────
void KeypadDialog::buildAlphaPad(QGridLayout *grid)
{
    // Row 0: Q W E R T Y U I O P
    // Row 1: A S D F G H J K L
    // Row 2: Z X C V B N M
    // Row 3: 1 2 3 4 5 6 7 8 9 0
    // Row 4: ⌫   CLR
    const QVector<QStringList> rows = {
        {"Q","W","E","R","T","Y","U","I","O","P"},
        {"A","S","D","F","G","H","J","K","L"},
        {"Z","X","C","V","B","N","M"},
        {"1","2","3","4","5","6","7","8","9","0"}
    };

    for (int r = 0; r < rows.size(); ++r) {
        const QStringList &rowKeys = rows[r];
        int totalCols = 10;  // normalise to 10 columns
        int startCol  = (totalCols - rowKeys.size()) / 2;
        for (int c = 0; c < rowKeys.size(); ++c) {
            QPushButton *btn = makeBtn(rowKeys[c], 52);
            btn->setProperty("keyValue", rowKeys[c]);
            connect(btn, &QPushButton::clicked, this, &KeypadDialog::onKeyPressed);
            grid->addWidget(btn, r, startCol + c);
        }
    }

    // Backspace + CLR row
    QPushButton *btnBck = makeBtn("⌫", 52);
    connect(btnBck, &QPushButton::clicked, this, &KeypadDialog::onBackspace);
    grid->addWidget(btnBck, 4, 0, 1, 5);

    QPushButton *btnClr = makeBtn("CLR", 52);
    btnClr->setStyleSheet(
        "QPushButton { background-color:#555; color:white; border-radius:6px;"
        " font-size:18px; font-weight:bold; }"
        "QPushButton:pressed { background-color:#333; }");
    connect(btnClr, &QPushButton::clicked, this, &KeypadDialog::onClear);
    grid->addWidget(btnClr, 4, 5, 1, 5);
}

// ─────────────────────────────────────────────────────────────────────────────
QPushButton *KeypadDialog::makeBtn(const QString &label, int minH)
{
    QPushButton *btn = new QPushButton(label, this);
    btn->setMinimumHeight(minH);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setStyleSheet(BTN_STYLE);
    return btn;
}

// ─────────────────────────────────────────────────────────────────────────────
void KeypadDialog::onKeyPressed()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString key = btn->property("keyValue").toString();

    // For numeric mode: allow only one decimal point
    if (key == "." && m_display->text().contains("."))
        return;

    m_display->setText(m_display->text() + key);
}

void KeypadDialog::onBackspace()
{
    QString txt = m_display->text();
    if (!txt.isEmpty())
        m_display->setText(txt.left(txt.length() - 1));
}

void KeypadDialog::onClear()
{
    m_display->clear();
}

void KeypadDialog::onOK()
{
    accept();
}

void KeypadDialog::onCancel()
{
    reject();
}
