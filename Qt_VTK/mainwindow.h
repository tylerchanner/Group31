/**
 * @file mainwindow.h
 * @brief Header file for the MainWindow class in a Qt application.
 *
 * Declares the MainWindow class, which initializes the application's main window and manages UI interactions.
 * It sets up a model-based tree view and handles button clicks to display messages in the status bar,
 * along with managing various actions related to the application's functionality.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include "ModelPartList.h" 
#include "ModelPart.h" 
#include "NewGroupDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Manages the main window and its UI components.
 *
 * Integrates UI setup, tree view model initialization, and user interaction handlers,
 * such as button clicks and action triggers, to manage application components like
 * rendering views, dialogs, and model part properties.
 */
    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
    void applyPropertiesToPart(ModelPart* part, const QString& name, bool visibility, const QColor& color, bool updateName = true);
    void updateChildrenProperties(ModelPart* part, bool visibility, const QColor& color);
    void initializePartList();
    void setupTreeView();
    void setupActions();
    void setupRenderer();
    void connectSignals();
    void addModelPartToTree();
    void createAction(QAction** action, const QString& text, void (MainWindow::* slot)());
    QModelIndex searchInTreeView(const QString& searchString, const QModelIndex& parentIndex);
    void selectItemInTreeView(const QModelIndex& index);
signals:
    void statusUpdateMessage(const QString& message, int timeout);

public slots:
    void handleTreeClicked();
    void on_actionOpen_File_triggered();
    void on_actionItemOptions_triggered();
    void on_actionNewGroup_triggered();
    void on_actionDeleteFile_triggered();
    void createModelPartFromFile(const QString& fileName);
    void removeActorsRecursively(ModelPart* part);
    void on_actionSearchItem_triggered();
    void addFloor();

private:
    Ui::MainWindow* ui; ///< User interface for the main window.
    ModelPartList* partList; ///< List of model parts displayed in the tree view.
    vtkSmartPointer<vtkRenderer> renderer; ///< Renderer for displaying VTK objects.
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow; ///< OpenGL render window for VTK rendering.
    vtkSmartPointer<vtkActor> floorActor;
    QAction* actionNewGroup; ///< Action to create a new group in the tree view.
    NewGroupDialog* newGroupDialog; ///< Dialog for creating new groups.
    QAction* actionDeleteGroup; ///< Action to delete a selected group.
    QAction* actionItemOptions; ///< Action to modify item options.
    QAction* actionDeleteItem; ///< Action to delete a selected item.
    QAction* actionSearch_Items;
};

#endif // MAINWINDOW_H
