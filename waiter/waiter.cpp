#include <string>
#include "stdlib.h"

#include "../includes/waiter.h"
#include "../includes/externs.h"

using namespace std;

Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder){

	int nextOrder = myIO.getNext(anOrder);
	//Seems like I could just return nextOrder, but header file implies otherwise
	if (nextOrder != SUCCESS) {
		b_WaiterIsFinished = true;
		return NO_ORDERS;
	}
	return SUCCESS;
}

void Waiter::beWaiter() {
	ORDER newOrder;

	while(getNext(newOrder) == SUCCESS) {
		lock_guard<mutex> lock(mutex_order_inQ);
		order_in_Q.push(newOrder);
	}

	b_WaiterIsFinished = true;
	cv_order_inQ.notify_all();
}

