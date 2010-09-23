#ifndef MULTIPLIER_H
#define MULTIPLIER_H

#include "operator.h"


class Multiplier : public Operator {
public:
	virtual int calc(int a, int b) const;
};


#endif
