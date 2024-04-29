/**
 * @file ModelPart.h
 *
 * Provides the definition of the ModelPart class, which is integral for representing individual components
 * or parts of a 3D model in a hierarchical structure. Each ModelPart can have child parts, forming a tree-like
 * organization, and includes functionalities for loading, manipulating, and visualizing the part's data, especially
 * from STL files.
 */

#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QColor>
#include <memory>
#include <vector>
#include <vtkSmartPointer.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkSTLReader.h>
#include <vtkColor.h>

 /**
  * @class ModelPart
  * @brief Represents a part or component of a model.
  *
  * This class encapsulates a part or component of a 3D model, supporting hierarchical structuring,
  * visualization properties like color and visibility, and the ability to load geometrical data from STL files.
  */
class ModelPart {
public:
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);
    ~ModelPart();

    void appendChild(ModelPart* item);
    ModelPart* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void set(int column, const QVariant& value);
    ModelPart* parentItem();
    int row() const;
    void setColour(const unsigned char R, const unsigned char G, const unsigned char B);
    unsigned char getColourR() const;
    unsigned char getColourG() const;
    unsigned char getColourB() const;
    void setVisible(bool isVisible);
    bool visible();
    void loadSTL(QString fileName);
    void removeChild(int position);
    void removeChildren(int position, int count);
    vtkSmartPointer<vtkActor> getActor();
    vtkSmartPointer<vtkActor> getNewActor();
    QColor getColor() const;

private:
    QList<ModelPart*> m_childItems; ///< Child parts of this model part.
    QList<QVariant> m_itemData; ///< Data associated with this part, like name and visibility.
    ModelPart* m_parentItem; ///< Parent part of this model part.
    bool isVisible; ///< Visibility state of this part.
    QColor color; ///< Color of this part.
    vtkSmartPointer<vtkSTLReader> reader; ///< STL reader for loading geometrical data.
    vtkSmartPointer<vtkMapper> mapper; ///< Mapper for geometrical data.
    vtkSmartPointer<vtkActor> actor; ///< Actor for rendering.
};

#endif // VIEWER_MODELPART_H
