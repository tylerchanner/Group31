/**
 * @file ModelPart.cpp
 * Includes the definition of the ModelPart class and the necessary VTK libraries
 * to handle 3D model parts, particularly for rendering STL files.
 */

#include "ModelPart.h"
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>

 /**
  * Constructor for the ModelPart class.
  * Initializes a model part with the given data and parent.
  *
  * @param data A list of QVariant items to initialize the item's data.
  * @param parent The parent ModelPart, nullptr if it's the root.
  */
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent), isVisible(false) {
}

/**
 * Destructor for the ModelPart class.
 * Cleans up the child items by deleting them.
 */
ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}

/**
 * Appends a child ModelPart to this model part.
 *
 * @param item The child ModelPart to append.
 */
void ModelPart::appendChild(ModelPart* item) {
    item->m_parentItem = this;
    m_childItems.append(item);
}

/**
 * Retrieves the child item at the specified row.
 *
 * @param row The index of the child to retrieve.
 * @return The child at the specified row, or nullptr if out of range.
 */
ModelPart* ModelPart::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

/**
 * Counts the number of child items.
 *
 * @return The number of child items.
 */
int ModelPart::childCount() const {
    return m_childItems.count();
}

/**
 * Counts the number of columns of data.
 *
 * @return The number of columns of item data.
 */
int ModelPart::columnCount() const {
    return m_itemData.count();
}

/**
 * Retrieves the data at the specified column.
 *
 * @param column The column from which to retrieve the data.
 * @return The data stored in the column.
 */
QVariant ModelPart::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData[column];
}

/**
 * Sets the data at a specified column.
 *
 * @param column The column where the data is to be set.
 * @param value The data value to set.
 */
void ModelPart::set(int column, const QVariant& value) {
    if (column >= 0 && column < m_itemData.size()) {
        m_itemData[column] = value;
    }
}

/**
 * Gets the parent item of this model part.
 *
 * @return The parent ModelPart.
 */
ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

/**
 * Determines the row index of this item in the parent's child list.
 *
 * @return The index of this item.
 */
int ModelPart::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

/**
 * Removes a range of children from the item.
 *
 * @param position The starting index from where children will be removed.
 * @param count The number of children to remove.
 */
void ModelPart::removeChildren(int position, int count) {
    for (int row = 0; row < count; ++row) {
        delete m_childItems.takeAt(position);
    }
}

/**
 * Sets the color of the model part.
 *
 * @param R Red component of the color.
 * @param G Green component of the color.
 * @param B Blue component of the color.
 */
void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    color = QColor(R, G, B);
}

/**
 * Gets the red component of the model part's color.
 *
 * @return The red component value.
 */
unsigned char ModelPart::getColourR() const {
    return static_cast<unsigned char>(color.red());
}

/**
 * Gets the green component of the model part's color.
 *
 * @return The green component value.
 */
unsigned char ModelPart::getColourG() const {
    return static_cast<unsigned char>(color.green());
}

/**
 * Gets the blue component of the model part's color.
 *
 * @return The blue component value.
 */
unsigned char ModelPart::getColourB() const {
    return static_cast<unsigned char>(color.blue());
}


QColor ModelPart::getColor() const {
    return QColor(getColourR(), getColourG(), getColourB());
}
/**
 * Sets the visibility of the model part.
 *
 * @param isVisible Boolean indicating whether the part is visible.
 */
void ModelPart::setVisible(bool isVisible) {
    this->isVisible = isVisible;
}

/**
 * Returns the visibility status of the model part.
 *
 * @return True if visible, false otherwise.
 */
bool ModelPart::visible() {
    return isVisible;
}

/**
 * Loads an STL file and creates the associated VTK actor for rendering.
 *
 * @param fileName The path to the STL file.
 */
void ModelPart::loadSTL(QString fileName) {
    vtkNew<vtkSTLReader> reader;
    reader->SetFileName(fileName.toStdString().c_str());
    reader->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    this->actor = actor;
}

/**
 * Retrieves the VTK actor associated with this model part.
 *
 * @return The VTK actor for this model part.
 */
vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return this->actor;
}

/**
 * Creates and returns a new VTK actor based on the current model data.
 * Useful for creating duplicate representations of the model part.
 *
 * @return A new VTK actor, or nullptr if the original actor or reader is not set.
 */
vtkSmartPointer<vtkActor> ModelPart::getNewActor() {
    if (!this->actor || !this->reader) {
        return nullptr;
    }

    vtkSmartPointer<vtkPolyDataMapper> newMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    newMapper->SetInputConnection(this->reader->GetOutputPort());

    vtkSmartPointer<vtkActor> newActor = vtkSmartPointer<vtkActor>::New();
    newActor->SetMapper(newMapper);

    if (this->actor->GetProperty()) {
        newActor->GetProperty()->DeepCopy(this->actor->GetProperty());
    }

    return newActor;
}

/**
 * Removes a single child from the model part at the specified position.
 *
 * @param position The index of the child to remove.
 */
void ModelPart::removeChild(int position) {
    if (position < 0 || position >= m_childItems.size())
        return;

    delete m_childItems.takeAt(position);
}
