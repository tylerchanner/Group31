/**
 * @file OptionDialog.h
 *
 * Defines the OptionDialog class, which provides a user interface for editing the properties
 * such as name, color, and visibility of an object or model part. This dialog facilitates
 * interactive user modifications to the attributes of selected items within the application.
 */

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QColor>

namespace Ui {
    class OptionDialog;
}

/**
 * @class OptionDialog
 * @brief A dialog for adjusting properties like name, color, and visibility.
 *
 * This dialog class is utilized for editing various properties of an item, including its name,
 * color, and visibility status. The class provides a graphical user interface for these modifications,
 * integrating seamlessly with the rest of the application's UI components.
 */
class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget* parent = nullptr);
    ~OptionDialog();

    QString getName() const; ///< Retrieves the item's name from the dialog.
    QColor getColor() const; ///< Retrieves the selected color from the dialog.
    bool getVisibility() const; ///< Checks the visibility status from the dialog.

    void setName(const QString& name); ///< Sets the item's name in the dialog.
    void setColor(const QColor& color); ///< Updates the color displayed in the dialog.
    void setVisibility(bool isVisible); ///< Sets the visibility status in the dialog.

private:
    Ui::OptionDialog* ui; ///< Pointer to the user interface elements of the dialog.

    void setupColorChangeConnections(); ///< Initializes connections for color value changes.
    void updateRedValue(const QString& text); ///< Handles updates to the red color component.
    void updateGreenValue(const QString& text); ///< Handles updates to the green color component.
    void updateBlueValue(const QString& text); ///< Handles updates to the blue color component.
    void openColorDialog();
};

#endif // OPTIONDIALOG_H
