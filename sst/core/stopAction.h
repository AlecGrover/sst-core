// Copyright 2009-2013 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2013, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.


#ifndef _SST_STOPACTION_H
#define _SST_STOPACTION_H

#include <sst/core/action.h>
#include <iostream>

namespace SST {

class StopAction : public Action
{
private:

    std::string message;
    bool print_message;
    
public:
    StopAction() {
	setPriority(1);
	print_message = false;
    }

    StopAction(std::string msg) {
	setPriority(1);
	print_message = true;
	message = msg;
    }

    void execute() {
	if ( print_message ) {
	    std::cout << message << std::endl;
	}
	endSimulation();
    }

    void print(const std::string &header, Output &out) const {
        out.output("%s StopAction to be delivered at %"PRIu64"\n", header.c_str(), getDeliveryTime());
    }

    friend class boost::serialization::access;
    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version )
    {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Action);
        ar & BOOST_SERIALIZATION_NVP(message);
        ar & BOOST_SERIALIZATION_NVP(print_message);
    }
};

} // namespace SST

#endif
