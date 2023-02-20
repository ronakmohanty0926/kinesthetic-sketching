#include "AABB.h"

namespace midl {
    // AABB2
    AABB2::AABB2()
    {
        xRng[0] = 0.0;
        xRng[1] = 0.0;
        yRng[0] = 0.0;
        yRng[1] = 0.0;
    }

    AABB2::~AABB2(){}

    bool AABB2::Compute(vector<float> &points)
    {
        if (points.size() == 0 ) return false;

        xRng[0] = FLT_MAX; xRng[1] = -FLT_MAX;
        yRng[0] = FLT_MAX; yRng[1] = -FLT_MAX;

        for (size_t i = 0; i < points.size(); i++)
        {
            if (points[2*i] < xRng[0]) xRng[0] = points[2*i];
            if (points[2*i] > xRng[1]) xRng[1] = points[2*i];

            if (points[2*i + 1] < yRng[0]) yRng[0] = points[2*i + 1];
            if (points[2*i + 1] > yRng[1]) yRng[1] = points[2*i + 1];
        }

        return true;
    }

    bool AABB2::Compute(vector<Tuple2f> &points)
    {
        if (points.size() == 0 ) return false;

        xRng[0] = FLT_MAX; xRng[1] = -FLT_MAX;
        yRng[0] = FLT_MAX; yRng[1] = -FLT_MAX;

        for (size_t i = 0; i < points.size(); i++) {
            if (points[i].data[0] < xRng[0]) xRng[0] = points[i].data[0];
            if (points[i].data[0] > xRng[1]) xRng[1] = points[i].data[0];

            if (points[i].data[1] < yRng[0]) yRng[0] = points[i].data[1];
            if (points[i].data[1] > yRng[1]) yRng[1] = points[i].data[1];
        }

        return true;
    }

    void AABB2::Get(float *xRng,float *yRng)
    {
        xRng[0] = this->xRng[0];
        xRng[1] = this->xRng[1];

        yRng[0] = this->yRng[0];
        yRng[1] = this->yRng[1];
    }

    void AABB2::GetCentroid(float *centroid)
    {
        centroid[0] = 0.5*(this->xRng[0] + this->xRng[1]);
        centroid[1] = 0.5*(this->yRng[0] + this->yRng[1]);
    }

    void AABB2::GetCorners(Tuple2f corners[4])
    {
        corners[0].data[0] = xRng[0];
        corners[0].data[1] = yRng[0];

        corners[1].data[0] = xRng[0];
        corners[1].data[1] = yRng[1];

        corners[2].data[0] = xRng[1];
        corners[2].data[1] = yRng[1];

        corners[3].data[0] = xRng[1];
        corners[3].data[1] = yRng[0];
    }

    bool AABB2::IsPointInside(float *p)
    {
        if(p[0] < xRng[0] || p[0] > xRng[1] || p[1] < yRng[0] || p[1] > yRng[1])return false;
        else return true;
    }

    int AABB2::RelationTo(AABB2 &box)
    {
        Tuple2f cornersBox[4];
        Tuple2f cornersThis[4];

        this->GetCorners(cornersThis);
        box.GetCorners(cornersBox);

        int countPointsOnBoxInThis = 0;

        for(int i = 0;i < 4;i++)
        {
            if(this->IsPointInside(cornersBox[i].data))
                countPointsOnBoxInThis++;
        }

        int countPointsOnThisInBox = 0;

        for(int i = 0;i < 4;i++)
        {
            if(box.IsPointInside(cornersThis[i].data))
                countPointsOnThisInBox++;
        }

        if(countPointsOnThisInBox == 0 && countPointsOnBoxInThis == 0)return AABB_INTERSECTION::DISJOINT;
        else if(countPointsOnThisInBox == 0 && countPointsOnBoxInThis == 4)return AABB_INTERSECTION::CONTAINS;
        else if(countPointsOnThisInBox == 4 && countPointsOnBoxInThis == 0)return AABB_INTERSECTION::CONTAINED_IN;
        else return AABB_INTERSECTION::OVERLAPS_WITH;
    }

    // AABB3
    AABB3::AABB3()
    {
        xRng[0] = 0.0;
        xRng[1] = 0.0;
        yRng[0] = 0.0;
        yRng[1] = 0.0;
        zRng[0] = 0.0;
        zRng[1] = 0.0;
    }

    AABB3::~AABB3(){}

    bool AABB3::Compute(vector<float> &points)
    {
        if (points.size() == 0 ) return false;

        xRng[0] = FLT_MAX; xRng[1] = -FLT_MAX;
        yRng[0] = FLT_MAX; yRng[1] = -FLT_MAX;
        zRng[0] = FLT_MAX; zRng[1] = -FLT_MAX;

        for (size_t i = 0; i < points.size(); i++)
        {
            if (points[3*i] < xRng[0]) xRng[0] = points[3*i];
            if (points[3*i] > xRng[1]) xRng[1] = points[3*i];

            if (points[3*i + 1] < yRng[0]) yRng[0] = points[3*i + 1];
            if (points[3*i + 1] > yRng[1]) yRng[1] = points[3*i + 1];

            if (points[3*i + 2] < zRng[0]) zRng[0] = points[3*i + 2];
            if (points[3*i + 2] > zRng[1]) zRng[1] = points[3*i + 2];
        }

        return true;
    }

    bool AABB3::Compute(vector<Tuple3f> &points)
    {
        if (points.size() == 0 ) return false;

        xRng[0] = FLT_MAX; xRng[1] = -FLT_MAX;
        yRng[0] = FLT_MAX; yRng[1] = -FLT_MAX;
        zRng[0] = FLT_MAX; zRng[1] = -FLT_MAX;

        for (size_t i = 0; i < points.size(); i++) {
            if (points[i].data[0] < xRng[0]) xRng[0] = points[i].data[0];
            if (points[i].data[0] > xRng[1]) xRng[1] = points[i].data[0];

            if (points[i].data[1] < yRng[0]) yRng[0] = points[i].data[1];
            if (points[i].data[1] > yRng[1]) yRng[1] = points[i].data[1];

            if (points[i].data[2] < zRng[0]) zRng[0] = points[i].data[2];
            if (points[i].data[2] > zRng[1]) zRng[1] = points[i].data[2];
        }

        return true;
    }

    void AABB3::Get(float *xRng, float *yRng, float *zRng)
    {
        xRng[0] = this->xRng[0];
        xRng[1] = this->xRng[1];

        yRng[0] = this->yRng[0];
        yRng[1] = this->yRng[1];

        zRng[0] = this->zRng[0];
        zRng[1] = this->zRng[1];
    }

    void AABB3::GetCentroid(float *centroid)
    {
        centroid[0] = 0.5*(this->xRng[0] + this->xRng[1]);
        centroid[1] = 0.5*(this->yRng[0] + this->yRng[1]);
        centroid[2] = 0.5*(this->zRng[0] + this->zRng[1]);
    }

    void AABB3::GetCorners(Tuple3f corners[8])
    {
        corners[0].data[0] = xRng[0];
        corners[0].data[1] = yRng[0];
        corners[0].data[2] = zRng[0];

        corners[1].data[0] = xRng[0];
        corners[1].data[1] = yRng[1];
        corners[1].data[2] = zRng[0];

        corners[2].data[0] = xRng[1];
        corners[2].data[1] = yRng[1];
        corners[2].data[2] = zRng[0];

        corners[3].data[0] = xRng[1];
        corners[3].data[1] = yRng[0];
        corners[3].data[2] = zRng[0];

        corners[0].data[0] = xRng[0];
        corners[0].data[1] = yRng[0];
        corners[0].data[2] = zRng[1];

        corners[1].data[0] = xRng[0];
        corners[1].data[1] = yRng[1];
        corners[1].data[2] = zRng[1];

        corners[2].data[0] = xRng[1];
        corners[2].data[1] = yRng[1];
        corners[2].data[2] = zRng[1];

        corners[3].data[0] = xRng[1];
        corners[3].data[1] = yRng[0];
        corners[3].data[2] = zRng[1];
    }

    bool AABB3::IsPointInside(float *p)
    {
        if(p[0] < xRng[0] || p[0] > xRng[1] || p[1] < yRng[0] || p[1] > yRng[1] || p[2] < zRng[0] || p[2] > zRng[1])return false;
        else return true;
    }

    int AABB3::RelationTo(AABB3 &box)
    {
        Tuple3f cornersBox[8];
        Tuple3f cornersThis[8];

        this->GetCorners(cornersThis);
        box.GetCorners(cornersBox);

        int countPointsOnBoxInThis = 0;

        for(int i = 0;i < 8;i++)
        {
            if(this->IsPointInside(cornersBox[i].data))
                countPointsOnBoxInThis++;
        }

        int countPointsOnThisInBox = 0;

        for(int i = 0;i < 8;i++)
        {
            if(box.IsPointInside(cornersThis[i].data))
                countPointsOnThisInBox++;
        }

        if(countPointsOnThisInBox == 0 && countPointsOnBoxInThis == 0)return AABB_INTERSECTION::DISJOINT;
        else if(countPointsOnThisInBox == 0 && countPointsOnBoxInThis == 8)return AABB_INTERSECTION::CONTAINS;
        else if(countPointsOnThisInBox == 8 && countPointsOnBoxInThis == 0)return AABB_INTERSECTION::CONTAINED_IN;
        else return AABB_INTERSECTION::OVERLAPS_WITH;
    }

}
