/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

float tentCDFInv(float p){
    return p < 0.5 ? sqrt(p * 2) - 1 : 1 - sqrt(2 - 2 * p);
}

Point2f Warp::squareToTent(const Point2f &sample) {
    return Point2f(tentCDFInv(sample.x()), tentCDFInv(sample.y()));
}

float Warp::squareToTentPdf(const Point2f &p) {
    return (1 - abs(p.x())) * (1 - abs(p.y()));
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    float r = sqrt(sample.x()), theta = 2 * M_PI * sample.y();
    return Point2f(r * cos(theta), r * sin(theta));
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    return p.x() * p.x() + p.y() * p.y() <= 1.0f ? 1.0f * INV_PI : 0.0f;
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    float phi = acos(1 - 2 * sample.x()), theta = 2 * M_PI * sample.y();
    return Vector3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    return v.x() * v.x() + v.y() * v.y() + v.z() * v.z() <= 1.0f ? 0.25 * INV_PI : 0.0f;
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    float phi = acos(1 - sample.x()), theta = 2 * M_PI * sample.y();
    return Vector3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    return v.x() * v.x() + v.y() * v.y() + v.z() * v.z() <= 1.0f && v.z() >= 0 ? 0.5 * INV_PI : 0.0f;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    float phi = asin(sqrt(sample.x())), theta = 2 * M_PI * sample.y();
    return Vector3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    return v.x() * v.x() + v.y() * v.y() + v.z() * v.z() <= 1.0f && v.z() >= 0 ? INV_PI * v.z() : 0.0f;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    //theta, phi exchange meaning
    float phi = 2 * M_PI * sample.y(), a2 = alpha * alpha, theta;
    if(isinf(log(1 - sample.x()))) theta = 0.0f;
    else theta = atan(sqrt(-a2 * log(1 - sample.x())));
    return Vector3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {

    //Attention: cosine theta cannot be zero!
    if(m.z() <= 0 + Epsilon) return 0.0f;
    float cosTheta = Frame::cosTheta(m), tanTheta = Frame::tanTheta(m);
    float a2 = alpha * alpha;
    float azi_part = 0.5 * INV_PI, long_part = 2 * exp(-tanTheta * tanTheta / a2) / (a2 * pow(cosTheta, 3));
    return azi_part * long_part;

}

NORI_NAMESPACE_END
