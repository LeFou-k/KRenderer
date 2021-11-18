#include<nori/integrator.h>



NORI_NAMESPACE_BEGIN

    //Multiple Importance Sampling
    class PathMis : public Integrator {
    public:
        PathMis(const PropertyList& props) {};
        Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& _ray) const {
            Intersection its;
            Color3f li(0.f), throughout(1.f);
            float eta = 1.f;
            Ray3f ray(_ray);
            bool hitNot = scene->rayIntersect(ray, its);
            if (hitNot && its.mesh->isEmitter() && Frame::cosTheta(its.toLocal(-ray.d)) > 0.f) {
                li += its.mesh->getEmitter()->getRadiance() * throughout;
            }

            for (int b = 0; hitNot; ++b) {
                //PDFs
                float chooseLightPDF, lightPDF, brdfPDF;

                //Randomly choose a light and record as chooseLightPDF
                const Mesh* lightMesh = scene->sampleEmittedLight(sampler, chooseLightPDF);
                const Emitter* emitter = lightMesh->getEmitter();

                //current mesh is diffuse:
                if (its.mesh->getBSDF()->isDiffuse()) {
                    //MIS:
                    //light weight part:
                    //calculate light sampling
                    Normal3f lightN;
       
                    Point3f lightpoint = lightMesh->squareToUniformMesh(sampler, lightN, lightPDF);
                    lightPDF *= chooseLightPDF;
                    Vector3f wo = lightpoint - its.p;
                    float d = wo.norm();
                    wo.normalize();
                    
                    BSDFQueryRecord diffuseBQR(its.toLocal(-ray.d), its.toLocal(wo), ESolidAngle);
                    brdfPDF = its.mesh->getBSDF()->pdf(diffuseBQR);

                    //convert lightpdf from unit area to solid angles
                    //inf here => clamp it!
                    float solidAnglePDF = clamp(lightPDF * d * d / abs((-wo).dot(lightN)), 0.f, 100000.f);


                    float w_light = solidAnglePDF / (solidAnglePDF + brdfPDF); // light weight
                    Ray3f shadowRay(its.p, wo, Epsilon, d - Epsilon);

                    if (!scene->rayIntersect(shadowRay)) {
                        float cos_wo_hitN = std::fmax(wo.dot(its.shFrame.n), 0.f);
                        Color3f light_fr = its.mesh->getBSDF()->eval(diffuseBQR);
                        li += w_light * cos_wo_hitN * light_fr * emitter->eval(EmitterQueryRecord(its.p, lightpoint, its.shFrame.n, lightN)) * throughout / lightPDF;
                    }
                }


                //whether diffuse or specular
                // you need to sample the brdf and calculate it.
                //calculate brdf sampling
                //sample brdf and get pdf
                BSDFQueryRecord bQR(its.toLocal(-ray.d));
                throughout *= its.mesh->getBSDF()->sample(bQR, sampler->next2D());
                
                //Next iteration info
                Vector3f wo = its.toWorld(bQR.wo);
                Ray3f nextRay(its.p, wo);
                Intersection nextIts;
                hitNot = scene->rayIntersect(nextRay, nextIts);
                

                //next hit mesh is a emitter light:

                //Problems: cosine theta here!!!

                if (hitNot && nextIts.mesh->isEmitter()) {
                    lightPDF = chooseLightPDF * nextIts.mesh->squareToUniformMeshPDF();

                    //clamp it:
                    float d = (nextIts.p - its.p).norm();
                    float cos_wo_lightN = (-wo).dot(nextIts.shFrame.n);
                    brdfPDF = its.mesh->getBSDF()->pdf(bQR);
                    float solidAnglePDF = clamp(lightPDF * d * d / abs(cos_wo_lightN), 0.f, 1000000.f);

                    float w_brdf = 1.f;

                    //Attention: inRadiance calculate with cos(wi, shadingNormal)
                    float cos_wo_N = std::max(wo.dot(its.shFrame.n), 0.f);
                    
                    

                    //brdfPDF is not zero => diffuse!
                    if (its.mesh->getBSDF()->isDiffuse()) {
                        w_brdf = brdfPDF / (brdfPDF + solidAnglePDF);
                    }

                    if(cos_wo_lightN > 0.f)
                        li += w_brdf * throughout * nextIts.mesh->getEmitter()->getRadiance();
                }

                its = nextIts;

                if (b > 3) {
                    float probility = fmin(throughout.maxCoeff() * eta * eta, 0.99f);
                    if (sampler->next1D() > probility) break;
                    throughout /= probility;
                }

                ray = nextRay;
            }
            return li;
        }

        std::string toString() const {
            return "Path_Mis_Integrator[]";
        }
    };


//bind integrator and xml tags:

    NORI_REGISTER_CLASS(PathMis, "path_mis");

NORI_NAMESPACE_END

