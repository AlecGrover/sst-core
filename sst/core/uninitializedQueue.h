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


#ifndef SST_UNINITIALIZEDQUEUE_H
#define SST_UNINITIALIZEDQUEUE_H

#include <sst/core/activityQueue.h>
#include <sst/core/serialization.h>


namespace SST {

class UninitializedQueue : public ActivityQueue {
public:
    UninitializedQueue(std::string message);
    UninitializedQueue(); // Only used for serialization
    ~UninitializedQueue();

    bool empty();
    int size();
    void insert(Activity* activity);
    Activity* pop();
    Activity* front();
    
    
private:
    std::string message;

    friend class boost::serialization::access;
    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version )
    {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ActivityQueue);
        ar & BOOST_SERIALIZATION_NVP(message);
    }
};

}

BOOST_CLASS_EXPORT_KEY(SST::UninitializedQueue)

#endif // SST_UNINITIALIZEDQUEUE_H
