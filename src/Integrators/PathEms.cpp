#include <nori/integrator.h>

NORI_NAMESPACE_BEGIN


class PathEms : public Integrator {
public:
    PathEms(const PropertyList& props) {};

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& _ray) const {
        Intersection its;
        Color3f lo(0.f), throughout(1.f);
        float eta = 1.f;
        Ray3f ray(_ray);
        bool countEmitter = true;

        for (int b = 0; scene->rayIntersect(ray, its); ++b) {
            if (its.mesh->isEmitter() && Frame::cosTheta(its.toLocal(-ray.d)) > 0 && countEmitter) {
                lo += its.mesh->getEmitter()->getRadiance() * throughout;
            }


            if (its.mesh->getBSDF()->isDiffuse()) {
                //importance sampling
                //sample a light and get emitter
                float lightPDF;
                const Mesh* lightMesh = scene->sampleEmittedLight(sampler, lightPDF);
                const Emitter* emitter = lightMesh->getEmitter();
                //get shadowRay
                Normal3f lightN;
                float pdf;
                Point3f lightpoint = lightMesh->squareToUniformMesh(sampler, lightN, pdf);
                Vector3f wi = lightpoint - its.p;
                float d = wi.norm();
                wi.normalize();
                Ray3f shadowRay(its.p, wi, Epsilon, d - Epsilon);

                //visible from light?
                if (!scene->rayIntersect(shadowRay)) {
                    float cosine = std::fmax(wi.dot(its.shFrame.n), 0.f);
                    Color3f brdf = its.mesh->getBSDF()->eval(BSDFQueryRecord(its.toLocal(-ray.d), its.toLocal(wi), ESolidAngle));
                    lo += cosine * brdf * emitter->eval(EmitterQueryRecord(its.p, lightpoint, its.shFrame.n, lightN)) * throughout / lightPDF / pdf;
                }
                countEmitter = false;
            }
            else {
                countEmitter = true;
            }


            BSDFQueryRecord bQR(its.toLocal(-ray.d));
            throughout *= its.mesh->getBSDF()->sample(bQR, sampler->next2D());
            eta *= bQR.eta;

            if (b > 3) {
                //float probability = fmin(throughout.maxCoeff() * eta * eta, 0.99f);
                float probability = fmin(throughout.maxCoeff() * eta * eta, 0.99f);

                if (sampler->next1D() > probability) break;
                throughout /= probability;
            }
            ray = Ray3f(its.p, its.toWorld(bQR.wo));
        }
        return lo;
    }

    std::string toString() const {
        return "Path_Ems_Integrator[]";
    }
};



NORI_REGISTER_CLASS(PathEms, "path_ems");

NORI_NAMESPACE_END