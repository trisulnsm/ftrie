#include "ftrie.h"
#include <cassert>
#include <algorithm>

static inline void printpos(uint8_t val)
{
	if (val==0xff) 
		printf(". ");
	else 
		printf("%d ", val);
}

ptreearray_node_4_t::ptreearray_node_4_t()
:node_value(NO_VALUE),
 ppos4(nullptr),
 ppos3(nullptr),
 ppos2(nullptr),
 ppos1(nullptr),
 pvb4(nullptr),
 pvb3(nullptr),
 pvb2(nullptr),
 pvb1(nullptr)
{

}

ptreearray_node_4_t::~ptreearray_node_4_t()
{
	delete ppos4;
	delete ppos3;
	delete ppos2;
	delete ppos1;
	cleanup( pvb4);
	cleanup( pvb3);
	cleanup( pvb2);
	cleanup( pvb1);
}

void  	 ptreearray_node_4_t::cleanup( std::vector<ptreearray_node_4_t*> * pv)
{
	if (pv) {
		for (auto p: *pv)  {
			delete p;
		}
		delete pv;
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
				if (ppos4==nullptr) ppos4= new std::vector<uint8_t>(16,0xff);
				if (pvb4==nullptr)  pvb4=  new std::vector<ptreearray_node_4_t*>;
				
				auto idx =  ppos4->at(p.second);

				if (pvb4==nullptr)
					pvb4= new std::vector<ptreearray_node_4_t*>;

				if (idx==0xff) {
					ppos4->at(p.second)=pvb4->size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					pvb4->push_back(pnew);
				} else {
					pvb4->at(idx)->insert(iter,value);
				}
			}
			break;

		case 3:
			{
				if (ppos3==nullptr) ppos3= new std::vector<uint8_t>(8,0xff);
				if (pvb3==nullptr)  pvb3=  new std::vector<ptreearray_node_4_t*>;

				auto idx =  ppos3->at(p.second);

				if (idx==0xff) {
					ppos3->at(p.second)=pvb3->size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					pvb3->push_back(pnew);
				} else {
					pvb3->at(idx)->insert(iter,value);
				}
			}
			break;

		case 2:
			{
				if (ppos2==nullptr) ppos2= new std::vector<uint8_t>(4,0xff);
				if (pvb2==nullptr)  pvb2=  new std::vector<ptreearray_node_4_t*>;

				auto idx =  ppos2->at(p.second);


				if (idx==0xff) {
					ppos2->at(p.second)=pvb2->size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					pvb2->push_back(pnew);
				} else {
					pvb2->at(idx)->insert(iter,value);
				}
			}
			break;

		case 1:
			{
				if (ppos1==nullptr) ppos1= new std::vector<uint8_t>(2,0xff);
				if (pvb1==nullptr)  pvb1=  new std::vector<ptreearray_node_4_t*>;

				auto idx =  ppos1->at(p.second);

				if (idx==0xff) {
					ppos1->at(p.second)=pvb1->size();
					auto pnew = new ptreearray_node_4_t;
					pnew->insert(iter,value);
					pvb1->push_back(pnew);
				} else {
					pvb1->at(idx)->insert(iter,value);
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
	if      (ppos4  && ppos4->at(p.second)!=0xff) {
		return pvb4->at(ppos4->at(p.second))->longest_match(iter,node_value);
	}
	else if (ppos3 && ppos3->at(p.second>>1)!=0xff) {
		return pvb3->at(ppos3->at(p.second>>1))->longest_match(iter,node_value);
	}
	else if (ppos2 && ppos2->at(p.second>>2)!=0xff) {
		return pvb2->at(ppos2->at(p.second>>2))->longest_match(iter,node_value);
	}
	else if (ppos1 && ppos1->at(p.second>>3)!=0xff) {
		return pvb1->at(ppos1->at(p.second>>3))->longest_match(iter,node_value);
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
		printpos(ppos4->at(i));
	}
	printf("\n");

	for (int i=0;i<level;i++) printf(" ");
	printf("3B: ");
	for (int i=0;i<8;i++) {
		printpos(ppos3->at(i));
	}
	printf("\n");

	for (int i=0;i<level;i++) printf(" ");
	printf("2B: ");
	for (int i=0;i<4;i++) {
		printpos(ppos2->at(i));
	}
	printf("\n");

	for (int i=0;i<level;i++) printf(" ");
	printf("1B: ");
	for (int i=0;i<2;i++) {
		printpos(ppos1->at(i));
	}
	printf("\n");


	if (pvb4 and not pvb4->empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 4: ");
		for ( int i=0;i<16;i++) {
			if (ppos4->at(i)!=0xff) {
				pvb4->at(ppos4->at(i))->debugdump(level+1);
			}
		}
		printf("\n");
	}
	
	
	if (pvb3 and not pvb3->empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 3: ");
		for ( int i=0;i<8;i++) {
			if (ppos3->at(i)!=0xff) {
				pvb3->at(ppos3->at(i))->debugdump(level+1);
			}
		}
		printf("\n");
	}
	
	if (pvb2 and not pvb2->empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 2: ");
		for ( int i=0;i<4;i++) {
			if (ppos2->at(i)!=0xff) {
				pvb2->at(ppos2->at(i))->debugdump(level+1);
			}
		}
		printf("\n");
	}
	
	if (pvb1 and not pvb1->empty()) {
		for (int i=0;i<level;i++) printf(" ");
		printf("Down 1: ");
		for ( int i=0;i<2;i++) {
			if (ppos1->at(i)!=0xff) {
				pvb1->at(ppos1->at(i))->debugdump(level+1);
			}
		}
		printf("\n");
	}


}

void	 ptreearray_node_4_t::shrink_to_fit()
{
	using child_t = std::vector<ptreearray_node_4_t*>;

	if (pvb4)  {
		pvb4->shrink_to_fit();
		std::for_each (pvb4->begin(), pvb4->end(),
							[] (ptreearray_node_4_t * p) {
								p->shrink_to_fit();
							});
	}

	if (pvb3)  {
		pvb3->shrink_to_fit();
		std::for_each (pvb3->begin(), pvb3->end(),
							[] (ptreearray_node_4_t * p) {
								p->shrink_to_fit();
							});
	}

	if (pvb2)  {
		pvb2->shrink_to_fit();
		std::for_each (pvb2->begin(), pvb2->end(),
							[] (ptreearray_node_4_t * p) {
								p->shrink_to_fit();
							});
	}

	if (pvb1)  {
		pvb1->shrink_to_fit();
		std::for_each (pvb1->begin(), pvb1->end(),
							[] (ptreearray_node_4_t * p) {
								p->shrink_to_fit();
							});
	}

}

