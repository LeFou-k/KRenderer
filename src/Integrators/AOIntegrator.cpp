#include<nori/integrator.h>


NORI_NAMESPACE_BEGIN

class AOIntegrator : public Integrator {
public:
    AOIntegrator(const PropertyList& props) {

    }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Vector3f wi = Warp::squareToCosineHemisphere(sampler->next2D());
        Vector3f shadowRayDir = its.shFrame.toWorld(wi).normalized();
        float pdf = Warp::squareToCosineHemispherePdf(wi);
        float li = 1.0f * shadowRayDir.dot(its.shFrame.n.normalized()) * INV_PI / pdf;
        return scene->rayIntersect(Ray3f(its.p, shadowRayDir)) ? 0.0f : Color3f(clamp(li, 0.0f, 1.0f));
    }

    std::string toString() const {
        return tfm::format("AOIntegrator[]");
    }
};



NORI_REGISTER_CLASS(AOIntegrator, "ao");

NORI_NAMESPACE_END