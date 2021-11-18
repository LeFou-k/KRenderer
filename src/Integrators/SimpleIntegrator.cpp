#include<nori/integrator.h>

NORI_NAMESPACE_BEGIN

class SimpleIntegrator : public Integrator {
public:
    SimpleIntegrator(const PropertyList& props) {
        lightPos = props.getPoint("position");
        lightE = props.getColor("energy");
    }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);
        Vector3f shadowRayDir = lightPos - its.p;
        Ray3f shadowRay(its.p, shadowRayDir.normalized());

        float cosine = shadowRayDir.normalized().dot(its.shFrame.n.normalized());
        float v = scene->rayIntersect(shadowRay) ? 0.0f : 1.0f;
        float d = shadowRayDir.norm();
        Color3f li = lightE * std::fmax(0.0f, cosine) * v * INV_PI * INV_PI / (4 * d * d);
        return li.clamp();
    }

    std::string toString() const {
        return tfm::format(
            "SimpleIntegrator["
            "     light position = \"%s\","
            "     light energy = \"%s\""
            "]", lightPos, lightE);
    }

protected:
    Point3f lightPos;
    Color3f lightE;
};

NORI_REGISTER_CLASS(SimpleIntegrator, "simple")

NORI_NAMESPACE_END