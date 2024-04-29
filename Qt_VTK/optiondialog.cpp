/**
 * @file optiondialog.cpp
 * @brief Implementation of the OptionDialog class.
 *
 * This file implements the functionality of the OptionDialog class. This class provides
 * a user interface for editing the properties of an item, including its name, color,
 * and visibility state.
 */

#include "optiondialog.h"
#include "ui_optiondialog.h"
#include <QColorDialog>


 /**
  * @brief Constructs an OptionDialog object with a parent.
  *
  * Initializes the UI components and sets up connections between the UI elements
  * and the class methods to enable real-time updates of the properties.
  *
  * @param parent The parent widget of this dialog, nullptr if there's no parent.
  */
OptionDialog::OptionDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::OptionDialog) {
    ui->setupUi(this);
    ui->checkBox->setChecked(true); // Default checked state
    connect(ui->pushButton, &QPushButton::clicked, this, &OptionDialog::openColorDialog);


    // Connect scroll bars to line edits for RGB values
    setupColorChangeConnections();
}

/**
 * @brief Destroys the OptionDialog object.
 */
OptionDialog::~OptionDialog() {
    delete ui;
}

void OptionDialog::openColorDialog() {
    QColor initialColor = getColor(); // Get the currently selected color
    QColor color = QColorDialog::getColor(initialColor, this, "Select Color");

    if (color.isValid()) {
        setColor(color);
    }
}



/**
 * @brief Retrieves the entered name from the dialog.
 *
 * @return QString containing the name.
 */
QString OptionDialog::getName() const {
    return ui->plainTextEdit->toPlainText();
}

/**
 * @brief Retrieves the selected color from the dialog.
 *
 * @return QColor representing the selected color.
 */
QColor OptionDialog::getColor() const {
    return QColor(ui->horizontalScrollBarRed->value(),
        ui->horizontalScrollBarGreen->value(),
        ui->horizontalScrollBarBlue->value());
}

/**
 * @brief Retrieves the visibility state from the dialog.
 *
 * @return bool representing the visibility (checked state of the checkbox).
 */
bool OptionDialog::getVisibility() const {
    return ui->checkBox->isChecked();
}

/**
 * @brief Sets the displayed name in the dialog.
 *
 * @param name The QString to set as the name.
 */
void OptionDialog::setName(const QString& name) {
    ui->plainTextEdit->setPlainText(name);
}

/**
 * @brief Sets the displayed color in the dialog.
 *
 * @param color The QColor to set as the current color.
 */
void OptionDialog::setColor(const QColor& color) {
    ui->horizontalScrollBarRed->setValue(color.red());
    ui->horizontalScrollBarGreen->setValue(color.green());
    ui->horizontalScrollBarBlue->setValue(color.blue());
}

/**
 * @brief Sets the visibility state in the dialog.
 *
 * @param isVisible The visibility state to set (true for visible, false for hidden).
 */
void OptionDialog::setVisibility(bool isVisible) {
    ui->checkBox->setChecked(isVisible);
}

/**
 * @brief Sets up connections between UI elements for real-time updates of color values.
 */
void OptionDialog::setupColorChangeConnections() {
    connect(ui->horizontalScrollBarRed, &QScrollBar::valueChanged, this, [this](int value) {
        ui->lineEdit->setText(QString::number(value));
        });
    connect(ui->horizontalScrollBarGreen, &QScrollBar::valueChanged, this, [this](int value) {
        ui->lineEdit_2->setText(QString::number(value));
        });
    connect(ui->horizontalScrollBarBlue, &QScrollBar::valueChanged, this, [this](int value) {
        ui->lineEdit_3->setText(QString::number(value));
        });

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &OptionDialog::updateRedValue);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &OptionDialog::updateGreenValue);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &OptionDialog::updateBlueValue);
}

/**
 * @brief Updates the red value in the color preview based on the line edit input.
 *
 * @param text The new text of the red line edit, expected to be a valid integer.
 */
void OptionDialog::updateRedValue(const QString& text) {
    bool ok;
    int value = text.toInt(&ok);
    if (ok && ui->horizontalScrollBarRed->value() != value) {
        ui->horizontalScrollBarRed->setValue(value);
    }
}

/**
 * @brief Updates the green value in the color preview based on the line edit input.
 *
 * @param text The new text of the green line edit, expected to be a valid integer.
 */
void OptionDialog::updateGreenValue(const QString& text) {
    bool ok;
    int value = text.toInt(&ok);
    if (ok && ui->horizontalScrollBarGreen->value() != value) {
        ui->horizontalScrollBarGreen->setValue(value);
    }
}

/**
 * @brief Updates the blue value in the color preview based on the line edit input.
 *
 * @param text The new text of the blue line edit, expected to be a valid integer.
 */
void OptionDialog::updateBlueValue(const QString& text) {
    bool ok;
    int value = text.toInt(&ok);
    if (ok && ui->horizontalScrollBarBlue->value() != value) {
        ui->horizontalScrollBarBlue->setValue(value);
    }
}
