#include "openvslam/optimize/g2o/landmark_vertex.h"

#if _MSC_VER && G2O_SHARED_LIBS
extern template class __declspec(dllimport) g2o::BaseVertex<3, g2o::Vector3>;
#endif

namespace openvslam {
namespace optimize {
namespace g2o {

landmark_vertex::landmark_vertex()
    : BaseVertex<3, Vec3_t>() {}

bool landmark_vertex::read(std::istream& is) {
    Vec3_t lv;
    for (unsigned int i = 0; i < 3; ++i) {
        is >> _estimate(i);
    }
    return true;
}

bool landmark_vertex::write(std::ostream& os) const {
    const Vec3_t pos_w = estimate();
    for (unsigned int i = 0; i < 3; ++i) {
        os << pos_w(i) << " ";
    }
    return os.good();
}

} // namespace g2o
} // namespace optimize
} // namespace openvslam
