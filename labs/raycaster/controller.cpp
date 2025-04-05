#include "controller.h"

const std::vector<Polygon> &Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon &polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF &new_vertex) {
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF &new_vertex) {
    polygons_.back().UpdateLastVertex(new_vertex);
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF &light_source) {
    light_source_ = light_source;
}