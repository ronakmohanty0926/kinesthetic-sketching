#pragma once

#include <Core.h>

namespace midl {

    enum AABB_INTERSECTION
    {
        DISJOINT,
        OVERLAPS_WITH,
        CONTAINS,
        CONTAINED_IN,

    };

    class AABB2
    {
    private:
        float xRng[2];
        float yRng[2];

    public:
        AABB2();
        ~AABB2();

        bool Compute(vector<float> &points);
        bool Compute(vector<Tuple2f> &points);

        void Get(float *xRng,float *yRng);
        void GetCentroid(float *centroid);
        void GetCorners(Tuple2f corners[4]);
        bool IsPointInside(float *p);
        int RelationTo(AABB2 &box);
    };

    class AABB3
    {
    private:
        float xRng[2];
        float yRng[2];
        float zRng[2];

    public:
        AABB3();
        ~AABB3();

        bool Compute(vector<float> &points);
        bool Compute(vector<Tuple3f> &points);

        void Get(float *xRng, float *yRng, float *zRng);
        void GetCentroid(float *centroid);
        void GetCorners(Tuple3f corners[8]);
        bool IsPointInside(float *p);
        int RelationTo(AABB3 &box);
    };

    // TO BE IMPLEMENTED: AABBTree2, AABBTree3
}
