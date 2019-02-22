#include "ftrie.h"
#include <cassert>
#include <algorithm>
#include "bitlevel.h"

ptreearray_node_4_t::ptreearray_node_4_t()
:node_value(NO_VALUE),
 bl4(nullptr),
 bl3(nullptr),
 bl2(nullptr),
 bl1(nullptr)
{

}

ptreearray_node_4_t::~ptreearray_node_4_t()
{
	delete bl4;
	delete bl3;
	delete bl2;
	delete bl1;
}

bool	 ptreearray_node_4_t::isemptynode()
{
	return bl4==nullptr && bl3==nullptr && bl2==nullptr && bl1==nullptr;
}

void	 ptreearray_node_4_t::collapse_leaves()
{
	bl4 && bl4->collapse_leaves();
	bl3 && bl3->collapse_leaves();
	bl2 && bl2->collapse_leaves();
	bl1 && bl1->collapse_leaves();
}

void 	ptreearray_node_4_t::insert(bits32_t::iterator &iter, uint32_t value)
{
	if (not iter.has_more()) {
		node_value=value;
		return;
	}

	auto p = iter.next(4);
	switch(p.first)
	{
		case 4:
				if (bl4==nullptr) 
					bl4=new bitlevel_t<4>();
				bl4->insert(iter,p.second,value);
				break;

		case 3:
				if (bl3==nullptr) 
					bl3=new bitlevel_t<3>();
				bl3->insert(iter,p.second,value);
				break;

		case 2:
				if (bl2==nullptr) 
					bl2=new bitlevel_t<2>();
				bl2->insert(iter,p.second,value);
				break;

		case 1:
				if (bl1==nullptr) 
					bl1=new bitlevel_t<1>();
				bl1->insert(iter,p.second,value);
				break;
		default:
			assert(false);
			break;
	}
}

uint32_t ptreearray_node_4_t::longest_match(const char * ipstr)
{
	int a,b,c,d;
	if (sscanf(ipstr,"%d.%d.%d.%d", &a,&b,&c,&d)==4)
	{
		uint32_t addr = (a<<24) | (b << 16) | (c << 8)  | d ;
		bits32_t b(addr, 32 ) ;
		auto iter=b.new_iterator();
		return longest_match(iter, node_value);
	} else {
		return NO_VALUE;
	}
}

uint32_t 	ptreearray_node_4_t::longest_match(bits32_t::iterator &iter, uint32_t parentval)
{
	auto p = iter.next(4);

	if      (bl4 && bl4->can_match(p.second) ) {
		return bl4->longest_match(iter,p.second,parentval);
	}
	else if      (bl3 && bl3->can_match(p.second) ) {
		return bl3->longest_match(iter,p.second,parentval);
	}
	else if      (bl2 && bl2->can_match(p.second) ) {
		return bl2->longest_match(iter,p.second,parentval);
	}
	else if      (bl1 && bl1->can_match(p.second) ) {
		return bl1->longest_match(iter,p.second,parentval);
	}
	else {
		return node_value==NO_VALUE?parentval:node_value;
	}

}

void    ptreearray_node_4_t::debugdump(int level)
{
	printf("\n");

	if (isemptynode()) {
		printf("EMPTY LEAF NODE\n size=%lu" , sizeof(*this) );

	}

	if (node_value != NO_VALUE) {
		printf("VALUE = %d\n", node_value);
	}

	// 4 
	if (bl4) {
		bl4->debugdumppos(level);
	}
	if (bl3) {
		bl3->debugdumppos(level);
	}
	if (bl2) {
		bl2->debugdumppos(level);
	}
	if (bl1) {
		bl1->debugdumppos(level);
	}

	if (bl4) {
		bl4->debugdumpchild(level);
	}
	if (bl3) {
		bl3->debugdumpchild(level);
	}
	if (bl2) {
		bl2->debugdumpchild(level);
	}
	if (bl1) {
		bl1->debugdumpchild(level);
	}

}

