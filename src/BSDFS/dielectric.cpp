/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/bsdf.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
public:
    Dielectric(const PropertyList &propList) {
        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);
    }

    Color3f eval(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    float pdf(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
        float cosine = Frame::cosTheta(bRec.wi);

        //fresnel's term
        float Fr = fresnel(cosine, m_extIOR, m_intIOR);
        bRec.measure = EDiscrete;

        if (sample.x() < Fr) {
            bRec.wo = Vector3f(
                -bRec.wi.x(),
                -bRec.wi.y(),
                bRec.wi.z()
            );
            bRec.eta = 1.f;
            return Color3f(1.0f);
        }
        else {
            float eta = cosine < 0 ? m_intIOR / m_extIOR : m_extIOR / m_intIOR;
            Normal3f n = cosine < 0 ? Normal3f(0.f, 0.f, -1.f) : Normal3f(0.f, 0.f, 1.f);
            cosine = abs(cosine);
            float sine2T = eta * eta * std::max(0.f, 1.f - cosine * cosine);
            float cosineT = std::sqrt(1 - sine2T);
            bRec.wo = eta * -bRec.wi + (eta * cosine - cosineT) * n;
            bRec.wo.normalize();
            bRec.eta = eta;
            return Color3f(eta * eta);
        }
    }

    std::string toString() const {
        return tfm::format(
            "Dielectric[\n"
            "  intIOR = %f,\n"
            "  extIOR = %f\n"
            "]",
            m_intIOR, m_extIOR);
    }


private:
    float m_intIOR, m_extIOR;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
NORI_NAMESPACE_END
