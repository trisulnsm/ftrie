#pragma once 

#include <cinttypes>
#include "bitstring.h"
#include <vector>

struct ptreearray_node_4_t
{
	enum { NO_VALUE=0xffffffff } ;
	ptreearray_node_4_t();
	~ptreearray_node_4_t();

	uint32_t							node_value;

	std::vector<uint8_t>				pos4; 	// 4 bit
	std::vector<uint8_t>				pos3; 	// 3 bit
	std::vector<uint8_t>				pos2; 	// 2 bit
	std::vector<uint8_t>				pos1; 	// 1 bit

	std::vector<ptreearray_node_4_t*>	vb4; 	// 4 bit
	std::vector<ptreearray_node_4_t*>	vb2;	// 3 bit 
	std::vector<ptreearray_node_4_t*>	vb3;
	std::vector<ptreearray_node_4_t*>	vb1;

	void 	 insert(bits32_t::iterator &iter, uint32_t value);
	uint32_t longest_match(const char * ipstr);
	uint32_t longest_match(bits32_t::iterator &iter, uint32_t parentval);
	void     debugdump(int level);
};
