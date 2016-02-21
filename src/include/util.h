#ifndef __UTIL_H__
#define __UTIL_H__

#define CONCAT(a_, b_) a_ ## b_
#define CONCAT2(a_, b_) CONCAT(a_, b_)
#define UNIQ_TOKEN(name_) CONCAT2(name_, __LINE__)

#define ROUND_DOWN(addr_, align_) ({				\
	__typeof__(addr_) UNIQ_TOKEN(addr) = addr_;		\
	UNIQ_TOKEN(addr) - (UNIQ_TOKEN(addr) % (align_));	\
})

#define ROUND_UP(addr_, align_) ({					\
	__typeof__(align_) UNIQ_TOKEN(align) = align_;			\
	ROUND_DOWN(addr_ + UNIQ_TOKEN(align) - 1, UNIQ_TOKEN(align));	\
})

#endif
