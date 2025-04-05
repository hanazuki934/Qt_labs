#include "ray.h"

Ray::Ray(const QPointF &begin, const QPointF &end, const double angle) : begin_(begin), end_(end), angle_(angle) {
}

QPointF Ray::GetBegin() const {
    return begin_;
}

QPointF Ray::GetEnd() const {
    return end_;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetBegin(const QPointF &begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPointF &end) {
    end_ = end;
}

void Ray::SetAngle(double angle) {
    angle_ = angle;
}
/*
Ray Ray::Rotate(const double delta_angle) const {
    const auto result = Ray(begin_, end_, angle_ + delta_angle);
    return result;
}*/