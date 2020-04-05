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

#include <cstdint>
#include <vector>
#include <bitset>
#include "ftrie.h"


template<std::size_t Nbits>
struct bitlevel_t
{
	enum 
	{
		EMPTY_SLOT=0xFF,
	};

	bitlevel_t()
	{
		for (int i=0;i< (1<<Nbits) ;i++) {
			pos[i]=EMPTY_SLOT;
		}
	}

	~bitlevel_t() 
	{
		for (int i=0;i<(1<<Nbits);i++) 
		{
			auto idx = pos[i];
			if (idx!=EMPTY_SLOT && not data_leaves.test(idx))
			{
				delete vb[idx];
			}
		}
	}

	void insert(bits32_t::iterator &iter, uint32_t  bitval, uint32_t value)
	{
		auto idx =  pos.at(bitval);
		if (idx==EMPTY_SLOT) {
			pos[bitval]=vb.size();
			auto pnew = new ptreearray_node_4_t;
			pnew->insert(iter,value);
			vb.push_back(pnew);
		} else {
			vb[idx]->insert(iter,value);
		}
	}

	bool  can_match(uint32_t bitval) 
	{
		auto idx  = bitval>>(4-Nbits);
		auto vidx = pos[idx];
		return vidx!=0xff;
	}

	uint32_t longest_match(bits32_t::iterator &iter, uint32_t bitval, uint32_t parentval)
	{
		auto idx  = bitval>>(4-Nbits);
		auto vidx = pos[idx];

		assert(can_match(bitval));

		if ( data_leaves.test(vidx)){
			return (uint32_t) reinterpret_cast<uintptr_t>(vb[vidx]);
		} else {
			return vb[vidx]->longest_match(iter,parentval);
		}
	}
	void  debugdumppos(int level)
	{
		for (int i=0;i<level;i++) printf(" ");
		printf("%luB: ",Nbits);
		for (int i=0;i<(1<<Nbits);i++) {
			printpos(pos[i]);
		}
		printf("\n");
	}
	void printpos(uint8_t val)
	{
		if (val==0xff) 
			printf(". ");
		else 
			printf("%d ", val);
	}

	void  debugdumpchild(int level)
	{
		for (int i=0;i<level;i++) printf(" ");
		printf("Down %lu: ", Nbits);
		for ( int i=0;i<(1<<Nbits);i++) {
			if (pos[i]!=0xff) {
				if ( data_leaves.test(pos[i])){
					printf("DATA NODE value=%u ", (uint32_t) reinterpret_cast<uintptr_t>(vb[pos[i]]));
				} else {
					vb[pos[i]]->debugdump(level+1);
				}
			}
		}
		printf("\n");
	}
	bool 	collapse_leaves()
	{
		for (int i=0;i<(1<<Nbits);i++) {
			if (pos[i] != EMPTY_SLOT && !data_leaves.test(pos[i])){
				auto pc = vb[pos[i]];
				if (pc->isemptynode()) {
					data_leaves.set(pos[i]);	
					vb[pos[i]]=(ptreearray_node_4_t*) (uint64_t) pc->node_value;
					delete pc;
				} else {
					pc->collapse_leaves();
				}
			}
		}
		return true;
	}


	std::bitset< 1 << Nbits >	 			data_leaves;
	std::array<uint8_t, ( 1<<Nbits) >		pos;
	std::vector<ptreearray_node_4_t*>	  	vb; 
};

