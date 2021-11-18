/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class Microfacet : public BSDF {
public:
    Microfacet(const PropertyList &propList) {
        /* RMS surface roughness */
        m_alpha = propList.getFloat("alpha", 0.1f);

        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);

        /* Albedo of the diffuse base material (a.k.a "kd") */
        m_kd = propList.getColor("kd", Color3f(0.5f));

        /* To ensure energy conservation, we must scale the 
           specular component by 1-kd. 

           While that is not a particularly realistic model of what 
           happens in reality, this will greatly simplify the 
           implementation. Please see the course staff if you're 
           interested in implementing a more realistic version 
           of this BRDF. */
        m_ks = 1 - m_kd.maxCoeff();
    }


    float GxShadowMaskingTerm(const Vector3f& w1, const Vector3f& wh) const {
        float b = 1.f / (m_alpha * Frame::tanTheta(w1));
        if (w1.dot(wh) / Frame::cosTheta(w1) <= 0.f)
            return 0.f;
        float b2 = b * b;
        return b < 1.6f ? (3.535f * b + 2.181f * b2) / (1.f + 2.276 * b + 2.577 * b2) : 1.f;
    }
    /// Evaluate the BRDF for the given pair of directions
    Color3f eval(const BSDFQueryRecord& bRec) const {
        if (bRec.measure != ESolidAngle || Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return Color3f(0.f);
        Vector3f h = bRec.wi + bRec.wo;
        h.normalize();
        float cosThetaI = Frame::cosTheta(bRec.wi), cosThetaO = Frame::cosTheta(bRec.wo), cosThetaH = Frame::cosTheta(h);
        float D = Warp::squareToBeckmannPdf(h, m_alpha);
        float Fr = fresnel(h.dot(bRec.wi), m_extIOR, m_intIOR);
        float G = GxShadowMaskingTerm(bRec.wi, h) * GxShadowMaskingTerm(bRec.wo, h);
        
        return m_kd * INV_PI + (m_ks * D * Fr * G ) / (4.f * cosThetaI * cosThetaO * cosThetaH);

    }

    /// Evaluate the sampling density of \ref sample() wrt. solid angles
    float pdf(const BSDFQueryRecord &bRec) const {
        if (bRec.measure != ESolidAngle || Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return 0.f;
        Vector3f h = bRec.wi + bRec.wo;
        h.normalize();
        float Jh = 1.f / (4.0f * h.dot(bRec.wo));
        float cosThetaO = Frame::cosTheta(bRec.wo);
        float D = Warp::squareToBeckmannPdf(h, m_alpha);
        return m_ks * D * Jh + (1.f - m_ks) * cosThetaO * INV_PI;
    }

    /// Sample the BRDF
    Color3f sample(BSDFQueryRecord &bRec, const Point2f &_sample) const {
        if (Frame::cosTheta(bRec.wi) <= 0)
            return Color3f(0.f);
        bRec.measure = ESolidAngle;

        if (_sample.x() < m_ks) {
            Point2f sampleReuse(_sample.x() / m_ks, _sample.y());
            Normal3f n = Warp::squareToBeckmann(sampleReuse, m_alpha);
            bRec.wo = 2.f * bRec.wi.dot(n) * n - bRec.wi;
            bRec.wo.normalize();
        }
        else {
            //diffuse
            Point2f sampleReuse((_sample.x() - m_ks) / (1.f - m_ks), _sample.y());
            bRec.wo = Warp::squareToCosineHemisphere(sampleReuse);
        }
        bRec.eta = m_extIOR / m_intIOR;
        
        //Attention: 
        //After you sample the output wo, 
        //exclude the wo out of the range of hemisphere
        if (Frame::cosTheta(bRec.wo) < 0.f) 
            return Color3f(0.f);

        return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);
        // Note: Once you have implemented the part that computes the scattered
        // direction, the last part of this function should simply return the
        // BRDF value divided by the solid angle density and multiplied by the
        // cosine factor from the reflection equation, i.e.
        // return eval(bRec) * Frame::cosTheta(bRec.wo) / pdf(bRec);
    }

    bool isDiffuse() const {
        /* While microfacet BRDFs are not perfectly diffuse, they can be
           handled by sampling techniques for diffuse/non-specular materials,
           hence we return true here */
        return true;
    }

    std::string toString() const {
        return tfm::format(
            "Microfacet[\n"
            "  alpha = %f,\n"
            "  intIOR = %f,\n"
            "  extIOR = %f,\n"
            "  kd = %s,\n"
            "  ks = %f\n"
            "]",
            m_alpha,
            m_intIOR,
            m_extIOR,
            m_kd.toString(),
            m_ks
        );
    }
private:
    float m_alpha;
    float m_intIOR, m_extIOR;
    float m_ks;
    Color3f m_kd;
};

NORI_REGISTER_CLASS(Microfacet, "microfacet");
NORI_NAMESPACE_END
