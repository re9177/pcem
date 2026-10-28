#ifndef PCEM_UTILS_FREELISTPOOL_HPP
#define PCEM_UTILS_FREELISTPOOL_HPP

#include <cstdint>

namespace util {

	namespace impl {


		/// Default free list configuration
		class DefaultFreeListPoolConfig {
			/// The max amount of items before the pool is garbage collected.
			constexpr static std::size_t MaxItemsInPool = 32;
		};

		/// A free list and memory pool.
		template<class T, class Config = DefaultFreeListPoolConfig>
		class FreelistPool {
			struct Entity {
				T item;

				Entity* pPrev;
				Entity* pNext;

				bool bInUse;
			};
		public:

		};

	}

}

#endif
