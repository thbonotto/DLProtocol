/*
 * Notifier.h
 *
 *  Created on: 25 de nov de 2015
 *      Author: aluno
 */

#ifndef NOTIFIER_H_
#define NOTIFIER_H_

#include "DataLinkProtocol.h"
#include <iostream>
#include <string>
#include <vector>


namespace ptc {

class Notifier : public NotifyCallback{
public:

	void operator() (std::string& recv){
		std::cout << recv << std::endl;
	}
	virtual ~Notifier();
};
}
#endif /* NOTIFIER_H_ */
