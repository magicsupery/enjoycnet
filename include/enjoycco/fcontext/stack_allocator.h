#pragma once
#include <memory>

namespace enjoyc
{
	namespace co
	{

		template< std::size_t Max, std::size_t Default, std::size_t Min >
			class StackAllocator 
			{
				public:
					static std::size_t maximum_stacksize()
					{ return Max; }

					static std::size_t default_stacksize()
					{ return Default; }

					static std::size_t minimum_stacksize()
					{ return Min; }

					void * allocate( std::size_t size) const
					{
						void * limit = malloc( size);
						if ( ! limit) throw std::bad_alloc();

						return static_cast< char * >( limit) + size;
					}

					void deallocate( void * vp, std::size_t size) const
					{
						void * limit = static_cast< char * >( vp) - size;
						free( limit);
					}
			};
	}
}
