#pragma once 

#include <memory>
#include <cstdint> 

struct bits32_t {

	struct iterator {

		public:
			iterator (const bits32_t& o) : wrap(o), currentpos(0) {}
			bool	 	has_more() const {
				return currentpos < wrap.kbitlength-1;
			}
			// pair: nbits , value 
			std::pair<uint32_t, uint32_t> 	next(size_t win) 
			{
				auto w = std::min(win, wrap.kbitlength-currentpos);
				auto p = currentpos;
				currentpos+=w;
				return  std::make_pair(w,(wrap.val>>(32-p-w))  &  ((1<<w)-1));
			}

		private:
			size_t 		    currentpos;
			const bits32_t & wrap;
	};

	friend struct iterator;

	public:
		bits32_t(uint32_t val, size_t width): val(val), kbitlength(width){}
		iterator new_iterator() {
			return iterator(*this);
		}
	
private:
	const	size_t				  kbitlength;
	const   uint32_t			  val;
};

