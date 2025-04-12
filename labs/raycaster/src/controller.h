#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QColor>
#include <vector>

class Controller {
public:
    [[nodiscard]] const std::vector<Polygon> &GetPolygons();

    void SetPolygon(int i, const Polygon &polygon);

    void AddPolygon(const Polygon &);

    void AddVertexToLastPolygon(const QPointF &new_vertex);

    void UpdateLastPolygon(const QPointF &new_vertex);

    void DeleteLastPolygon();

    [[nodiscard]] QPointF GetLightSource() const;

    void SetLightSource(const QPointF &light_source);

    [[nodiscard]] std::vector<QPointF> GetLightSources() const;

    void SetLightSources();

    void AddAdditionalLightSource(const QPointF &light_source, QColor color);

    [[nodiscard]] std::vector<std::pair<QPointF, QColor> > GetAdditionalLightSource() const;

    void DeleteLastAdditionalLightSource();

    void ClearPolygons();

    std::vector<Ray> CastRays(QPointF light_source);

    void IntersectRays(std::vector<Ray> *rays) const;

    void RemoveAdjacentRays(std::vector<Ray> *rays) const;

    Polygon CreateLightArea(QPointF light_source);

    void Clear();

private:
    std::vector<Polygon> polygons_ = {};
    std::vector<QPointF> light_sources_ = {};
    std::vector<std::pair<QPointF, QColor> > additional_sources_ = {};
    QPointF light_source_ = QPointF(0, 0);
};

#endif //CONTROLLER_H
