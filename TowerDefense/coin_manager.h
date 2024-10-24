#ifndef _COIN_MANAGER_H_
#define _COIN_MANAGER_H_

#include "manager.h"

class CoinManager : public Manager<CoinManager>
{
	friend class Manager<CoinManager>;
public:
	void increase_coin(double val)
	{
		num_coin += val;
	}

	void decrease_coin(double val)
	{
		num_coin -= val;
		if (num_coin < 0)
			num_coin = 0;
	}

protected:
	CoinManager()
	{

	}

	~CoinManager()
	{

	}

private:
	double num_coin = 0;

};
#endif // !_COIN_MANAGER_H_
