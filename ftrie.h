/*
	ftrie.h
	a new variant of a RADIX trie that uses multiple levels at each node
	each node can have a 4-bit match, a 3-bit match, 2, or 1-bit
	may seem awkward but wins due to cache effects :-) 

	send comments to vivek@unleashnetworks.com 

*/
/*
	The MIT License

	Copyright (c) 2019 Unleash Networks. Trisul Network Analytics https://trisul.org 

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.


*/

#pragma once 

#include <cinttypes>
#include "bitstring.h"
#include <vector>

struct ptreearray_node_4_t
{
	enum { NO_VALUE=0xffffffff } ;
	ptreearray_node_4_t();
	~ptreearray_node_4_t();

	uint32_t								node_value;

	std::vector<uint8_t>			      * ppos4; 	// 4 bit
	std::vector<uint8_t>			      * ppos3; 	// 3 bit
	std::vector<uint8_t>			      * ppos2; 	// 2 bit
	std::vector<uint8_t>			      * ppos1; 	// 1 bit

	std::vector<ptreearray_node_4_t*>	  * pvb4; // 4 bit
	std::vector<ptreearray_node_4_t*>	  * pvb2;	// 3 bit 
	std::vector<ptreearray_node_4_t*>	  * pvb3;
	std::vector<ptreearray_node_4_t*>	  * pvb1;

	void 	 insert(bits32_t::iterator &iter, uint32_t value);
	uint32_t longest_match(const char * ipstr);
	uint32_t longest_match(bits32_t::iterator &iter, uint32_t parentval);
	void     debugdump(int level);
	void  	 cleanup( std::vector<ptreearray_node_4_t*> * p);
	void	 shrink_to_fit();
};
