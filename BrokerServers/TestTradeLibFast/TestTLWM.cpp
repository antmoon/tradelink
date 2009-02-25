#include "stdafx.h"
#include <cfix.h>
#include "TradeLibFast.h"

using namespace TradeLibFast;

// pickup the results from our test in this class
class TestClient : public TLClient_WM
{
public:
	TestClient():orders(0),fills(0),ticks(0) { }
	int orders;
	int fills;
	int ticks;
	TLTrade lastfill;
	TLTick lasttick;
	TLOrder lastorder;
	void gotOrder(TLOrder o)
	{
		lastorder = o;
		orders++;
	}
	void gotFill(TLTrade fill)
	{
		lastfill = fill;
		fills++;
	}
	void gotTick(TLTick tick)
	{
		lasttick = tick;
		ticks++;
	}

};


static void __stdcall Basics()
{
	// we need this line here in order to create windows from the cfix 
	// console application that runs our tests
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		return;

	// setup our objects
	TLServer_WM s;
	s.Start(true);
	TestClient c;
	TestClient c2;
	CString sym = "TST";
	int size = 200;
	double price = 100;
	TLMarketBasket mb;
	mb.Add(sym);
	c.Subscribe(mb);

	// initial tests
	CFIX_ASSERT(c.ticks+c.fills+c.orders==0);

	// Tick test
	TLTick k;
	k.sym = sym;
	k.trade = price;
	k.size = size;
	s.SrvGotTick(k); // send tick
	CFIX_ASSERT(c.ticks==1);
	CFIX_ASSERT(c.lasttick.sym==k.sym);
	CFIX_ASSERT(c.lasttick.trade==k.trade);
	// make sure ticks were not copied to other clients
	CFIX_ASSERT(c.ticks!=c2.ticks);

	// Fill test
	TLTrade f;
	f.symbol = sym;
	f.xprice = price;
	f.xsize = size;
	f.xdate = 20081220;
	f.xtime = 1556;
	f.xsec = 0;
	f.side = true;
	s.SrvGotFill(f);
	CFIX_ASSERT(c.fills==1);
	CFIX_ASSERT(c.lastfill.symbol==f.symbol);
	CFIX_ASSERT(c.lastfill.xprice==f.xprice);
	// make sure fills were copiedto other clients
	CFIX_ASSERT(c.fills==c2.fills);

	// Order test
	TLOrder o;
	o.symbol = sym;
	o.price = price;
	o.size = size;
	s.SrvGotOrder(o);
	CFIX_ASSERT(c.orders==1);
	CFIX_ASSERT(c.lastorder.symbol==o.symbol);
	CFIX_ASSERT(c.lastorder.price==o.price);
	// make sure orders were copied to other clients
	CFIX_ASSERT(c.orders==c2.orders);


}


CFIX_BEGIN_FIXTURE( TestTLWM )
	CFIX_FIXTURE_ENTRY( Basics )
CFIX_END_FIXTURE()