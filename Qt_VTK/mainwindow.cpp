/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class.
 *
 * This implementation file provides the functionality of the MainWindow class.
 * It includes the setup of the user interface, the initialization of the tree view model,
 * and the handling of various UI events such as button clicks and actions triggered.
 */

#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "OptionDialog.h"
#include "NewGroupDialog.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <QDebug>
#include <QMessageBox>
#include <vtkPlaneSource.h>
#include <QInputDialog>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <QtConcurrent/QtConcurrentRun>


 /**
  * @brief Constructs the MainWindow object.
  *
  * Initializes the user interface, sets up the tree view, actions, renderer,
  * and connects the necessary signals and slots.
  *
  * @param parent The parent widget of the MainWindow, or nullptr if no parent.
  */
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    partList(nullptr) {
    ui->setupUi(this);
    initializePartList();
    setupTreeView();
    setupActions();
    setupRenderer();
    connectSignals();
}

/**
 * @brief Destructor for the MainWindow class.
 *
 * Cleans up the user interface and the dynamically allocated partList.
 */
MainWindow::~MainWindow() {
    delete ui;
    delete partList;
}

/**
 * @brief Initializes the part list model.
 *
 * This function creates a new ModelPartList and assigns it to partList.
 */
void MainWindow::initializePartList() {
    partList = new ModelPartList("Parts List");
}

/**
 * @brief Sets up the tree view in the UI.
 *
 * Configures the model for the tree view and establishes the context menu policy.
 * It also populates the tree with initial data.
 */
void MainWindow::setupTreeView() {
    ui->treeView->setModel(partList);
    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    addModelPartToTree();
}

/**
 * @brief Adds a model part to the tree view.
 *
 * Creates a root item and a child item, appending the child to the root in the tree.
 */
void MainWindow::addModelPartToTree() {
    ModelPart* rootItem = partList->getRootItem();
    ModelPart* childItem = new ModelPart({ "Model", "true", "255,255,255" });
    rootItem->appendChild(childItem);
}

/**
 * @brief Sets up the actions in the UI.
 *
 * Initializes actions for item options and new group, connecting them to their respective slots.
 */
void MainWindow::setupActions() {
    createAction(&actionItemOptions, tr("Item Options"), &MainWindow::on_actionItemOptions_triggered);
    createAction(&actionNewGroup, tr("New Group"), &MainWindow::on_actionNewGroup_triggered);
    createAction(&actionDeleteItem, tr("Delete Item"), &MainWindow::on_actionDeleteFile_triggered);
    
}

/**
 * @brief Creates and configures a QAction.
 *
 * @param action Pointer to the QAction pointer to be initialized.
 * @param text The display text for the action.
 * @param slot The slot to connect the action's triggered signal to.
 */
void MainWindow::createAction(QAction** action, const QString& text, void (MainWindow::* slot)()) {
    *action = new QAction(text, this);
    ui->treeView->addAction(*action);
    connect(*action, &QAction::triggered, this, slot);
}

/**
 * @brief Sets up the renderer for the VTK visualization.
 *
 * Initializes the VTK render window and renderer, and associates them with the UI.
 */
void MainWindow::setupRenderer() {
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    addFloor(); // Add the floor to the scene
}


/**
 * @brief Connects signals from various UI elements to the corresponding slots.
 */
void MainWindow::connectSignals() {
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->actionDelete_File, &QAction::triggered, this, &MainWindow::on_actionDeleteFile_triggered);
    connect(ui->actionItem_Options, &QAction::triggered, this, &MainWindow::on_actionItemOptions_triggered);
    connect(ui->actionNew_Group, &QAction::triggered, this, &MainWindow::on_actionNewGroup_triggered);
    connect(ui->actionSearch_Items, &QAction::triggered, this, &MainWindow::on_actionSearchItem_triggered);
}

/**
 * @brief Handles the event when the tree view is clicked.
 *
 * When a tree item is clicked, this function retrieves the selected item's data
 * and emits a status update message to display the item's name.
 */
void MainWindow::handleTreeClicked() {
    // Get the index of the selected item
    QModelIndex index = ui->treeView->currentIndex();

    // Get a pointer to the item from the index
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    // Retrieve the name string from the internal QVariant data array
    QString text = selectedPart->data(0).toString();

    emit statusUpdateMessage("The selected item is: " + text, 2000);
}

/**
 * @brief Triggered when the 'Item Options' action is activated.
 *
 * Opens a dialog for editing the properties (name, visibility, color) of the selected item.
 * If changes are confirmed, it applies the updated properties to the selected item
 * and all its child items recursively.
 */
void MainWindow::on_actionItemOptions_triggered() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("No Selection"), tr("There is no selected part."));
        return;
    }

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    if (!selectedPart) {
        QMessageBox::information(this, tr("No Selection"), tr("There is no selected part."));
        return;
    }

    OptionDialog dialog(this);
    dialog.setName(selectedPart->data(0).toString());
    dialog.setColor(QColor(selectedPart->getColourR(), selectedPart->getColourG(), selectedPart->getColourB()));
    dialog.setVisibility(selectedPart->visible());

    if (dialog.exec() == QDialog::Accepted) {
        QColor color = dialog.getColor();
        applyPropertiesToPart(selectedPart, dialog.getName(), dialog.getVisibility(), color, true);
        updateChildrenProperties(selectedPart, dialog.getVisibility(), color);
        emit statusUpdateMessage("Item and its children updated.", 2000);
    }
}

/**
 * @brief Applies specified properties to a part.
 *
 * Sets the provided name, visibility, and color to the specified part. If updateName is true,
 * the name of the part is updated along with its visibility and color.
 *
 * @param part The part to which the properties will be applied.
 * @param name The new name to set, applicable only if updateName is true.
 * @param visibility The new visibility state to apply.
 * @param color The new color to apply.
 * @param updateName Flag to determine whether to update the part's name.
 */
void MainWindow::applyPropertiesToPart(ModelPart* part, const QString& name, bool visibility, const QColor& color, bool updateName) {
    if (!part) return;

    if (updateName) {
        part->set(0, QVariant(name)); // Update name only if updateName is true
    }
    part->set(1, QVariant(visibility ? "true" : "false"));
    part->set(2, QVariant(QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue())));

    part->setColour(color.red(), color.green(), color.blue());
    part->setVisible(visibility);

    QAbstractItemModel* model = ui->treeView->model();
    QModelIndex startIndex = model->index(part->row(), 0);
    QModelIndex endIndex = model->index(part->row(), model->columnCount());
    model->dataChanged(startIndex, endIndex);
    ui->treeView->update(startIndex);

    vtkSmartPointer<vtkActor> actor = part->getActor();
    if (actor) {
        actor->SetVisibility(visibility);
        actor->GetProperty()->SetDiffuseColor(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
        updateRender();
    }
}

/**
 * @brief Recursively updates the properties of child parts.
 *
 * Applies the specified visibility and color recursively to all child parts of the given part.
 * The name of the child parts is not updated.
 *
 * @param part The part whose children will be updated.
 * @param visibility The visibility state to apply to all child parts.
 * @param color The color to apply to all child parts.
 */
void MainWindow::updateChildrenProperties(ModelPart* part, bool visibility, const QColor& color) {
    for (int i = 0; i < part->childCount(); ++i) {
        ModelPart* child = part->child(i);
        applyPropertiesToPart(child, QString(), visibility, color, false); // false to not update name
        updateChildrenProperties(child, visibility, color); // Recursive call
    }
}

/**
 * @brief Updates the renderer with the current tree structure.
 *
 * Iterates over the top-level items in the tree and updates the rendering accordingly.
 * This includes resetting the camera and updating the render view.
 */
void MainWindow::updateRender() {
    renderer->RemoveAllViewProps(); // Remove existing actors

   

    int topLevelItemCount = partList->rowCount(QModelIndex());
    for (int i = 0; i < topLevelItemCount; ++i) {
        QModelIndex topLevelIndex = partList->index(i, 0, QModelIndex());
        updateRenderFromTree(topLevelIndex);
    }

    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();
    
    renderer->Render();
    
}


/**
 * @brief Recursively adds actors to the renderer from the tree structure.
 *
 * Starting from the provided tree index, this function adds corresponding actors
 * to the renderer. It recurses through all child items, adding each to the renderer.
 *
 * @param index The model index to start adding actors from.
 */
void MainWindow::updateRenderFromTree(const QModelIndex& index) {
    if (index.isValid()) {
        ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
        if (selectedPart) {
            vtkActor* actor = selectedPart->getActor();
            if (actor) {
                renderer->AddActor(actor);
            }
        }
        int rows = partList->rowCount(index);
        for (int i = 0; i < rows; ++i) {
            updateRenderFromTree(partList->index(i, 0, index));
        }
    }
}

/**
 * @brief Slot triggered to open and load files.
 *
 * Opens a file dialog allowing the user to select and load STL files. Each selected file
 * creates a new ModelPart that is appended to the tree and rendered in the viewport.
 */
void MainWindow::on_actionOpen_File_triggered() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"), QDir::homePath(), tr("STL Files (*.stl);;Text Files (*.txt)"));
    for (const QString& fileName : fileNames) {
        if (!fileName.isEmpty()) {
            createModelPartFromFile(fileName);
        }
    }
}

/**
 * @brief Creates a ModelPart from a file and adds it to the tree.
 *
 * Given a file name, this function creates a new ModelPart, loads the data from the file,
 * and appends it to the model tree, updating the rendering accordingly.
 *
 * @param fileName The name of the file to create the ModelPart from.
 */
void MainWindow::createModelPartFromFile(const QString& fileName) {
    // Run the STL loading in a separate thread
    QtConcurrent::run([this, fileName] {
        // This code is now running in a separate thread
        QFileInfo fileInfo(fileName);
        QString justFileName = fileInfo.fileName();

        QList<QVariant> data = { QVariant(justFileName), QVariant("true"), QVariant("255,255,255") };
        ModelPart* newPart = new ModelPart(data);

        // Load STL file (heavy operation)
        newPart->loadSTL(fileName);

        QColor whiteColor(255, 255, 255);
        newPart->setColour(whiteColor.red(), whiteColor.green(), whiteColor.blue());
        newPart->setVisible(true);

        // Once done, schedule the following code to be run on the main thread
        QMetaObject::invokeMethod(this, [this, newPart, fileName] {
                QModelIndex currentIndex = ui->treeView->currentIndex();
                ModelPart* parentPart = currentIndex.isValid() ? static_cast<ModelPart*>(currentIndex.internalPointer()) : partList->getRootItem();
                parentPart->appendChild(newPart);

                QAbstractItemModel* model = ui->treeView->model();
                model->dataChanged(model->index(newPart->row(), 0), model->index(newPart->row(), model->columnCount() - 1));
                emit model->dataChanged(model->index(newPart->row(), 0), model->index(newPart->row(), model->columnCount() - 1));
                emit model->layoutChanged();

                updateRender();
                addFloor(); // Re-add the floor every time the scene is updated
                emit statusUpdateMessage(QString("Loaded STL file: %1").arg(fileName), 5000);
            }, Qt::QueuedConnection);
    });
}




/**
 * @brief Slot triggered to handle the creation of a new group.
 *
 * Opens a dialog to input the name for the new group. Once confirmed, it adds a new
 * group ModelPart to the current selection or to the root if nothing is selected.
 */
void MainWindow::on_actionNewGroup_triggered() {
    newGroupDialog = new NewGroupDialog(this);
    QModelIndex index = ui->treeView->currentIndex();
    connect(newGroupDialog, &NewGroupDialog::accepted, [this, index]() {
        QString groupName = newGroupDialog->getGroupName();
        ModelPart* parentPart = index.isValid() ? static_cast<ModelPart*>(index.internalPointer()) : this->partList->getRootItem();
        ModelPart* newGroup = new ModelPart({ groupName, "true", "255,255,255" });
        parentPart->appendChild(newGroup);
        ui->treeView->model()->layoutChanged();
        });

    newGroupDialog->show();
}

/**
 * @brief Slot triggered to delete the currently selected file/part.
 *
 * Removes the selected item from the model and the tree view, updating the renderer
 * accordingly. If no item is selected, no action is taken.
 */
void MainWindow::on_actionDeleteFile_triggered() {
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Selection Error"), tr("Please select an item to delete."));
        return;
    }

    ModelPartList* model = qobject_cast<ModelPartList*>(ui->treeView->model());
    if (!model) {
        return;
    }

    ModelPart* selectedItem = static_cast<ModelPart*>(currentIndex.internalPointer());
    if (!selectedItem) {
        return;
    }

    if (selectedItem->data(0).toString() == "Model") {
        QMessageBox::warning(this, tr("Invalid Operation"), tr("Cannot delete root item."));
        return;
    }

    auto response = QMessageBox::question(this, tr("Confirm Deletion"),
        tr("Are you sure you want to delete this item?"),
        QMessageBox::Yes | QMessageBox::No);

    if (response == QMessageBox::Yes) {
        removeActorsRecursively(selectedItem);
        if (model->removeRows(currentIndex.row(), 1, currentIndex.parent())) {
            emit statusUpdateMessage("Item deleted successfully.", 5000);
        }
        else {
            emit statusUpdateMessage("Error deleting item.", 5000);
        }
    }
}

/**
 * @brief Recursively removes vtkActor objects from the renderer for a ModelPart and its descendants.
 *
 * Iterates over the ModelPart hierarchy, removing each associated vtkActor from the renderer.
 * It ensures the graphical representation is consistent with the tree view's structure.
 *
 * @param part The ModelPart to start removal from; does nothing if null.
 */
void MainWindow::removeActorsRecursively(ModelPart* part) {
    if (!part) return;

    vtkSmartPointer<vtkActor> actor = part->getActor();
    if (actor) {
        renderer->RemoveActor(actor);
    }

    for (int i = 0; i < part->childCount(); ++i) {
        removeActorsRecursively(part->child(i));
    }

    renderWindow->Render();
}


void MainWindow::on_actionSearchItem_triggered() {
    bool ok;
    QString searchTerm = QInputDialog::getText(this, tr("Search in TreeView"),
        tr("Enter search term:"), QLineEdit::Normal,
        QString(), &ok);
    if (ok && !searchTerm.isEmpty()) {
        QModelIndex searchResult = searchInTreeView(searchTerm, QModelIndex()); // Start from the root

        if (searchResult.isValid()) {
            selectItemInTreeView(searchResult);
        }
        else {
            QMessageBox::information(this, tr("Search Result"), tr("Item not found."));
        }
    }
}

QModelIndex MainWindow::searchInTreeView(const QString& searchString, const QModelIndex& parentIndex) {
    for (int r = 0; r < partList->rowCount(parentIndex); ++r) {
        QModelIndex index = partList->index(r, 0, parentIndex); // Assuming the name is in the first column
        QString itemText = partList->data(index, Qt::DisplayRole).toString();

        if (itemText.contains(searchString, Qt::CaseInsensitive)) {
            return index; // Item found
        }

        // Search in child nodes
        QModelIndex childResult = searchInTreeView(searchString, index);
        if (childResult.isValid()) {
            return childResult; // Item found in children
        }
    }

    return QModelIndex(); // Not found
}

void MainWindow::selectItemInTreeView(const QModelIndex& index) {
    ui->treeView->setCurrentIndex(index);
    ui->treeView->scrollTo(index);
    ui->treeView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::addFloor() {
    vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
    planeSource->Update();

    double scale = 500.0; // Adjust the scale as needed

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(50.0, 50.0, -10.0); // Adjust positioning if needed
    transform->RotateX(0); // Rotating 90 degrees around the X-axis
    transform->Scale(scale, scale, 1); // Scaling the plane

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(planeSource->GetOutputPort());
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.8, 0.8, 0.8); // Set the floor color

    renderer->AddActor(actor);
}







