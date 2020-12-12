#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
using namespace std;

Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

void Baker::bake_and_box(ORDER &anOrder) {
	Box box;
	int count = 0;

	for (int i = 0; i < anOrder.number_donuts; i++) {
		//put the next donut in the box
		count++;
		DONUT donut;
		box.addDonut(donut);

		//If box is full, push it onto the boxes vector and start a new box
		if (count == DOZEN) {
			lock_guard<mutex> lock(mutex_order_outQ);
			anOrder.boxes.push_back(box);
			box.clear();
			count = 0;
		}
	}
	//Push remaining partial box
	if (box.size() != 0) {
		lock_guard<mutex> lock(mutex_order_outQ);
		anOrder.boxes.push_back(box);
	}
}

void Baker::beBaker() {
	//Baker keeps working until there are no orders and the waiter is done
	while (!b_WaiterIsFinished || !order_in_Q.empty()) {
		unique_lock<mutex> lock(mutex_order_inQ);

		//Wait if there are no orders but the waiter isn't done
		while (order_in_Q.size() == 0 && !b_WaiterIsFinished) {
			cv_order_inQ.wait(lock);
		}

		//MAKE DOUTS! definitely going to Krispy Kreme later
		ORDER order;
		if(!order_in_Q.empty()) {
			order = order_in_Q.front();
			order_in_Q.pop();
			bake_and_box(order);
			lock_guard<mutex> lock2(mutex_order_outQ);
			order_out_Vector.push_back(order);
		}
	}
}
