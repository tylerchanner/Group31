/**
 * @file ModelPartList.h
 *
 * Defines the ModelPartList class, which serves as a custom model to represent a hierarchical structure
 * of model parts in a tree view within a Qt application. This class is designed to interface with Qt's
 * model-view framework, enabling the visualization and interaction with a tree-like structure of model
 * components, each represented by a ModelPart instance.
 */

#ifndef VIEWER_MODELPARTLIST_H
#define VIEWER_MODELPARTLIST_H

#include "ModelPart.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>

 /**
  * @class ModelPartList
  * @brief A custom item model that represents a list of model parts in a hierarchical structure.
  *
  * Inherits from QAbstractItemModel and is designed to represent and manage a tree of ModelPart objects.
  * This class provides the necessary implementations to interface with Qt's view components, facilitating
  * the display and manipulation of a tree or list of ModelPart objects within the application.
  */
class ModelPartList : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ModelPartList(const QString& data, QObject* parent = nullptr);
    ~ModelPartList();

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    ModelPart* getRootItem();
    ModelPart* getItem(const QModelIndex& index) const;
    QModelIndex appendChild(QModelIndex& parent, const QList<QVariant>& data);
    bool removeRows(int position, int rows, const QModelIndex& parentIndex = QModelIndex());

private:
    ModelPart* rootItem; ///< Pointer to the root item of the model tree.
};

#endif // VIEWER_MODELPARTLIST_H
