#include "controller.h"
#include <algorithm>
#include <QDebug>

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

std::vector<QPointF> Controller::GetLightSources() const {
    return light_sources_;
}

void Controller::SetLightSources() {
    const int num_additional_sources = 10; // Количество дополнительных источников света
    const double radius = 10.0; // Радиус окружности (небольшой, чтобы источники были близко)
    const double angle_step = 2 * M_PI / num_additional_sources; // Угол между источниками

    // Очищаем текущий список источников света
    light_sources_.clear();

    // Добавляем основной источник света
    light_sources_.push_back(light_source_);

    // Добавляем дополнительные источники света на окружности
    for (int i = 0; i < num_additional_sources; ++i) {
        double angle = i * angle_step;
        double x = light_source_.x() + radius * std::cos(angle);
        double y = light_source_.y() + radius * std::sin(angle);
        light_sources_.push_back(QPointF(x, y));
    }
}

void Controller::ClearPolygons() {
    polygons_.clear();
}

std::vector<Ray> Controller::CastRays(QPointF light_source) {
    std::vector<Ray> rays = {};
    for (auto polygon: polygons_) {
        for (auto vertex: polygon.GetVertices()) {
            Ray ray1 = Ray{light_source, vertex};
            //Ray ray2 = ray1.Rotate(0.1);
            Ray ray2 = ray1.Rotate(0.0001);
            //Ray ray3 = ray1.Rotate(2 * M_PI - 0.1);
            Ray ray3 = ray1.Rotate(-0.0001);
            rays.push_back(ray1);
            rays.push_back(ray2);
            rays.push_back(ray3);
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray> *rays) {
    for (auto& ray: *rays) {
        std::vector<QPointF> vertices(0);
        for (auto polygon: polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection.has_value()) {
                vertices.push_back(intersection.value());
            }
        }
        QPointF new_end = {100000, 1000000};
        for (int i = 0; i < vertices.size(); i++) {
            if (Dst(light_source_, vertices[i]) <= Dst(light_source_, new_end)) {
                new_end = vertices[i];
            }
        }
        ray.SetEnd(new_end);
    }
    /*for (const auto& ray : *rays) {
        qDebug() << "{" << ray.GetEnd().x() << " " << ray.GetEnd().y() << "} ";
    }
    qDebug() << "\n";*/
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (!rays || rays->empty()) {
        return;
    }

    const double distance_threshold = 2.0;
    std::vector<Ray> filtered_rays;

    std::vector<bool> used(rays->size(), false);

    for (size_t i = 0; i < rays->size(); ++i) {
        if (used[i]) {
            continue;
        }

        filtered_rays.push_back((*rays)[i]);
        used[i] = true;

        for (size_t j = i + 1; j < rays->size(); ++j) {
            if (used[j]) {
                continue;
            }

            double dist = Dst((*rays)[i].GetEnd(), (*rays)[j].GetEnd());
            if (dist < distance_threshold) {
                used[j] = true;
            }
        }
    }

    *rays = std::move(filtered_rays);
}

Polygon Controller::CreateLightArea(QPointF light_source) {
    std::vector<Ray> rays = CastRays(light_source);
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);
    std::sort(rays.begin(), rays.end(), [](const Ray &ray1, const Ray &ray2) {return ray1.GetAngle() < ray2.GetAngle();});
    std::vector<QPointF> vertices(0);
    for (auto ray: rays) {
        vertices.push_back(ray.GetEnd());
    }
    return Polygon(vertices);
}
