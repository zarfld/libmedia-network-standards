#include "qos.h"
#include "scheduler.h"

namespace IEEE { namespace _802_1Q { namespace _2020 {

void QoSProfile::apply_to_shaper(IShaper& shaper) const {
	for (uint8_t i = 0; i < num_tc_; ++i) {
		const auto& c = tc_cfg_[i];
		shaper.configure_class(i, c.idle_slope_kbps, c.send_slope_kbps, c.hi_credit_bytes, c.lo_credit_bytes);
	}
}

void QoSProfile::apply_to_scheduler(IQueueScheduler& sched) const {
	sched.configure_num_classes(num_tc_);
}
}}} // namespace
