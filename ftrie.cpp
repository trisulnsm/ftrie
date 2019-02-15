#include "ftrie.h"
#include <cassert>

static inline void printpos(uint8_t val)
{
	if (val==0xff) 
		printf(". ");
	else 
		printf("%d ", val);
}

ptreearray_node_4_t::ptreearray_node_4_t()
:node_value(NO_VALUE),
 pos4(16,0xff),
 pos3( 8,0xff),
 pos2( 4,0xff),
 pos1( 2,0xff)
{

}

ptreearray_node_4_t::~ptreearray_node_4_t()
{
	for (auto p:vb4)  {
		delete p;
	}
	for (auto p:vb3)  {
		delete p;
	}
	for (auto p:vb2)  {
		delete p;
	}
	for (auto p:vb1)  {
		delete p;
	}
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
			{
				auto idx =  pos4[p.second];
				if (idx==0xff) {
					pos4[p.second]=vb4.size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					vb4.push_back(pnew);
				} else {
					vb4[idx]->insert(iter,value);
				}
			}
			break;

		case 3:
			{
				auto idx =  pos3[p.second];
				if (idx==0xff) {
					pos3[p.second]=vb3.size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					vb3.push_back(pnew);
				} else {
					vb3[idx]->insert(iter,value);
				}
			}
			break;

		case 2:
			{
				auto idx =  pos2[p.second];
				if (idx==0xff) {
					pos2[p.second]=vb2.size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					vb2.push_back(pnew);
				} else {
					vb2[idx]->insert(iter,value);
				}
			}
			break;

		case 1:
			{
				auto idx =  pos1[p.second];
				if (idx==0xff) {
					pos1[p.second]=vb1.size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					vb1.push_back(pnew);
				} else {
					vb1[idx]->insert(iter,value);
				}
			}
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
	if (pos4[p.second]!=0xff) {
		return vb4[pos4[p.second]]->longest_match(iter,node_value);
	}
	else if (pos3[p.second>>1]!=0xff) {
		return vb3[pos3[p.second>>1]]->longest_match(iter,node_value);
	}
	else if (pos2[p.second>>2]!=0xff) {
		return vb2[pos2[p.second>>2]]->longest_match(iter,node_value);
	}
	else if (pos1[p.second>>3]!=0xff) {
		return vb1[pos1[p.second>>3]]->longest_match(iter,node_value);
	}
	else {
		return node_value==NO_VALUE?parentval:node_value;
	}

}

void    ptreearray_node_4_t::debugdump(int level)
{
	printf("\n");

	if (node_value != NO_VALUE) {
		printf("VALUE = %d\n", node_value);
	}

	// 4 
	for (int i=0;i<level;i++) printf(" ");
	printf("4B: ");
	for (int i=0;i<16;i++) {
		printpos(pos4[i]);
	}
	printf("\n");

	for (int i=0;i<level;i++) printf(" ");
	printf("3B: ");
	for (int i=0;i<8;i++) {
		printpos(pos3[i]);
	}
	printf("\n");

	for (int i=0;i<level;i++) printf(" ");
	printf("2B: ");
	for (int i=0;i<4;i++) {
		printpos(pos2[i]);
	}
	printf("\n");

	for (int i=0;i<level;i++) printf(" ");
	printf("1B: ");
	for (int i=0;i<2;i++) {
		printpos(pos1[i]);
	}
	printf("\n");


	if (not vb4.empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 4: ");
		for ( int i=0;i<16;i++) {
			if (pos4[i]!=0xff) {
				vb4[pos4[i]]->debugdump(level+1);
			}
		}
		printf("\n");
	}
	
	
	if (not vb3.empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 3: ");
		for ( int i=0;i<8;i++) {
			if (pos3[i]!=0xff) {
				vb3[pos3[i]]->debugdump(level+1);
			}
		}
		printf("\n");
	}
	
	if (not vb2.empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 2: ");
		for ( int i=0;i<4;i++) {
			if (pos2[i]!=0xff) {
				vb2[pos2[i]]->debugdump(level+1);
			}
		}
		printf("\n");
	}
	
	if (not vb1.empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 1: ");
		for ( int i=0;i<2;i++) {
			if (pos1[i]!=0xff) {
				vb1[pos1[i]]->debugdump(level+1);
			}
		}
		printf("\n");
	}


}

