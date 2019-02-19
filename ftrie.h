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
