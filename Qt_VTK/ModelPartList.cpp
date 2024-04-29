/**
 * @file ModelPartList.cpp
 * @brief Implementation of the ModelPartList class.
 *
 * This file includes the definitions of the ModelPartList class, which
 * manages a list of model parts. It extends QAbstractItemModel to provide
 * a custom model for representing hierarchical data of model parts.
 */

#include "ModelPartList.h"
#include "ModelPart.h"
#include <QStandardItem>

 /**
  * @brief Constructor for ModelPartList.
  *
  * Initializes a new instance of the ModelPartList class using the provided data.
  *
  * @param data String data used for initialization.
  * @param parent Pointer to the parent QObject.
  */
ModelPartList::ModelPartList(const QString& data, QObject* parent) : QAbstractItemModel(parent) {
    rootItem = new ModelPart({ tr("Part"), tr("Visible?"), tr("Colour") });
}

/**
 * @brief Destructor for ModelPartList.
 *
 * Cleans up the resources used by the ModelPartList, particularly deleting the root item.
 */
ModelPartList::~ModelPartList() {
    delete rootItem;
}

/**
 * @brief Returns the number of columns for the children of the given parent.
 *
 * @param parent The index of the parent whose children are counted.
 * @return The number of columns under the given parent.
 */
int ModelPartList::columnCount(const QModelIndex& parent) const {
    return rootItem->columnCount();
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 *
 * @param index The index of the item to return the data for.
 * @param role The role for which data is requested.
 * @return The data stored under the given role for the item referred to by the index.
 */
QVariant ModelPartList::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    auto* item = static_cast<ModelPart*>(index.internalPointer());
    return item ? item->data(index.column()) : QVariant();
}

/**
 * @brief Returns the flags for the item at the given index.
 *
 * @param index The index of the item.
 * @return The item flags for the given index.
 */
Qt::ItemFlags ModelPartList::flags(const QModelIndex& index) const {
    return index.isValid() ? QAbstractItemModel::flags(index) : Qt::NoItemFlags;
}

/**
 * @brief Provides header data for the table.
 *
 * @param section The section of the header.
 * @param orientation The orientation of the header (horizontal or vertical).
 * @param role The role of the data.
 * @return The data for the given header section.
 */
QVariant ModelPartList::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

/**
 * @brief Creates an index in the model for a given row and column.
 *
 * @param row The row number.
 * @param column The column number.
 * @param parent The parent index.
 * @return The model index for the specified location.
 */
QModelIndex ModelPartList::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ModelPart* parentItem = getItem(parent);
    ModelPart* childItem = parentItem->child(row);
    return childItem ? createIndex(row, column, childItem) : QModelIndex();
}

/**
 * @brief Finds the parent of a given index.
 *
 * @param index The index to find the parent for.
 * @return The parent index of the given index.
 */
QModelIndex ModelPartList::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    ModelPart* childItem = getItem(index);
    ModelPart* parentItem = childItem ? childItem->parentItem() : nullptr;
    return parentItem == rootItem || !parentItem ? QModelIndex() : createIndex(parentItem->row(), 0, parentItem);
}

/**
 * @brief Returns the number of rows under the given parent.
 *
 * @param parent The parent index.
 * @return The number of rows under the parent.
 */
int ModelPartList::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() && parent.column() != 0)
        return 0;

    ModelPart* parentItem = getItem(parent);
    return parentItem ? parentItem->childCount() : 0;
}

/**
 * @brief Retrieves the root item of the model.
 *
 * @return Pointer to the root ModelPart item.
 */
ModelPart* ModelPartList::getRootItem() {
    return rootItem;
}

/**
 * @brief Appends a child to a given parent in the model.
 *
 * @param parent The parent index to which the child is appended.
 * @param data The data for the new child.
 * @return The index of the newly added child.
 */
QModelIndex ModelPartList::appendChild(QModelIndex& parent, const QList<QVariant>& data) {
    ModelPart* parentPart = getItem(parent);
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    ModelPart* childPart = new ModelPart(data, parentPart);
    parentPart->appendChild(childPart);
    endInsertRows();
    return createIndex(rowCount(parent) - 1, 0, childPart);
}

/**
 * @brief Removes a number of rows starting from a given position.
 *
 * @param position The position to start removing rows.
 * @param rows The number of rows to remove.
 * @param parentIndex The parent from which the rows will be removed.
 * @return True if the rows were successfully removed; otherwise false.
 */
bool ModelPartList::removeRows(int position, int rows, const QModelIndex& parentIndex) {
    ModelPart* parentItem = getItem(parentIndex);
    if (!parentItem || position < 0 || position + rows > parentItem->childCount())
        return false;

    beginRemoveRows(parentIndex, position, position + rows - 1);
    for (int row = 0; row < rows; ++row) {
        parentItem->removeChild(position);
    }
    endRemoveRows();

    return true;
}

/**
 * @brief Retrieves the item associated with a given index.
 *
 * @param index The index of the item to retrieve.
 * @return The item associated with the specified index.
 */
ModelPart* ModelPartList::getItem(const QModelIndex& index) const {
    if (!index.isValid())
        return rootItem;

    return static_cast<ModelPart*>(index.internalPointer());
}
