#include <utility>
#include <cmath>

namespace inertia {
static inline std::pair<float, float> decay(float v, int ms) {
	float nv = std::pow(.9985, ms) * v;
	nv = std::abs(nv) < 0.05 ? 0 : nv;
	return std::make_pair(nv, (nv + v) / 2);
}
}


