/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include <nori/object.h>

NORI_NAMESPACE_BEGIN

    /**
     * \brief Convenience data structure used to pass multiple parameters
     *        to the evalution and sampling routines in Emitter.
     * */
    struct EmitterQueryRecord {
        Point3f shadingPoint, lightPoint;
        Normal3f shadingNormal, lightNormal;

        EmitterQueryRecord(Point3f &sp, Point3f &lp, Normal3f &sn, Normal3f &ln) : shadingPoint(sp), lightPoint(lp),
                                                                                   shadingNormal(sn), lightNormal(ln) {}

    };

    /**
     * \brief Superclass of all emitters
     */
    class Emitter : public NoriObject {
    public:

    /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.)
     * provided by this instance
     * */
    EClassType getClassType() const { return EEmitter; }

    /**
     * \brief
     * \return emitter's radiance
     * */
    virtual Color3f getRadiance() const = 0;

    /**
     * \brief
     * \param eRec
     *      Emitter query record
     * \return the radiance emitter send to shading point
     * */
    virtual Color3f eval(const EmitterQueryRecord& eRec) const = 0;

    };


NORI_NAMESPACE_END
