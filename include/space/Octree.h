#pragma once

#include <vector>

#include "linear/Vector.h"

namespace sylvanmats::space {
    enum DIRECIONS{
        TLF,    // top left front
        TRF,    // top right front
        BRF,    // bottom right front
        BLF,    // bottom left front
        TLB,    // top left back
        TRB,    // top right back
        BRB,    // bottom right back
        BLB     // bottom left back        
    };
    
    class Octree {
    private:
        sylvanmats::linear::Vector3d point;
        
        sylvanmats::linear::Vector3d top_left_front, bottom_right_back;   // represents the space.
        std::vector<std::shared_ptr<Octree>> children;
        
    public:
        Octree() = default;
        Octree(double x, double y, double z) : point (sylvanmats::linear::Vector3d(x, y, z)) {
        };
        Octree(double x1, double y1, double z1, double x2, double y2, double z2){
            if(x2 < x1 || y2 < y1 || z2 < z1)
                return;
            point =  sylvanmats::linear::Vector3d{};
            top_left_front = sylvanmats::linear::Vector3d(x1, y1, z1);
            bottom_right_back = sylvanmats::linear::Vector3d(x2, y2, z2);
            children.assign(8, std::shared_ptr<Octree>(new Octree()));
            for(int i = TLF; i <= BLB; ++i)
                children[i].reset(new Octree());
        };
        Octree(const Octree& orig) = delete;
        virtual ~Octree()= default;
        
    void insert(double x, double y, double z){
        if(x < top_left_front.x() || x > bottom_right_back.x()
            || y < top_left_front.y() || y > bottom_right_back.y()
            || z < top_left_front.z() || z > bottom_right_back.z())
            return;
        double midx = (top_left_front.x() + bottom_right_back.x())/2.0,
            midy = (top_left_front.y() + bottom_right_back.y())/2.0,
            midz = (top_left_front.z() + bottom_right_back.z())/2.0;
        int pos = -1;
        if(x <= midx){
            if(y <= midy){
                if(z <= midz)
                    pos = TLF;
                else
                    pos = TLB;
            }
            else{
                if(z <= midz)
                    pos = BLF;
                else
                    pos = BLB;
            }
        }
        else{
            if(y <= midy){
                if(z <= midz)
                    pos = TRF;
                else
                    pos = TRB;
            }
            else{
                if(z <= midz)
                    pos = BRF;
                else
                    pos = BRB;
            }
        }

        if(children[pos]->point == sylvanmats::linear::Vector3d{}){
           // if region node
            children[pos]->insert(x, y, z);
            return;
        }
        else if(children[pos]->point.x() == -1){
            // if empty node
            children[pos].reset(new Octree(x, y, z));
            return;
        }
        else{
            double x_ = children[pos]->point.x(),
                y_ = children[pos]->point.y(),
                z_ = children[pos]->point.z();
            children[pos].reset(new Octree());
            if(pos == TLF){
                children[pos].reset(new Octree(top_left_front.x(), top_left_front.y(), top_left_front.z(),
                                        midx, midy, midz));
            }
            else if(pos == TRF){
                children[pos].reset(new Octree(midx + 1, top_left_front.y(), top_left_front.z(),
                                        bottom_right_back.x(), midy, midz));
            }
            else if(pos == BRF){
                children[pos].reset(new Octree(midx + 1, midy + 1, top_left_front.z(),
                                        bottom_right_back.x(), bottom_right_back.y(), midz));
            }
            else if(pos == BLF){
                children[pos].reset(new Octree(top_left_front.x(), midy + 1, top_left_front.z(),
                                        midx, bottom_right_back.y(), midz));
            }
            else if(pos == TLB){
                children[pos].reset(new Octree(top_left_front.x(), top_left_front.y(), midz + 1,
                                        midx, midy, bottom_right_back.z()));
            }
            else if(pos == TRB){
                children[pos].reset(new Octree(midx + 1, top_left_front.y(), midz + 1,
                                        bottom_right_back.x(), midy, bottom_right_back.z()));
            }
            else if(pos == BRB){
                children[pos].reset(new Octree(midx + 1, midy + 1, midz + 1,
                                        bottom_right_back.x(), bottom_right_back.y(), bottom_right_back.z()));
            }
            else if(pos == BLB){
                children[pos].reset(new Octree(top_left_front.x(), midy + 1, midz + 1,
                                        midx, bottom_right_back.y(), bottom_right_back.z()));
            }
            children[pos]->insert(x_, y_, z_);
            children[pos]->insert(x, y, z);
        }
    }
    
    bool find(double x, double y, double z){
        if(x < top_left_front.x() || x > bottom_right_back.x()
            || y < top_left_front.y() || y > bottom_right_back.y()
            || z < top_left_front.z() || z > bottom_right_back.z())
            return false;
        double midx = (top_left_front.x() + bottom_right_back.x())/2.0,
            midy = (top_left_front.y() + bottom_right_back.y())/2.0,
            midz = (top_left_front.z() + bottom_right_back.z())/2.0;
        int pos = -1;
        if(x <= midx){
            if(y <= midy){
                if(z <= midz)
                    pos = TLF;
                else
                    pos = TLB;
            }
            else{
                if(z <= midz)
                    pos = BLF;
                else
                    pos = BLB;
            }
        }
        else{
            if(y <= midy){
                if(z <= midz)
                    pos = TRF;
                else
                    pos = TRB;
            }
            else{
                if(z <= midz)
                    pos = BRF;
                else
                    pos = BRB;
            }
        }
        if(children[pos]->point == sylvanmats::linear::Vector3d{}){
           // if region node
            return children[pos]->find(x, y, z);
        }
        else if(children[pos]->point.x() == -1){
            // if empty node
            return false;
        }
        else{
            if(x == children[pos]->point.x() && y == children[pos]->point.y()
                && z == children[pos]->point.z())
                return true;
        }
        return 0;
    }
    };

}
