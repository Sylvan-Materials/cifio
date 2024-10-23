#pragma once

#include <vector>
#include <type_traits>

#include "linear/Vector.h"

template<class T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
   almost_equal(T x, T y, T const abs_relative_error)
{
  return 2 * std::abs(x - y) <= abs_relative_error * std::abs(x + y);
}

namespace sylvanmats::space {
    enum DIRECIONS : int{
        UNKNOWN=-1, //not set yet
        TLF,        // top left front
        TRF,        // top right front
        BRF,        // bottom right front
        BLF,        // bottom left front
        TLB,        // top left back
        TRB,        // top right back
        BRB,        // bottom right back
        BLB         // bottom left back        
    };
    /**
    * https://iq.opengenus.org/octree/
    **/
    class Octree {
    private:
        std::shared_ptr<sylvanmats::linear::Vector3i> point;
        sylvanmats::linear::Vector3i top_left_front, bottom_right_back;   // represents the space.
        bool empty = false;
        bool regional = false;
        
        std::vector<std::shared_ptr<Octree>> children;
        
    public:
        Octree() : point (new sylvanmats::linear::Vector3i(-1, -1, -1)), empty (true) {
        };
        Octree(int x, int y, int z) : point (new sylvanmats::linear::Vector3i(x, y, z)), empty (false) {
        };
        Octree(int x1, int y1, int z1, int x2, int y2, int z2) : Octree(sylvanmats::linear::Vector3i(x1, y1, z1), sylvanmats::linear::Vector3i(x2, y2, z2)) {
            if(x2 < x1 || y2 < y1 || z2 < z1)
                return;
         };
        Octree(sylvanmats::linear::Vector3i top_left_front, sylvanmats::linear::Vector3i bottom_right_back) : top_left_front (top_left_front), bottom_right_back (bottom_right_back), empty (false) {
            if(bottom_right_back.x() < top_left_front.x() || bottom_right_back.y() < top_left_front.y() || bottom_right_back.z() < top_left_front.z())
                return;
            regional = true;
            children.assign(8, std::shared_ptr<Octree>(new Octree()));
            for(int i = TLF; i <= BLB; ++i)
                children[i].reset(new Octree());
        };
        Octree(const Octree& orig) = delete;
        virtual ~Octree()= default;
        
    void insert(int x, int y, int z){
        // If the point already exists in the octree
        if (find(x, y, z)) {
            std::cout << "Point already exist in the tree" << std::endl;
            return;
        }
        if(x < top_left_front.x() || x > bottom_right_back.x()
            || y < top_left_front.y() || y > bottom_right_back.y()
            || z < top_left_front.z() || z > bottom_right_back.z()){
            std::cout << "Point is out of bound" << std::endl;
            return;
        }
        int midx = (top_left_front.x() + bottom_right_back.x())/2.0,
            midy = (top_left_front.y() + bottom_right_back.y())/2.0,
            midz = (top_left_front.z() + bottom_right_back.z())/2.0;
        DIRECIONS pos = UNKNOWN;
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

        if(children[pos]->regional){
           // if region node
            children[pos]->insert(x, y, z);
            return;
        }
        else if(children[pos]->empty){
            // if empty node
            children[pos].reset(new Octree(x, y, z));
            return;
        }
        else{
            int x_ = children[pos]->point->x(),
                y_ = children[pos]->point->y(),
                z_ = children[pos]->point->z();
            children[pos].reset();
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
    
    bool find(int x, int y, int z){
        if(x < top_left_front.x() || x > bottom_right_back.x()
            || y < top_left_front.y() || y > bottom_right_back.y()
            || z < top_left_front.z() || z > bottom_right_back.z())
            return false;
        int midx = (top_left_front.x() + bottom_right_back.x())/2.0,
            midy = (top_left_front.y() + bottom_right_back.y())/2.0,
            midz = (top_left_front.z() + bottom_right_back.z())/2.0;
        DIRECIONS pos = UNKNOWN;
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
        if(children[pos]->regional){
           // if region node
            return children[pos]->find(x, y, z);
        }
        else if(children[pos]->empty){
            // if empty node
            return false;
        }
        else{
        //std::cout<<"tol "<<x<<" "<<y<<" "<<z<<" "<<children[pos]->point->x()<<" "<<children[pos]->point->y()<<" "<<children[pos]->point->z()<<std::endl;
            if(x==children[pos]->point->x() && y==children[pos]->point->y()
                && z==children[pos]->point->z())
                return true;
        }
        return false;
    }
    };

}
