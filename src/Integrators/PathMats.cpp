#include<nori/integrator.h>


NORI_NAMESPACE_BEGIN

class PathMats : public Integrator {
public:
    PathMats(const PropertyList& props) { }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& _ray) const {
        Intersection its;
        Color3f lo(0.f);
        Color3f throughout(1.f);
        float eta = 1.f;
        const int MAX_BOUNCE = 50;
        Ray3f ray(_ray);
        for (int b = 0; b < MAX_BOUNCE && scene->rayIntersect(ray, its); ++b) {

            if (its.mesh->isEmitter() && Frame::cosTheta(its.toLocal(-ray.d)) > 0) {
                lo += its.mesh->getEmitter()->getRadiance() * throughout;
            }

            float lightPDF;

            BSDFQueryRecord bQR(its.toLocal(-ray.d));
            throughout *= its.mesh->getBSDF()->sample(bQR, sampler->next2D());
            eta *= bQR.eta;

            if (b > 3) {
                float probability = fmin(throughout.maxCoeff() * eta * eta, 0.99f);
                if (sampler->next1D() > probability) break;
                throughout /= probability;
            }

            ray = Ray3f(its.p, its.toWorld(bQR.wo));
        }
        return lo;
    }

    std::string toString() const {
        return "Path_Mat_Integrator[]";
    }
};



NORI_REGISTER_CLASS(PathMats, "path_mats");

NORI_NAMESPACE_END