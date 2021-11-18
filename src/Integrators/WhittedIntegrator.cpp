#include <nori/integrator.h>


NORI_NAMESPACE_BEGIN

class WhittedIntegrator : public Integrator {
public:
    WhittedIntegrator(const PropertyList& props) {

    }

    Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        if (its.mesh->getBSDF()->isDiffuse()) {
            //random numbers
            const Point2f rand2 = sampler->next2D();

            //choose a area light randomly
            float light_pdf;
            const Mesh* sample_mesh = scene->sampleEmittedLight(sampler, light_pdf);

            //if hit a emitted mesh => add the emit radiance
            Color3f le(0.0f);
            if (its.mesh->isEmitter())
                le += its.mesh->getEmitter()->getRadiance();

            //calculate the total
            Color3f L;
            const Emitter* emitter = sample_mesh->getEmitter();

            //importance sampling from area light
            Normal3f lightN;
            float pdf;
            Point3f lightpoint = sample_mesh->squareToUniformMesh(sampler, lightN, pdf);
            Vector3f wi = lightpoint - its.p;
            float d = wi.norm();
            wi.normalize();


            //Important!!!
            ///Todo: study pbrt and copy the epsilon solution.
            Ray3f shadowRay(its.p, wi, Epsilon, d - Epsilon);

            //float v = scene->rayIntersect(shadowRay) ? 0.0f : 1.0f;
            if (scene->rayIntersect(shadowRay))
                return le;

            wi.normalized();

            Color3f brdf = its.mesh->getBSDF()->eval(BSDFQueryRecord(its.toLocal(wi), its.toLocal(-ray.d), ESolidAngle));
            float cosine = std::fmax(wi.dot(its.shFrame.n), 0.0f);
            Color3f GxLE = cosine * emitter->eval(EmitterQueryRecord(its.p, lightpoint, its.shFrame.n, lightN));

            L = le + brdf * GxLE / pdf / light_pdf;
            return L;
        }
        else {
            if (sampler->next1D() < 0.95f) {
                BSDFQueryRecord bqr(its.toLocal(-ray.d));
                Color3f brdf = its.mesh->getBSDF()->sample(bqr, sampler->next2D());

                return 1.0f / 0.95f * brdf * Li(scene, sampler, Ray3f(its.p, its.toWorld(bqr.wo)));
            }
            else {
                return Color3f(0.0f);
            }
        }

    }

    std::string toString() const {
        return tfm::format("WhittedIntegrator[]");
    }

};


NORI_REGISTER_CLASS(WhittedIntegrator, "whitted");

NORI_NAMESPACE_END