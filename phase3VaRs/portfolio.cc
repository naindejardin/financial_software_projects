#include <stdlib.h>
#include "SBB_date.h"
#include "portfolio.h"

using namespace std;

Portfolio::Portfolio(InstrumentFields r[],
		     BondCalculatorInterface* rc[],
		     int len) {
	lAmt = 0;
	sAmt = 0;
	risk = 0;
	sum = 0;

	records = r;
	records_calc = rc;
	length = len;

	SBB_date from_dt;
	SBB_date to_dt;

	for(int i=0;i<length;i++) {
		from_dt.set_from_yyyymmdd(records[i].SettlementDate);
		to_dt.set_from_yyyymmdd(records[i].MaturityDate);
		int T = Term::getNumberOfPeriods(from_dt, to_dt, 1);

		if (T>0 && T<=2)
			bucket0_2.push_back(records_calc[i]);
		else if(T>2 && T<=5)
			bucket2_5.push_back(records_calc[i]);
		else if(T>5 && T<=10)
			bucket5_10.push_back(records_calc[i]);
		else if(T>10 && T<=30)
			bucket10_30.push_back(records_calc[i]);
	}
}

int
Portfolio::getLongest() {
	for (int i=0;i<length;i++)
	{
		int amt = records[i].Amount;
		if(amt > lAmt)
			lAmt = amt;
	}
	return lAmt;
}

int
Portfolio::getShortest() {
	for (int i=0;i<length;i++)
	{
		int amt = records[i].Amount;
		if(amt < sAmt)
			sAmt = amt;
	}
	return sAmt;
}

double
Portfolio::getMostRisky() {
	for (int i=0;i<length;i++)
	{
		double tRisk = records_calc[i]->getRisk();
		if(abs(tRisk) > abs(risk))
			risk = tRisk;
	}
	return risk;
}

double
Portfolio::getTotalRisk() {
	sum = 0;
	for(int i=0;i<length;i++){
	double tRisk = records_calc[i]->getRisk();
	sum+=tRisk;	
	}
	return sum;
}

void
Portfolio::show() {
	printf("Longest: %d Shortest: %d Risky: %.3f Total: %.3f\n",
		lAmt, sAmt, risk, sum);
}

double
Portfolio::cal_amount10_30(double dvo1)
{
	double bucket_risk=0;
	for (unsigned int i=0; i<bucket10_30.size(); i++)
	{
		bucket_risk+=bucket10_30[i]->getRisk();
	}

	return bucket_risk/dvo1;
}

double
Portfolio::cal_Market_Value(BondCalculatorInterface** records_calc,int item_count, int shift)
{
	double market_value = 0;
	for (int i=0; i< item_count; i++)
	{
		market_value += records_calc[i]->getShiftMktVal(shift);
		//cout<<i<<": shift:"<<shift<<" shiftedMV:"<<records_calc[i]->getShiftMktVal(shift)<<endl;
	}
	return market_value;
}

double
Portfolio::getTotalLGD()
{
	double total_LGD = 0;
	for(int i=0; i<length; i++) {
		total_LGD += records_calc[i]->getLGD() * records[i].Amount;
	}
	return total_LGD;
}

int
Portfolio::getTotalAmount()
{
	int total_amount=0;
	for (int i=0; i<length; i++)
	{
		total_amount += records[i].Amount;
	}
	return total_amount;
}
