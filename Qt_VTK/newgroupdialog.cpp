/**
 * @file newgroupdialog.cpp
 * Implementation of the NewGroupDialog class, which provides a dialog interface for creating a new group.
 * This file includes the definitions of the NewGroupDialog constructor, destructor, and its member functions.
 */

#include "newgroupdialog.h"
#include "ui_newgroupdialog.h"

 /**
  * Constructor for the NewGroupDialog class.
  * Initializes a new instance of the NewGroupDialog with the provided parent.
  * It sets up the user interface for the dialog.
  *
  * @param parent The parent widget of this dialog, typically a main window or nullptr.
  */
NewGroupDialog::NewGroupDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::NewGroupDialog)
{
    ui->setupUi(this); // Setup the generated UI
}

/**
 * Destructor for the NewGroupDialog class.
 * Cleans up the allocated resources by deleting the Ui::NewGroupDialog object.
 */
NewGroupDialog::~NewGroupDialog()
{
    delete ui; // Ensure proper deletion of the UI resources
}

/**
 * Retrieves the name of the group from the input field in the dialog.
 * This method returns the text contained in the lineEdit UI element,
 * which is intended to be the name of the new group.
 *
 * @return The name of the group as a QString.
 */
QString NewGroupDialog::getGroupName() const
{
    return ui->lineEdit->text(); // Extract the text from the lineEdit widget
}
