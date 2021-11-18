#include <nori/emitter.h>

NORI_NAMESPACE_BEGIN

class AreaLight : public Emitter{
public:
    AreaLight(const PropertyList& props){
        radiance = props.getColor("radiance");
    }

    Color3f eval(const EmitterQueryRecord& eRec) const{
        Vector3f light2Point = eRec.shadingPoint - eRec.lightPoint;
        float d2 = light2Point.squaredNorm();
        light2Point.normalize();
        float cosLight = std::max(light2Point.dot(eRec.lightNormal), 0.f);
        return radiance * cosLight / d2;
    }

    Color3f getRadiance() const{
        return radiance;
    }

    std::string toString() const{
        return tfm::format("AreaLight:[Radiance:\"%s\"]", radiance);
    }



protected:
    Color3f radiance;
};

NORI_REGISTER_CLASS(AreaLight, "area");

NORI_NAMESPACE_END

