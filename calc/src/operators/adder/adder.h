#ifndef ADDER_H
#define ADDER_H

#include "../operator.h"


class Adder : public Operator {
public:
	virtual int calc(int a, int b) const;
};


#endif
