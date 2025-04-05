#include "polygon.h"

Polygon::Polygon(const std::vector<QPointF> &vertices) : vertices_(vertices) {
}

const std::vector<QPointF> &Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF &vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF &new_vertex) {
    vertices_.back() = new_vertex;
}
