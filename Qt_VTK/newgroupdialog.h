/**
 * @file NewGroupDialog.h
 *
 * Defines the NewGroupDialog class, which is a dialog used in a Qt application to prompt the user
 * for entering a name for a new group. This dialog is typically used when the user wants to create
 * a new group in a tree or list within the application's UI, providing a simple interface for naming.
 */

#ifndef NEWGROUPDIALOG_H
#define NEWGROUPDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
    class NewGroupDialog;
}

/**
 * @class NewGroupDialog
 * @brief Dialog class for creating a new group.
 *
 * This class provides a dialog window where the user can enter the name of a new group.
 * It's typically used to input names for creating new groups in a hierarchical data structure or tree view.
 */
class NewGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGroupDialog(QWidget* parent = nullptr);
    ~NewGroupDialog();

    /**
     * @brief Retrieves the entered name for the new group.
     *
     * This function fetches the text from the input field, which is intended to be the name
     * of a new group that the user wants to create.
     *
     * @return Returns the group name as a QString.
     */
    QString getGroupName() const;

private:
    Ui::NewGroupDialog* ui; ///< Pointer to the user interface for this dialog.
};

#endif // NEWGROUPDIALOG_H
